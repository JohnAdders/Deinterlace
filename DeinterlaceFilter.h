/////////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2001 John Adcock.  All rights reserved.
/////////////////////////////////////////////////////////////////////////////
//
//    This file is subject to the terms of the GNU General Public License as
//    published by the Free Software Foundation.  A copy of this license is
//    included with this software distribution in the file COPYING.txt.  If you
//    do not have a copy, you may obtain a copy by writing to the Free
//    Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
//    This software is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details
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

#define MAX_FRAMES_IN_HISTORY    2
#define MAX_INPUT_LINES_PER_FIELD   2048

class CDeinterlacePlugin;

class CDeinterlaceFilter : public CTransformFilter,
         public IDeinterlace2,
         public ISpecifyPropertyPages,
         public CPersistStream
{

public:

    DECLARE_IUNKNOWN;
    static CUnknown*  WINAPI CreateInstance(LPUNKNOWN punk, HRESULT* phr);

    // Reveals IDeinterlace, ISpecifyPropertyPages, IPersistStream
    STDMETHOD(NonDelegatingQueryInterface)(REFIID riid, void**  ppv);

    // CPersistStream stuff
    HRESULT WriteToStream(IStream* pStream);
    HRESULT ReadFromStream(IStream* pStream);
    STDMETHOD(GetClassID)(CLSID* pClsid);

    CBasePin* GetPin(int n);

    // Overrriden from CTransformFilter base class
    HRESULT Receive(IMediaSample* pSample);
    HRESULT CheckInputType(const CMediaType* mtIn);
    HRESULT CheckTransform(const CMediaType* mtIn, const CMediaType* mtOut);
    HRESULT DecideBufferSize(IMemAllocator* pAlloc, ALLOCATOR_PROPERTIES* pProperties);
    HRESULT GetMediaType(int iPosition, CMediaType* pMediaType);
    HRESULT StartStreaming();
    HRESULT StopStreaming();

    // These implement the IDeinerlace interface
    STDMETHOD(get_DeinterlaceType)(int* IPEffect);
    STDMETHOD(put_DeinterlaceType)(int IPEffect);

    // These implement the IDeinerlace2 interface
    STDMETHOD(get_IsOddFieldFirst)(VARIANT_BOOL* OddFirst);
    STDMETHOD(put_IsOddFieldFirst)(VARIANT_BOOL OddFirst);
    STDMETHOD(get_DScalerPluginName)(BSTR* OddFirst);
    STDMETHOD(put_DScalerPluginName)(BSTR OddFirst);
    STDMETHOD(get_RefreshRateDouble)(VARIANT_BOOL* RateDouble);
    STDMETHOD(put_RefreshRateDouble)(VARIANT_BOOL RateDouble);

    // ISpecifyPropertyPages interface
    STDMETHOD(GetPages)(CAUUID* pPages);

    void SetHistoryAllowed(int HistoryAllowed);

private:
    // Constructor
    CDeinterlaceFilter(TCHAR* tszName, LPUNKNOWN punk, HRESULT* phr);
    BOOL CanPerformDeinterlace(const CMediaType* pMediaType) const;
    HRESULT GetOutputSampleBuffer(IMediaSample* pSource,IMediaSample** ppOutput);
    HRESULT Deinterlace(IMediaSample* pIn);
    void CallDeinterlaceMethod(TDeinterlaceInfo* pInfo, int DeinterlaceType);

    void FixOverlayPitch();

    HRESULT ActivateDeinterlacePlugin();
    HRESULT InactivateDeinterlacePlugin();

    CCritSec    m_DeinterlaceLock;  // Private play critical section
    int         m_DeinterlaceType;  // Which type of deinterlacing shall we do
    CComPtr<IMediaSample> m_pInputHistory[MAX_FRAMES_IN_HISTORY];
    CDeinterlacePlugin* m_pDeinterlacePlugin;
    TDeinterlaceInfo m_Info;
    TPicture m_Pictures[4];
    int m_History;
    REFERENCE_TIME m_LastStop;
    LONGLONG m_MediaStart;
    LONGLONG m_MediaStop;
    BOOL m_bIsOddFieldFirst;
    CComBSTR m_PlugInName;
    BOOL m_RateDouble;
    int m_HistoryAllowed;
};

#endif
