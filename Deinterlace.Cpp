/////////////////////////////////////////////////////////////////////////////
// $Id: Deinterlace.Cpp,v 1.7 2001-11-09 11:42:39 pgubanov Exp $
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000 John Adcock.  All rights reserved.
/////////////////////////////////////////////////////////////////////////////
//
//	This file is subject to the terms of the GNU General Public License as
//	published by the Free Software Foundation.  A copy of this license is
//	included with this software distribution in the file COPYING.txt.  If you
//	do not have a copy, you may obtain a copy by writing to the Free
//	Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
//	This software is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details
//
/////////////////////////////////////////////////////////////////////////////
// CVS Log
//
// $Log: not supported by cvs2svn $
// Revision 1.6  2001/11/01 11:04:19  adcockj
// Updated headers
// Checked in changes by Micheal Eskin and Hauppauge
//
/////////////////////////////////////////////////////////////////////////////
// Change Log
//
// Date          Developer             Changes
//
// 28 Jun 2000   John Adcock           File created
//
// 20 Sep 2001   Michael Eskin         Changed to support use with video from bt878
//                                     Driver 
//
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <atlbase.h>
#include <streams.h>
#include <dvdmedia.h>
#include <initguid.h>
#if (1100 > _MSC_VER)
#include <olectlid.h>
#else
#include <olectl.h>
#endif
#include "DeinterlaceGuids.h"
#include "IDeinterlace.h"
#include "DeinterlaceProperties.h"
#include "Deinterlace.h"
#include "resource.h"

#include "DI.h"


#define MAX_INPUT_LINES_PER_FIELD	2048

/////////////////////////////////////////////////////////////////////////////
// Setup information
// This is used when registering the filter
/////////////////////////////////////////////////////////////////////////////

const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
    &MEDIATYPE_Video,       // Major type
    &MEDIASUBTYPE_NULL      // Minor type
};

const AMOVIESETUP_PIN sudpPins[] =
{
    { L"Input",             // Pins string name
      FALSE,                // Is it rendered
      FALSE,                // Is it an output
      FALSE,                // Are we allowed none
      FALSE,                // And allowed many
      &CLSID_NULL,          // Connects to filter
      NULL,                 // Connects to pin
      1,                    // Number of types
      &sudPinTypes          // Pin information
    },
    { L"Output",            // Pins string name
      FALSE,                // Is it rendered
      TRUE,                 // Is it an output
      FALSE,                // Are we allowed none
      FALSE,                // And allowed many
      &CLSID_NULL,          // Connects to filter
      NULL,                 // Connects to pin
      1,                    // Number of types
      &sudPinTypes          // Pin information
    }
};

const AMOVIESETUP_FILTER sudDeinterlace =
{
    &CLSID_Deinterlace,     // Filter CLSID
    L"Deinterlace Filter",  // String name
    MERIT_DO_NOT_USE,       // Filter merit
    2,                      // Number of pins
    sudpPins                // Pin information
};


// List of class IDs and creator functions for the class factory. This
// provides the link between the OLE entry point in the DLL and an object
// being created. The class factory will call the static CreateInstance

