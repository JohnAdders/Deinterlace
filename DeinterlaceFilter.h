/////////////////////////////////////////////////////////////////////////////
// $Id: DeinterlaceFilter.h,v 1.2 2001-11-14 13:32:05 adcockj Exp $
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2001 John Adcock.  All rights reserved.
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
//  In addition, as a special exception, John Adcock
//  gives permission to link the code of this program with
//  DirectShow Filter graph and distribute linked combinations including
//  the two.  You must obey the GNU General Public License in all
//  respects for all of the code used other than that which mapipulated 
//  the filter graph. If you modify
//  this file, you may extend this exception to your version of the
//  file, but you are not obligated to do so.  If you do not wish to
//  do so, delete this exception statement from your version.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __DEINTERLACEFILTER_H__
#define __DEINTERLACEFILTER_H__

#include "Deinterlace.h"
#include "DI.h"

#define MAX_FRAMES_IN_HISTORY	2
#define MAX_INPUT_LINES_PER_FIELD   2048

class DGenericDSPlugin;

class CDeinterlaceFilter : public CTransformFilter,
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
    STDMETHOD(get_DeinterlaceType)(long *IPEffect);
    STDMETHOD(put_DeinterlaceType)(long IPEffect);

    // ISpecifyPropertyPages interface
    STDMETHODIMP GetPages(CAUUID *pPages);


private:
    // Constructor
    CDeinterlaceFilter(TCHAR *tszName, LPUNKNOWN punk, HRESULT *phr);
    BOOL CanPerformDeinterlace(const CMediaType *pMediaType) const;
	HRESULT getOutputSampleBuffer(IMediaSample *pSource,IMediaSample **ppOutput);
    HRESULT deinterlace(IMediaSample *pIn);
	void callDeinterlaceMethod(DEINTERLACE_INFO *pInfo) const;


    CCritSec	m_DeinterlaceLock;  // Private play critical section
    int         m_DeinterlaceType;  // Which type of deinterlacing shall we do
	CComPtr<IMediaSample> m_pInputHistory[MAX_FRAMES_IN_HISTORY];
	DGenericDSPlugin *m_pDeinterlacePlugin;
    DEINTERLACE_INFO m_Info;
    short* m_OddLines[2][MAX_INPUT_LINES_PER_FIELD];
	short* m_EvenLines[2][MAX_INPUT_LINES_PER_FIELD];
    int m_History;
    REFERENCE_TIME m_LastStop;
};

#endif
