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
// Deinterlace.cpp
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
CDeinterlace::CDeinterlace(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr) :
    CTransformFilter(tszName, punk, CLSID_Deinterlace),
    m_DeinterlaceType(IDC_WEAVE),
    CPersistStream(punk, phr)
{
}


/////////////////////////////////////////////////////////////////////////////
// CreateInstance
// Provide the way for COM to create a Deinterlace object
/////////////////////////////////////////////////////////////////////////////
CUnknown *CDeinterlace::CreateInstance(LPUNKNOWN punk, HRESULT *phr)
{
    CDeinterlace *pNewObject = new CDeinterlace(NAME("Deinterlace Filer"), punk, phr);
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
	for (int i=0;i<MAX_FRAMES_IN_HISTORY;i++)
	{
		if (m_pInputHistory[i])
			m_pInputHistory[i]->Release();
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

HRESULT CDeinterlace::Transform(IMediaSample *pSource, IMediaSample *pDest)
{
	CAutoLock l(&m_DeinterlaceLock);
	int i;

    // Copy the properties across
//	HRESULT hr = CopyProperties(pSource, pDest);
//	if (FAILED(hr))
//	{
//		return hr;
//	}

	if (m_pInputHistory[0])
		m_pInputHistory[0]->Release();
	for (i=1;i<MAX_FRAMES_IN_HISTORY;i++)
		m_pInputHistory[i-1] = m_pInputHistory[i];
	m_pInputHistory[MAX_FRAMES_IN_HISTORY-1] = pSource;
	pSource->AddRef();
	
	BYTE *pPrevSourceBuffer, *pSourceBuffer, *pDestBuffer;
	long lSourceSize = pSource->GetSize();
	long lDestSize	= pDest->GetSize();

	ASSERT(lDestSize >= lSourceSize);

	pPrevSourceBuffer = NULL;
	if (m_pInputHistory[MAX_FRAMES_IN_HISTORY-2])
		m_pInputHistory[MAX_FRAMES_IN_HISTORY-2]->GetPointer(&pPrevSourceBuffer);

	pSource->GetPointer(&pSourceBuffer);
	pDest->GetPointer(&pDestBuffer);

    
	AM_MEDIA_TYPE* pType = &m_pInput->CurrentMediaType();
    VIDEOINFOHEADER2 *pvi = (VIDEOINFOHEADER2 *) pType->pbFormat;

	AM_MEDIA_TYPE* pTypeOut = &m_pOutput->CurrentMediaType();
    VIDEOINFOHEADER2 *pviOut = (VIDEOINFOHEADER2 *) pTypeOut->pbFormat;

    // Get the image properties from the BITMAPINFOHEADER
    int iPixelSize = pvi->bmiHeader.biBitCount / 8;
    int cxImage    = pvi->bmiHeader.biWidth;
    int cyImage    = pvi->bmiHeader.biHeight;
    int cbImage    = cyImage * cxImage * iPixelSize;
    int numPixels  = cxImage * cyImage;
    int Line;

	DEINTERLACE_INFO info;

	short *OddLines[2][2048];
	short *EvenLines[2][2048];

	info.OverlayPitch = pviOut->bmiHeader.biWidth*pviOut->bmiHeader.biBitCount/8;
	info.LineLength = pvi->bmiHeader.biWidth*pvi->bmiHeader.biBitCount/8;
	info.FrameWidth = pvi->bmiHeader.biWidth;
	info.FrameHeight = pvi->bmiHeader.biHeight;
	info.FieldHeight = pvi->bmiHeader.biHeight / 2;

	for (i=0;i<info.FieldHeight;i++)
	{
		EvenLines[0][i] = (short*)(pSourceBuffer+(2*i)*info.LineLength);
		OddLines[0][i] = (short*)(pSourceBuffer+(2*i+1)*info.LineLength);
		EvenLines[1][i] = (short*)(pPrevSourceBuffer+(2*i)*info.LineLength);
		OddLines[1][i] = (short*)(pPrevSourceBuffer+(2*i+1)*info.LineLength);
	}
	info.OddLines[0] = OddLines[0];
	info.EvenLines[0] = EvenLines[0];
	info.OddLines[1] = OddLines[1];
	info.EvenLines[1] = EvenLines[1];
	info.Overlay = pDestBuffer;
	info.IsOdd = FALSE;

    switch (m_DeinterlaceType)
    {
	case IDC_WEAVE:
		// doesn't do anything
		// I assume that this is what was done by the capture filter
		CopyMemory((PVOID) pDestBuffer, (PVOID) pSourceBuffer, lSourceSize);
		break;
	case IDC_BOB:
		Bob(&info);
		break;
	case IDC_TYPE1:
		// Let it be two frame
		if (pPrevSourceBuffer)
			DeinterlaceFieldTwoFrame(&info);
		break;
	case IDC_TYPE2:
		// Let it be blended clipping
		if (pPrevSourceBuffer)
			BlendedClipping(&info);
		break;
	case IDC_TYPE3:
		if (pPrevSourceBuffer)
			DeinterlaceFieldBob(&info);
        break;
    }

	CComQIPtr<IMediaSample2,&IID_IMediaSample2> pMediaSample2(pSource);
	if (!(!pMediaSample2)) {
		AM_SAMPLE2_PROPERTIES Props;
		if (SUCCEEDED(pMediaSample2->GetProperties(sizeof(Props), (PBYTE)&Props))) {
			DbgLog((LOG_TRACE,2,TEXT("Frame Type: %s, %s, "),
				(Props.dwSampleFlags & AM_VIDEO_FLAG_I_SAMPLE ? "I" : (Props.dwSampleFlags & AM_VIDEO_FLAG_P_SAMPLE ? "P" : (Props.dwSampleFlags & AM_VIDEO_FLAG_B_SAMPLE ? "B" : "?"))),
				(Props.dwSampleFlags & AM_VIDEO_FLAG_INTERLEAVED_FRAME ? "interleaved" : (Props.dwSampleFlags & AM_VIDEO_FLAG_FIELD1 ? "field 1" : (Props.dwSampleFlags & AM_VIDEO_FLAG_FIELD2 ? "field 2" : "?"))),
				(Props.dwSampleFlags & AM_VIDEO_FLAG_FIELD1FIRST ? "first field first" : ""),
				(Props.dwSampleFlags & AM_VIDEO_FLAG_REPEAT_FIELD ? "repeat first field" : "") ));
		}
	}
    return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// Copy
// Make destination properties the same as source
/////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////
// CheckInputType
// Check the input type is OK - return an error otherwise
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlace::CheckInputType(const CMediaType *mtIn)
{
	// check this is a VIDEOINFOHEADER type
	if (/*(*mtIn->FormatType() != FORMAT_VideoInfo) &&*/
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

    pProperties->cBuffers = 1;
    pProperties->cbBuffer = m_pInput->CurrentMediaType().GetSampleSize();
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
#pragma message (REMIND("Need to use upstream filter enumerator and act only on supported mediatypes"))

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

        if (IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB24)) {
            return (pbi->biBitCount == 24);
        }
		else if(IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_YUY2)) {
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
		if (Props.cbBuffer < CurrentMediaType().GetSampleSize())
			Props.cbBuffer = CurrentMediaType().GetSampleSize();

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
	pProps->cBuffers = MAX_FRAMES_IN_HISTORY+1;

	return NOERROR;
} // GetAllocatorRequirements
