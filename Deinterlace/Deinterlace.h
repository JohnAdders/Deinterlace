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

    // Overrriden from CTransformFilter base class
    HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut);
    HRESULT CheckInputType(const CMediaType *mtIn);
    HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut);
    HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pProperties);
    HRESULT GetMediaType(int iPosition, CMediaType *pMediaType);
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
    HRESULT CopyProperties(IMediaSample *pSource, IMediaSample *pDest) const;

    CCritSec	m_DeinterlaceLock;  // Private play critical section
    int         m_DeinterlaceType;  // Which type of deinterlacing shall we do
};


/////////////////////////////////////////////////////////////////////////////
// Macros used to read and write to stream
/////////////////////////////////////////////////////////////////////////////

#define WRITEOUT(var)  hr = pStream->Write(&var, sizeof(var), NULL); \
		       if (FAILED(hr)) return hr;
#define READIN(var)    hr = pStream->Read(&var, sizeof(var), NULL); \
		       if (FAILED(hr)) return hr;