CFactoryTemplate g_Templates[] = {
    { L"Deinterlace"
    , &CLSID_Deinterlace
    , CDeinterlace::CreateInstance
    , NULL
    , &sudDeinterlace }
  ,
    { L"Deinterlace Settings"
    , &CLSID_DeinterlacePropertyPage
    , CDeinterlaceProperties::CreateInstance }
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer
// Pass off to base classes
// We don;t need anything extra here
/////////////////////////////////////////////////////////////////////////////
STDAPI DllRegisterServer()
{
    // Create entry in HKEY_CLASSES_ROOT\Filter
    OLECHAR szCLSID[CHARS_IN_GUID];
    TCHAR achTemp[MAX_PATH];
    HKEY hKey;

    HRESULT hr = AMovieDllRegisterServer2( TRUE );
    if (SUCCEEDED(hr))
    {
        // Incompatible way to register:
        // ActiveMovie uses Filter\* tree to find its filters
        StringFromGUID2(*g_Templates[0].m_ClsID, szCLSID, CHARS_IN_GUID);
        wsprintf(achTemp, TEXT("Filter\\%ls"), szCLSID);
        // create key
        RegCreateKey(HKEY_CLASSES_ROOT, (LPCTSTR)achTemp, &hKey);
        RegCloseKey(hKey);
    }

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer
// Pass off to base classes
// We don;t need anything extra here
/////////////////////////////////////////////////////////////////////////////
STDAPI DllUnregisterServer()
{
    HRESULT hr = AMovieDllRegisterServer2( FALSE );
    if (SUCCEEDED(hr))
    {
        // Incompatible way to unregister:
        // Delete entry in HKEY_CLASSES_ROOT\Filter
        OLECHAR szCLSID[CHARS_IN_GUID];
        TCHAR achTemp[MAX_PATH];

        StringFromGUID2(*g_Templates[0].m_ClsID, szCLSID, CHARS_IN_GUID);
        wsprintf(achTemp, TEXT("Filter\\%ls"), szCLSID);
        // remove key
        RegDeleteKey(HKEY_CLASSES_ROOT, (LPCTSTR)achTemp);
    }

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDeinterlace Constructor
/////////////////////////////////////////////////////////////////////////////

//Tee changed IDC_WEAVE to IDC_TYPE1 Two Frame 
CDeinterlace::CDeinterlace(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr) :
    CTransformFilter(tszName, punk, CLSID_Deinterlace),
    m_DeinterlaceType(IDC_TYPE1),
    CPersistStream(punk, phr)
{
}


/////////////////////////////////////////////////////////////////////////////
// CreateInstance
// Provide the way for COM to create a Deinterlace object
/////////////////////////////////////////////////////////////////////////////
CUnknown *CDeinterlace::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{

#ifdef	_DEBUG
// uncomment to allow debug break point when filter is created
//	#pragma message (REMIND("INT 3 on entry for DEBUG included"))
//    __asm int 3
#endif

	CDeinterlace *pNewObject = new CDeinterlace(NAME("Deinterlace Filter"), punk, phr);
    if (pNewObject == NULL)
	{
        *phr = E_OUTOFMEMORY;
    }
    return pNewObject;
}

/////////////////////////////////////////////////////////////////////////////
// GetPin
/////////////////////////////////////////////////////////////////////////////
CBasePin *
CDeinterlace::GetPin(int n)
{
    HRESULT hr = S_OK;

    // Create an input pin if not already done

    if (m_pInput == NULL) {

        m_pInput = new CDeinterlaceInputPin( NAME("Deinterlace input pin")
                                            , this        // Owner filter
                                            , &hr         // Result code
                                            , L"Input"    // Pin name
                                            );

        // Constructor for CTransInPlaceInputPin can't fail
        ASSERT(SUCCEEDED(hr));
    }

    // Create an output pin if not already done

    if (m_pInput!=NULL && m_pOutput == NULL) {

        m_pOutput = new CTransformOutputPin( NAME("TransInPlace output pin")
                                              , this       // Owner filter
                                              , &hr        // Result code
                                              , L"Output"  // Pin name
                                              );

        // a failed return code should delete the object

        ASSERT(SUCCEEDED(hr));
        if (m_pOutput == NULL) {
            delete m_pInput;
            m_pInput = NULL;
        }
    }

    // Return the appropriate pin

    ASSERT (n>=0 && n<=1);
    if (n == 0) {
        return m_pInput;
    } else if (n==1) {
        return m_pOutput;
    } else {
        return NULL;
    }

} // GetPin

/////////////////////////////////////////////////////////////////////////////
// Start Streaming
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlace::StartStreaming()
{
	for (int i=0;i<MAX_FRAMES_IN_HISTORY;i++)
		m_pInputHistory[i] = NULL;

	return CTransformFilter::StartStreaming();
}

/////////////////////////////////////////////////////////////////////////////
// Stop Streaming
// Release anything we have created during processing
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlace::StopStreaming()
{
	for (int i=0;i<MAX_FRAMES_IN_HISTORY;i++) {
		m_pInputHistory[i] = NULL;
	}

	return CTransformFilter::StopStreaming();
}

/////////////////////////////////////////////////////////////////////////////
// NonDelegatingQueryInterface
// Reveals IDeinterlace, ISpecifyPropertyPages, IPersistStream
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDeinterlace::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
    CheckPointer(ppv,E_POINTER);

    if (riid == IID_IDeinterlace)
	{
        return GetInterface((IDeinterlace *) this, ppv);
    }
	else if (riid == IID_ISpecifyPropertyPages)
	{
        return GetInterface((ISpecifyPropertyPages *) this, ppv);
    }
	else if (riid == IID_IPersistStream)
	{
        return GetInterface((IPersistStream *) this, ppv);
    }
	else
	{
        return CTransformFilter::NonDelegatingQueryInterface(riid, ppv);
    }
}


  
/////////////////////////////////////////////////////////////////////////////
// Transform
// Actually do processing on data
// This is called from within the base classes
/////////////////////////////////////////////////////////////////////////////
long                PulldownThresholdLow = -2000;
long                PulldownThresholdHigh = 2000;
long                PulldownRepeatCount = 4;
long                PulldownRepeatCount2 = 2;
long                Threshold32Pulldown = 15;
long                ThresholdPulldownMismatch = 900;
long                ThresholdPulldownComb = 150;
long                PulldownSwitchInterval = 3000;
long                PulldownSwitchMax = 4;
long				StaticImageFieldCount = 100;
long				LowMotionFieldCount = 4;
BOOL                bAutoDetectMode = TRUE;
BOOL                bFallbackToVideo = TRUE;

long BitShift = 13;
long EdgeDetect = 625;
long JaggieThreshold = 73;
long DiffThreshold = 224;
long TemporalTolerance = 300;
long SpatialTolerance = 600;
long SimilarityThreshold = 25;

/////////////////////////////////////////////////////////////////////////////
// memcpyMMX
// Uses MMX instructions to move memory around
// does as much as we can in 64 byte chunks (128-byte on SSE machines)
// using MMX instructions
// then copies any extra bytes
// assumes there will be at least 64 bytes to copy
// This code was originally from Borg's bTV plugin SDK 
/////////////////////////////////////////////////////////////////////////////
void memcpyMMX(void *Dest, void *Src, size_t nBytes)
{
#ifdef USE_SSE
	// On SSE machines, we can use the 128-bit floating-point registers and
	// bypass write caching to copy a bit faster.  The destination has to be
	// 16-byte aligned.  
	if ((CpuFeatureFlags & FEATURE_SSE) && (((long) Dest) & 15) == 0)
	__asm {
		mov		esi, dword ptr[Src]
		mov		edi, dword ptr[Dest]
		mov		ecx, nBytes
		shr     ecx, 7                      // nBytes / 128
align 8
CopyLoopSSE:
		// movaps would be slightly more efficient but the capture data
		// isn't reliably 16-byte aligned.
		movups	xmm0, xmmword ptr[esi]
		movups	xmm1, xmmword ptr[esi+16*1]
		movups	xmm2, xmmword ptr[esi+16*2]
		movups	xmm3, xmmword ptr[esi+16*3]
		movups	xmm4, xmmword ptr[esi+16*4]
		movups	xmm5, xmmword ptr[esi+16*5]
		movups	xmm6, xmmword ptr[esi+16*6]
		movups	xmm7, xmmword ptr[esi+16*7]
		movntps	xmmword ptr[edi], xmm0
		movntps	xmmword ptr[edi+16*1], xmm1
		movntps	xmmword ptr[edi+16*2], xmm2
		movntps	xmmword ptr[edi+16*3], xmm3
		movntps	xmmword ptr[edi+16*4], xmm4
		movntps	xmmword ptr[edi+16*5], xmm5
		movntps	xmmword ptr[edi+16*6], xmm6
		movntps	xmmword ptr[edi+16*7], xmm7
		add		esi, 128
		add		edi, 128
		loop CopyLoopSSE
		mov		ecx, nBytes
		and     ecx, 127
		cmp     ecx, 0
		je EndCopyLoopSSE
align 8
CopyLoop2SSE:
		mov dl, byte ptr[esi] 
		mov byte ptr[edi], dl
		inc esi
		inc edi
		dec ecx
		jne near CopyLoop2SSE
EndCopyLoopSSE:
		emms
	}
	else
#endif /* USE_SSE */
	__asm {
		mov		esi, dword ptr[Src]
		mov		edi, dword ptr[Dest]
		mov		ecx, nBytes
		shr     ecx, 6                      // nBytes / 64
align 8
CopyLoop:
		movq	mm0, qword ptr[esi]
		movq	mm1, qword ptr[esi+8*1]
		movq	mm2, qword ptr[esi+8*2]
		movq	mm3, qword ptr[esi+8*3]
		movq	mm4, qword ptr[esi+8*4]
		movq	mm5, qword ptr[esi+8*5]
		movq	mm6, qword ptr[esi+8*6]
		movq	mm7, qword ptr[esi+8*7]
		movq	qword ptr[edi], mm0
		movq	qword ptr[edi+8*1], mm1
		movq	qword ptr[edi+8*2], mm2
		movq	qword ptr[edi+8*3], mm3
		movq	qword ptr[edi+8*4], mm4
		movq	qword ptr[edi+8*5], mm5
		movq	qword ptr[edi+8*6], mm6
		movq	qword ptr[edi+8*7], mm7
		add		esi, 64
		add		edi, 64
		loop CopyLoop
		mov		ecx, nBytes
		and     ecx, 63
		cmp     ecx, 0
		je EndCopyLoop
align 8
CopyLoop2:
		mov dl, byte ptr[esi] 
		mov byte ptr[edi], dl
		inc esi
		inc edi
		dec ecx
		jne near CopyLoop2
EndCopyLoop:
		emms
	}
}

// Set up our output sample
HRESULT
CDeinterlace::getOutputSampleBuffer(IMediaSample *pSample, IMediaSample **ppOutput)
{
    IMediaSample *pOutSample;

	// default - times are the same
	AM_SAMPLE2_PROPERTIES * const pProps = m_pInput->SampleProps();
	DWORD dwFlags = m_bSampleSkipped ? AM_GBF_PREVFRAMESKIPPED : 0;

	// This will prevent the image renderer from switching us to DirectDraw
	// when we can't do it without skipping frames because we're not on a
	// keyframe.  If it really has to switch us, it still will, but then we
	// will have to wait for the next keyframe
	if (!(pProps->dwSampleFlags & AM_SAMPLE_SPLICEPOINT)) {
		dwFlags |= AM_GBF_NOTASYNCPOINT;
	}

	HRESULT hr = m_pOutput->GetDeliveryBuffer(&pOutSample,
		pProps->dwSampleFlags & AM_SAMPLE_TIMEVALID ? &pProps->tStart : NULL,
		pProps->dwSampleFlags & AM_SAMPLE_STOPVALID ? &pProps->tStop : NULL,
		dwFlags);

    *ppOutput = pOutSample;
    if (FAILED(hr)) {
		*ppOutput = NULL;
        return hr;
    }

	//
	// Check if output media type has been changed.
	// Downstream filter may ask us to output data to
	// a larger bitmap - so usually does Overlay Mixer,
	// or to a different format - so does Video Renderer.
	// Since we accept only YUY2, we primarily take care of
	// the former.
	//
	AM_MEDIA_TYPE *pmt;
	pOutSample->GetMediaType(&pmt);
	if (pmt != NULL) {
		CMediaType cmt(*pmt);
		DeleteMediaType(pmt);
		m_pOutput->SetMediaType(&cmt);
	}

	//
	// Copy IMediaSample2 properties
	//
    ASSERT(pOutSample);
    IMediaSample2 *pOutSample2;
    if (SUCCEEDED(pOutSample->QueryInterface(IID_IMediaSample2,
                                             (void **)&pOutSample2))) {
        //  Modify it
        AM_SAMPLE2_PROPERTIES OutProps;
        EXECUTE_ASSERT(SUCCEEDED(pOutSample2->GetProperties(
            FIELD_OFFSET(AM_SAMPLE2_PROPERTIES, tStart), (PBYTE)&OutProps)
        ));
        OutProps.dwTypeSpecificFlags = pProps->dwTypeSpecificFlags;
        OutProps.dwSampleFlags =
            (OutProps.dwSampleFlags & AM_SAMPLE_TYPECHANGED) |
            (pProps->dwSampleFlags & ~AM_SAMPLE_TYPECHANGED);
        OutProps.tStart = pProps->tStart;
        OutProps.tStop  = pProps->tStop;
        OutProps.cbData = FIELD_OFFSET(AM_SAMPLE2_PROPERTIES, dwStreamId);
        hr = pOutSample2->SetProperties(
            FIELD_OFFSET(AM_SAMPLE2_PROPERTIES, dwStreamId),
            (PBYTE)&OutProps
        );
        if (pProps->dwSampleFlags & AM_SAMPLE_DATADISCONTINUITY) {
            m_bSampleSkipped = FALSE;
        }
        pOutSample2->Release();
    } else {
        if (pProps->dwSampleFlags & AM_SAMPLE_TIMEVALID) {
            pOutSample->SetTime(&pProps->tStart,
                                &pProps->tStop);
        }
        if (pProps->dwSampleFlags & AM_SAMPLE_SPLICEPOINT) {
            pOutSample->SetSyncPoint(TRUE);
        }
        if (pProps->dwSampleFlags & AM_SAMPLE_DATADISCONTINUITY) {
            pOutSample->SetDiscontinuity(TRUE);
            m_bSampleSkipped = FALSE;
        }

        // Copy the media times
		// Will be updated later if needed
        LONGLONG MediaStart, MediaEnd;
        if (pSample->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR) {
            pOutSample->SetMediaTime(&MediaStart,&MediaEnd);
        }
    }
    return S_OK;
}

//
// Process input samples
//
HRESULT
CDeinterlace::Receive(IMediaSample *pSample)
{
    //
	// Check for other streams and pass them on
	//
    AM_SAMPLE2_PROPERTIES * const pProps = m_pInput->SampleProps();
    if (pProps->dwStreamId != AM_STREAM_MEDIA) {
        return m_pOutput->Deliver(pSample);
    }
    HRESULT hr;
    ASSERT(pSample);

    // If no output to deliver to then no point sending us data
    ASSERT (m_pOutput != NULL) ;

    // Process input sample and deliver output sample(s)
    hr = deinterlace(pSample);

    return hr;
}

//
// Deinterlace input frames/fields and generate output samples
//
HRESULT
CDeinterlace::deinterlace(IMediaSample *pSource)
{
	CAutoLock l(&m_DeinterlaceLock);
	int i;

	//
	// Release the sample we don't need any more
	//
	if (m_pInputHistory[0])
		m_pInputHistory[0] = NULL;

	//
	// Shift the history buffer and push the new sample
	//
	for (i=1;i<MAX_FRAMES_IN_HISTORY;i++)
		m_pInputHistory[i-1] = m_pInputHistory[i];
	m_pInputHistory[MAX_FRAMES_IN_HISTORY-1] = pSource;
	
	//
	// Output buffer pointers and sizes
	//
	BYTE *pPrevSourceBuffer, *pSourceBuffer;
	long lSourceSize = pSource->GetSize();

	pPrevSourceBuffer = NULL;
	if (m_pInputHistory[MAX_FRAMES_IN_HISTORY-2])
		m_pInputHistory[MAX_FRAMES_IN_HISTORY-2]->GetPointer(&pPrevSourceBuffer);

	pSource->GetPointer(&pSourceBuffer);

	//
	// Get input BITMAPINFOHEADER and rectangle
	// 
	LPBITMAPINFOHEADER pbiIn;
	LPRECT prcIn;
	if (IsEqualGUID(*m_pInput->CurrentMediaType().FormatType(), FORMAT_VideoInfo)) {
		pbiIn = &(((VIDEOINFOHEADER *)m_pInput->CurrentMediaType().Format())->bmiHeader);
		prcIn = &(((VIDEOINFOHEADER *)m_pInput->CurrentMediaType().Format())->rcSource);
	}
	else if (IsEqualGUID(*m_pInput->CurrentMediaType().FormatType(), FORMAT_VIDEOINFO2)) {
		pbiIn = &(((VIDEOINFOHEADER2 *)m_pInput->CurrentMediaType().Format())->bmiHeader);
		prcIn = &(((VIDEOINFOHEADER *)m_pInput->CurrentMediaType().Format())->rcSource);
	}

	//
    // Get the image properties from the BITMAPINFOHEADER
	//
    int iPixelSize = pbiIn->biBitCount / 8;
    int cxImage    = pbiIn->biWidth;
    int cyImage    = pbiIn->biHeight;
    int cbImage    = cyImage * cxImage * iPixelSize;
    int numPixels  = cxImage * cyImage;

	//
	// Initialize a deinterlacer method
	//
	DEINTERLACE_INFO info;

	short *OddLines[2][MAX_INPUT_LINES_PER_FIELD];
	short *EvenLines[2][MAX_INPUT_LINES_PER_FIELD];

	//
	// Generate output buffer pointers
	//
	info.LineLength = pbiIn->biWidth*pbiIn->biBitCount/8;
	info.FrameWidth = pbiIn->biWidth;
	info.FrameHeight = pbiIn->biHeight;
#pragma message("REMIND(Assume frame pictures on input)")
	info.FieldHeight = pbiIn->biHeight / 2;
	if (info.FieldHeight > MAX_INPUT_LINES_PER_FIELD) {
		info.FieldHeight = MAX_INPUT_LINES_PER_FIELD;
	}

	for (i=0;i<info.FieldHeight;i++) {
		EvenLines[0][i] = (short*)(pSourceBuffer+(2*i)*info.LineLength);
		OddLines[0][i] = (short*)(pSourceBuffer+(2*i+1)*info.LineLength);
		EvenLines[1][i] = (short*)(pPrevSourceBuffer+(2*i)*info.LineLength);
		OddLines[1][i] = (short*)(pPrevSourceBuffer+(2*i+1)*info.LineLength);
	}
	info.OddLines[0] = OddLines[0];
	info.EvenLines[0] = EvenLines[0];
	info.OddLines[1] = OddLines[1];
	info.EvenLines[1] = EvenLines[1];

	//
	// Call deinterlace method and deliver samples
	//
	// Assume frame input and call deinterlace method
	// twice for each sample thus producing two
	// output frames
	//


	//
	// Generate first field
	//
	do {
		CComPtr<IMediaSample> pDest;

		HRESULT hr = getOutputSampleBuffer(pSource,&pDest);
		if (FAILED(hr))
			return NOERROR;

		long lDestSize = pDest->GetSize();
		BYTE *pDestBuffer;
		pDest->GetPointer(&pDestBuffer);

		//
		// Get output BITMAPINFOHEADER and rectangle
		//
		LPBITMAPINFOHEADER pbiOut;
		LPRECT prcOut;
		if (IsEqualGUID(*m_pOutput->CurrentMediaType().FormatType(), FORMAT_VideoInfo)) {
			pbiOut = &(((VIDEOINFOHEADER *)m_pOutput->CurrentMediaType().Format())->bmiHeader);
			prcOut = &(((VIDEOINFOHEADER *)m_pOutput->CurrentMediaType().Format())->rcTarget);
		}
		else if (IsEqualGUID(*m_pOutput->CurrentMediaType().FormatType(), FORMAT_VIDEOINFO2)) {
			pbiOut = &(((VIDEOINFOHEADER2 *)m_pOutput->CurrentMediaType().Format())->bmiHeader);
			prcOut = &(((VIDEOINFOHEADER *)m_pOutput->CurrentMediaType().Format())->rcTarget);
		}

		info.OverlayPitch = pbiOut->biWidth*pbiOut->biBitCount/8;
		info.Overlay = pDestBuffer;
		info.IsOdd = FALSE;
		callDeinterlaceMethod(&info,pPrevSourceBuffer != NULL);

		// Time for first field
		REFERENCE_TIME rtStart, rtStop;
		if (S_OK == pSource->GetTime(&rtStart,&rtStop)) {
			rtStop = rtStart + (rtStop - rtStart)/2;
			pDest->SetTime(&rtStart,&rtStop);
		}

		m_pOutput->Deliver(pDest);
	} while(0);

	//
	// Generate second field
	//
	do {
		CComPtr<IMediaSample> pDest;

		HRESULT hr = getOutputSampleBuffer(pSource,&pDest);
		if (FAILED(hr))
			return NOERROR;

		long lDestSize = pDest->GetSize();
		BYTE *pDestBuffer;
		pDest->GetPointer(&pDestBuffer);

		//
		// Get output BITMAPINFOHEADER and rectangle
		//
		LPBITMAPINFOHEADER pbiOut;
		LPRECT prcOut;
		if (IsEqualGUID(*m_pOutput->CurrentMediaType().FormatType(), FORMAT_VideoInfo)) {
			pbiOut = &(((VIDEOINFOHEADER *)m_pOutput->CurrentMediaType().Format())->bmiHeader);
			prcOut = &(((VIDEOINFOHEADER *)m_pOutput->CurrentMediaType().Format())->rcTarget);
		}
		else if (IsEqualGUID(*m_pOutput->CurrentMediaType().FormatType(), FORMAT_VIDEOINFO2)) {
			pbiOut = &(((VIDEOINFOHEADER2 *)m_pOutput->CurrentMediaType().Format())->bmiHeader);
			prcOut = &(((VIDEOINFOHEADER *)m_pOutput->CurrentMediaType().Format())->rcTarget);
		}

		info.OverlayPitch = pbiOut->biWidth*pbiOut->biBitCount/8;
		info.Overlay = pDestBuffer;
		info.IsOdd = TRUE;
		callDeinterlaceMethod(&info,pPrevSourceBuffer != NULL);

		// Time for second field
		REFERENCE_TIME rtStart, rtStop;
		if (S_OK == pSource->GetTime(&rtStart,&rtStop)) {
			rtStart = rtStart + (rtStop - rtStart + 1)/2;
			pDest->SetTime(&rtStart,&rtStop);
		}

		m_pOutput->Deliver(pDest);
	} while(0);

    return NOERROR;
}

//
// Call apropriate deinterlace method
//
void
CDeinterlace::callDeinterlaceMethod(DEINTERLACE_INFO *pInfo,BOOL fHistoryValid) const
{
    switch (m_DeinterlaceType) {
	case IDC_WEAVE:
	default:
		Weave(pInfo);
		break;
	case IDC_BOB:
		Bob(pInfo);
		break;
	case IDC_TYPE1:
		// Let it be two frame
		if (fHistoryValid)
			DeinterlaceFieldTwoFrame(pInfo);
		else
			Bob(pInfo);
		break;
	case IDC_TYPE2:
		// Let it be blended clipping
		if (fHistoryValid)
			BlendedClipping(pInfo);
		else
			Bob(pInfo);
		break;
	case IDC_TYPE3:
		if (fHistoryValid)
			DeinterlaceFieldBob(pInfo);
		else
			Bob(pInfo);
        break;
    }

}

/////////////////////////////////////////////////////////////////////////////
// Copy
// Make destination properties the same as source
/////////////////////////////////////////////////////////////////////////////
#if 0
HRESULT CDeinterlace::CopyProperties(IMediaSample *pSource, IMediaSample *pDest) const
{
    // Copy the sample times

    REFERENCE_TIME TimeStart, TimeEnd;
    if (NOERROR == pSource->GetTime(&TimeStart, &TimeEnd))
	{
        pDest->SetTime(&TimeStart, &TimeEnd);
    }

    LONGLONG MediaStart, MediaEnd;
    if (pSource->GetMediaTime(&MediaStart,&MediaEnd) == NOERROR)
	{
        pDest->SetMediaTime(&MediaStart,&MediaEnd);
    }

    // Copy the Sync point property

    HRESULT hr = pSource->IsSyncPoint();
    if (hr == S_OK)
	{
        pDest->SetSyncPoint(TRUE);
    }
    else if (hr == S_FALSE)
	{
        pDest->SetSyncPoint(FALSE);
    }
    else
	{  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the media type

    AM_MEDIA_TYPE *pMediaType;
    pSource->GetMediaType(&pMediaType);
    pDest->SetMediaType(pMediaType);
    DeleteMediaType(pMediaType);

    // Copy the preroll property

    hr = pSource->IsPreroll();
    if (hr == S_OK)
	{
        pDest->SetPreroll(TRUE);
    }
    else if (hr == S_FALSE)
	{
        pDest->SetPreroll(FALSE);
    }
    else
	{  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the discontinuity property

    hr = pSource->IsDiscontinuity();
    if (hr == S_OK)
	{
		pDest->SetDiscontinuity(TRUE);
    }
    else if (hr == S_FALSE)
	{
        pDest->SetDiscontinuity(FALSE);
    }
    else
	{  // an unexpected error has occured...
        return E_UNEXPECTED;
    }

    // Copy the actual data length
    long lDataLength = pSource->GetActualDataLength();
    pDest->SetActualDataLength(lDataLength);

    return NOERROR;

}
#endif

/////////////////////////////////////////////////////////////////////////////
// CheckInputType
// Check the input type is OK - return an error otherwise
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlace::CheckInputType(const CMediaType *mtIn)
{

//return NOERROR;

	// check this is a VIDEOINFOHEADER type // MAE Added VideoInfo back in
	if ((*mtIn->FormatType() != FORMAT_VideoInfo) && 
		(*mtIn->FormatType() != FORMAT_VIDEOINFO2))
	{
		return E_INVALIDARG;
	}

	// Can we transform this type

	if (CanPerformDeinterlace(mtIn))
	{
		return NOERROR;
	}
	return E_FAIL;
}


/////////////////////////////////////////////////////////////////////////////
// CheckTransform
// Check a transform can be done between these formats
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlace::CheckTransform(const CMediaType *pmtIn, const CMediaType *pmtOut)
{
    DbgLog((LOG_TRACE, 2, TEXT("CDeinterlace::CheckTransform")));

    // we only output video
    if (*pmtOut->Type() != MEDIATYPE_Video) {
        return VFW_E_TYPE_NOT_ACCEPTED;
    }

    // Check there is a format block
    if (*pmtOut->FormatType() != FORMAT_VideoInfo &&
		*pmtOut->FormatType() != FORMAT_VIDEOINFO2) {
        return VFW_E_TYPE_NOT_ACCEPTED;
    }

    if (!CanPerformDeinterlace(pmtIn))
	{
        return VFW_E_TYPE_NOT_ACCEPTED;
    }

    //
    // See if we can use dci/direct draw.
    // First check that there is a non empty target rectangle.
    //

	// Since we need only rectangles here, skip VIDEOINFOHEADER 2 cheking
    VIDEOINFO *videoInfo = (VIDEOINFO *)pmtOut->pbFormat;
	VIDEOINFO *videoInfoIn = (VIDEOINFO *)pmtIn->pbFormat;
    if (!IsRectEmpty(&videoInfo->rcTarget)) {

        //
        // Next, check that the source rectangle is the entire movie.
        //

        if ( videoInfo->rcSource.left   == 0
          && videoInfo->rcSource.top    == 0
          && videoInfo->rcSource.right  == (videoInfoIn->rcSource.right-videoInfoIn->rcSource.left)
          && videoInfo->rcSource.bottom == (videoInfoIn->rcSource.bottom-videoInfoIn->rcSource.top))
		{

            //
            // Now check that the target rectangles size is the same as
            // the movies, that is there is no stretching or shrinking.
            //

            if ( (videoInfo->rcTarget.right - videoInfo->rcTarget.left)
                    == (videoInfoIn->rcSource.right-videoInfoIn->rcSource.left)
              && (videoInfo->rcTarget.bottom - videoInfo->rcTarget.top)
                    == (videoInfoIn->rcSource.bottom-videoInfoIn->rcSource.top)) {
#ifndef _X86_
                // On Risc machines make sure we are DWORD aligned
                if ((videoInfo->rcTarget.left & 0x03) == 0x00)
#endif
                {
                    DbgLog((LOG_TRACE, 2, TEXT("Using DCI")));
                    return S_OK;
                }
            }
        }
        DbgLog((LOG_TRACE, 2, TEXT("NOT Using DCI")));
        return E_FAIL;
    }


    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// DecideBufferSize
// Tell the output pin's allocator what size buffers we
// require. Can only do this when the input is connected
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlace::DecideBufferSize(IMemAllocator *pAlloc,ALLOCATOR_PROPERTIES *pProperties)
{
    // Is the input pin connected

    if (m_pInput->IsConnected() == FALSE)
	{
        return E_UNEXPECTED;
    }

    ASSERT(pAlloc);
    ASSERT(pProperties);
    HRESULT hr = NOERROR;

    // Get input pin's allocator size and use that
    ALLOCATOR_PROPERTIES InProps;
    CComPtr<IMemAllocator> pInAlloc;
    hr = m_pInput->GetAllocator(&pInAlloc);
    if (SUCCEEDED (hr))
    {
        hr = pInAlloc->GetProperties (&InProps);
        if (SUCCEEDED (hr))
        {
            pProperties->cbBuffer = InProps.cbBuffer;
        }
    }


    pProperties->cBuffers = 1;

#if 0
    pProperties->cbBuffer = m_pInput->CurrentMediaType().GetSampleSize();
    pProperties->cbBuffer = m_pOutput->CurrentMediaType().GetSampleSize(); 
	pProperties->cbBuffer = 640*480*2;
#endif

    ASSERT(pProperties->cbBuffer);

    // Ask the allocator to reserve us some sample memory, NOTE the function
    // can succeed (that is return NOERROR) but still not have allocated the
    // memory that we requested, so we must check we got whatever we wanted

    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pProperties,&Actual);
    if (FAILED(hr))
	{
        return hr;
    }

    ASSERT( Actual.cBuffers == 1 );

    if (pProperties->cBuffers > Actual.cBuffers || pProperties->cbBuffer > Actual.cbBuffer)
	{
        return E_FAIL;
    }
    return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// GetMediaType
//
// I support one type, namely the type of the input pin
// This type is only available if my input is connected
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlace::GetMediaType(int iPosition, CMediaType *pMediaType)
{
    // Is the input pin connected
    if (m_pInput->IsConnected() == FALSE)
	{
        return E_UNEXPECTED;
    }

    // This should never happen
    if (iPosition < 0)
	{
        return E_INVALIDARG;
    }

    // Do we have more items to offer
    if (iPosition > 0)
	{
        return VFW_S_NO_MORE_ITEMS;
    }

    // copy format from input pin
	CMediaType cmt;
	cmt = m_pInput->CurrentMediaType();
	*pMediaType = cmt;

    return NOERROR;

}


/////////////////////////////////////////////////////////////////////////////
// CanPerformDeinterlace
// Check if this is one of out supported formats and data has 24 bits
/////////////////////////////////////////////////////////////////////////////
BOOL CDeinterlace::CanPerformDeinterlace(const CMediaType *pMediaType) const
{
    if (IsEqualGUID(*pMediaType->Type(), MEDIATYPE_Video))
	{
		LPBITMAPINFOHEADER pbi;
		if (IsEqualGUID(*pMediaType->FormatType(), FORMAT_VideoInfo)) {
			pbi = &(((VIDEOINFOHEADER *)pMediaType->Format())->bmiHeader);
		}
		else if (IsEqualGUID(*pMediaType->FormatType(), FORMAT_VIDEOINFO2)) {
			pbi = &(((VIDEOINFOHEADER2 *)pMediaType->Format())->bmiHeader);
		}

//
// Some deinterlace routines assume the input is YUY2
// and produce incorrect results on RGB input
//
//        if (IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB24)) {
//            return (pbi->biBitCount == 24);
//        }
//		else 
		if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_YUY2)) {
			return (pbi->biBitCount == 16);
		}
    }
    return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// GetClassID
// This is the only method of IPersist
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDeinterlace::GetClassID(CLSID *pClsid)
{
    return CBaseFilter::GetClassID(pClsid);
}


/////////////////////////////////////////////////////////////////////////////
// ScribbleToStream
// Overriden to write our state into a stream
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlace::WriteToStream(IStream *pStream)
{
    HRESULT hr;
    WRITEOUT(m_DeinterlaceType);
    return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// ReadFromStream
// Likewise overriden to restore our state from a stream
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlace::ReadFromStream(IStream *pStream)
{
    HRESULT hr;
    READIN(m_DeinterlaceType);
    return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// GetPages
// Returns the clsid's of the property pages we support
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDeinterlace::GetPages(CAUUID *pPages)
{
    pPages->cElems = 1;
    pPages->pElems = (GUID *) CoTaskMemAlloc(sizeof(GUID));
    if (pPages->pElems == NULL)
	{
        return E_OUTOFMEMORY;
    }
    *(pPages->pElems) = CLSID_DeinterlacePropertyPage;
    return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// get_DeinterlaceType
// Return the current effect selected
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDeinterlace::get_DeinterlaceType(int *piType)
{
    CAutoLock cAutolock(&m_DeinterlaceLock);
    CheckPointer(piType,E_POINTER);
    *piType = m_DeinterlaceType;
    return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// put_DeinterlaceType
// Set the required video effect
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDeinterlace::put_DeinterlaceType(int iType)
{
    CAutoLock cAutolock(&m_DeinterlaceLock);
    m_DeinterlaceType = iType;
    SetDirty(TRUE);
    return NOERROR;
}

// =================================================================
// Implements the CDeinterlaceInputPin class
// =================================================================


// constructor

CDeinterlaceInputPin::CDeinterlaceInputPin
    ( TCHAR               *pObjectName
    , CDeinterlace        *pFilter
    , HRESULT             *phr
    , LPCWSTR              pName
    )
    : CTransformInputPin(pObjectName,
                         pFilter,
                         phr,
                         pName)
{
    DbgLog((LOG_TRACE, 2
           , TEXT("CDeinterlaceInputPin::CDeinterlaceInputPin")));

	m_pDeinterlaceFilter = pFilter;
} // constructor

// =================================================================
// Implements IMemInputPin interface
// =================================================================


// If the downstream filter has one then offer that (even if our own output
// pin is not using it yet.  If the upstream filter chooses it then we will
// tell our output pin to ReceiveAllocator).
// Else if our output pin is using an allocator then offer that.
//     ( This could mean offering the upstream filter his own allocator,
//       it could mean offerring our own
//     ) or it could mean offering the one from downstream
// Else fail to offer any allocator at all.

STDMETHODIMP CDeinterlaceInputPin::GetAllocator(IMemAllocator ** ppAllocator)
{
	if (!IsConnected())
		return VFW_E_NO_ALLOCATOR;

	HRESULT hr = CTransformInputPin::GetAllocator(ppAllocator);

	if (SUCCEEDED(hr)) {
		ALLOCATOR_PROPERTIES Props, Actual;
		(*ppAllocator)->GetProperties(&Props);
		if (Props.cBuffers < MAX_FRAMES_IN_HISTORY+1)
			Props.cBuffers = MAX_FRAMES_IN_HISTORY+1;
		if (Props.cbAlign == 0)
			Props.cbAlign = 1;
		if (Props.cbBuffer < (long)CurrentMediaType().GetSampleSize())
			Props.cbBuffer = (long)CurrentMediaType().GetSampleSize();

		hr = (*ppAllocator)->SetProperties(&Props,&Actual);
		if (SUCCEEDED(hr) && Actual.cBuffers < MAX_FRAMES_IN_HISTORY+1)
			hr = E_FAIL;
	}

    return hr;

} // GetAllocator



/* Get told which allocator the upstream output pin is actually going to use */

STDMETHODIMP
CDeinterlaceInputPin::NotifyAllocator(
    IMemAllocator * pAllocator,
    BOOL bReadOnly)
{
	HRESULT hr = CTransformInputPin::NotifyAllocator(pAllocator,bReadOnly);
	if (SUCCEEDED(hr)) {
		ALLOCATOR_PROPERTIES Props;
		m_pAllocator->GetProperties(&Props);
		if (Props.cBuffers < MAX_FRAMES_IN_HISTORY+1)
			hr = E_FAIL;
	}

    return hr;

} // NotifyAllocator

/////////////////////////////////////////////////////////////////////////////
// GetAllocatorRequirements
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP
CDeinterlaceInputPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *pProps)
{
    DbgLog((LOG_TRACE, 2
           , TEXT("CDeinterlaceInputPin::GetAllocatorRequirements")));

	pProps->cbAlign = 0;
	pProps->cbBuffer = 0;
	pProps->cbPrefix = 0;
	pProps->cBuffers = MAX_FRAMES_IN_HISTORY+4;  // JBC 9/20/01 we drop frames if to few buffers
	
	return NOERROR;
} // GetAllocatorRequirements
