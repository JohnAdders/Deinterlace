/////////////////////////////////////////////////////////////////////////////
// $Id: Deinterlace.h,v 1.4 2001-11-09 11:42:38 pgubanov Exp $
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
// Deinterlace.h
/////////////////////////////////////////////////////////////////////////////

#ifndef _DEINTERLACE_H
#define _DEINTERLACE_H

#include <atlbase.h>

#include "DI.h"

#define MAX_FRAMES_IN_HISTORY	2

class CDeinterlace : public CTransformFilter,
		 public IDeinterlace,
		 public ISpecifyPropertyPages,
		 public CPersistStream
{

public:

    DECLARE_IUNKNOWN;
    static CUnknown * WINAPI CreateInstance(LPUNKNOWN punk, HRESULT *phr);

    // Reveals IDeinterlace, ISpecifyPropertyPages, IPersistStream
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

    // CPersistStream stuff
    HRESULT WriteToStream(IStream *pStream);
    HRESULT ReadFromStream(IStream *pStream);
    STDMETHODIMP GetClassID(CLSID *pClsid);

	CBasePin *GetPin(int n);

    // Overrriden from CTransformFilter base class
	HRESULT Receive(IMediaSample *pSample);
    HRESULT CheckInputType(const CMediaType *mtIn);
    HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
    HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties);
    HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
    HRESULT StartStreaming();
    HRESULT StopStreaming();

	// These implement the custom IIPEffect interface
    STDMETHODIMP get_DeinterlaceType(int *IPEffect);
    STDMETHODIMP put_DeinterlaceType(int IPEffect);

    // ISpecifyPropertyPages interface
    STDMETHODIMP GetPages(CAUUID *pPages);


private:
    // Constructor
    CDeinterlace(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);

    BOOL CanPerformDeinterlace(const CMediaType *pMediaType) const;
#if 0
    HRESULT CopyProperties(IMediaSample *pSource, IMediaSample *pDest) const;
#endif

	HRESULT getOutputSampleBuffer(IMediaSample *pSource,IMediaSample **ppOutput);
    HRESULT deinterlace(IMediaSample *pIn);
	void callDeinterlaceMethod(DEINTERLACE_INFO *pInfo,BOOL fHistoryValid) const;


    CCritSec	m_DeinterlaceLock;  // Private play critical section
    int         m_DeinterlaceType;  // Which type of deinterlacing shall we do

	CComPtr<IMediaSample> m_pInputHistory[MAX_FRAMES_IN_HISTORY];
};

// ==================================================
// Implements the input pin
// ==================================================

class CDeinterlaceInputPin : public CTransformInputPin
{

protected:
    CDeinterlace *m_pDeinterlaceFilter;    // our filter

public:

    CDeinterlaceInputPin(
        TCHAR               *pObjectName,
        CDeinterlace        *pFilter,
        HRESULT             *phr,
        LPCWSTR              pName);

    // --- IMemInputPin -----

    // Return our upstream allocator
    STDMETHODIMP GetAllocator(IMemAllocator ** ppAllocator);

    // get told which allocator the upstream output pin is actually
    // going to use.
    STDMETHODIMP NotifyAllocator(IMemAllocator * pAllocator,
                                 BOOL bReadOnly);

    // Allow the filter to see what allocator we have
    // N.B. This does NOT AddRef
    IMemAllocator * PeekAllocator()
        {  return m_pAllocator; }

    // Pass this on downstream if it ever gets called.
    STDMETHODIMP
    CDeinterlaceInputPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *pProps);

};  // CDeinterlaceInputPin

/////////////////////////////////////////////////////////////////////////////
// Macros used to read and write to stream
/////////////////////////////////////////////////////////////////////////////

#define WRITEOUT(var)  hr = pStream->Write(&var, sizeof(var), NULL); \
		       if (FAILED(hr)) return hr;
#define READIN(var)    hr = pStream->Read(&var, sizeof(var), NULL); \
		       if (FAILED(hr)) return hr;

#endif	// _DEINTERLACE_H
