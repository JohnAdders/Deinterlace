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
#include <streams.h>
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
    return AMovieDllRegisterServer2(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer
// Pass off to base classes
// We don;t need anything extra here
/////////////////////////////////////////////////////////////////////////////
STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2(FALSE);
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
    CDeinterlace *pNewObject = new CDeinterlace(NAME("Image Effects"), punk, phr);
    if (pNewObject == NULL)
	{
        *phr = E_OUTOFMEMORY;
    }
    return pNewObject;
}

/////////////////////////////////////////////////////////////////////////////
// Stop Streaming
// Release anything we have created during processing
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlace::StopStreaming()
{
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
HRESULT CDeinterlace::Transform(IMediaSample *pSource, IMediaSample *pDest)
{
    // Copy the properties across
    HRESULT hr = CopyProperties(pSource, pDest);
    if (FAILED(hr))
	{
        return hr;
    }

	BYTE *pSourceBuffer, *pDestBuffer;
	long lSourceSize = pSource->GetActualDataLength();
	long lDestSize	= pDest->GetSize();

	ASSERT(lDestSize >= lSourceSize);

	pSource->GetPointer(&pSourceBuffer);
	pDest->GetPointer(&pDestBuffer);

    
	AM_MEDIA_TYPE* pType = &m_pInput->CurrentMediaType();
    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) pType->pbFormat;


    // Get the image properties from the BITMAPINFOHEADER
    int iPixelSize = pvi->bmiHeader.biBitCount / 8;
    int cxImage    = pvi->bmiHeader.biWidth;
    int cyImage    = pvi->bmiHeader.biHeight;
    int cbImage    = cyImage * cxImage * iPixelSize;
    int numPixels  = cxImage * cyImage;
    int Line;

    switch (m_DeinterlaceType)
    {
	case IDC_WEAVE:
		// doesn't do anything
		// I assume that this is what was done by the capture filter
		CopyMemory((PVOID) pDestBuffer, (PVOID) pSourceBuffer, lSourceSize);
		break;
	case IDC_BOB:
		// loop through the lines two at a time
		// copy lower line of each pair to both upper and lower
		for (Line = 1; Line < cyImage; ++Line, ++Line)
		{
			CopyMemory(pDestBuffer + (Line - 1) * cxImage * iPixelSize, pSourceBuffer + Line * cxImage * iPixelSize, cxImage * iPixelSize);
			CopyMemory(pDestBuffer + Line * cxImage * iPixelSize, pSourceBuffer + Line * cxImage * iPixelSize, cxImage * iPixelSize);
		}
		break;
	case IDC_TYPE1:
		// go for it .....
		// the current frame is in pSource
		// you may need to save some data to do motion estimation
		// do your worst....
		CopyMemory((PVOID) pDestBuffer, (PVOID) pSourceBuffer, lSourceSize);
		break;
	case IDC_TYPE2:
		CopyMemory((PVOID) pDestBuffer, (PVOID) pSourceBuffer, lSourceSize);
		break;
	case IDC_TYPE3:
		CopyMemory((PVOID) pDestBuffer, (PVOID) pSourceBuffer, lSourceSize);
        break;
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
	if (*mtIn->FormatType() != FORMAT_VideoInfo)
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
HRESULT CDeinterlace::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
    if (CanPerformDeinterlace(mtIn))
	{
        if (*mtIn == *mtOut)
		{
            return NOERROR;
        }
    }
    return E_FAIL;
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
    *pMediaType = m_pInput->CurrentMediaType();
	VIDEOINFOHEADER* pFormat = (VIDEOINFOHEADER*)pMediaType->Format();
	pMediaType->SetFormat((BYTE*)pFormat, sizeof(VIDEOINFOHEADER));
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
        if (IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_RGB24) ||
			IsEqualGUID(*pMediaType->Subtype(), MEDIASUBTYPE_YUY2))
		{
            VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) pMediaType->Format();
            return (pvi->bmiHeader.biBitCount == 24);
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
STDMETHODIMP CDeinterlace::get_DeinterlaceType(int *IPEffect)
{
    CAutoLock cAutolock(&m_DeinterlaceLock);
    CheckPointer(IPEffect,E_POINTER);
    *IPEffect = m_DeinterlaceType;
    return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// put_DeinterlaceType
// Set the required video effect
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDeinterlace::put_DeinterlaceType(int IPEffect)
{
    CAutoLock cAutolock(&m_DeinterlaceLock);
    m_DeinterlaceType = IPEffect;
    SetDirty(TRUE);
    return NOERROR;
}

