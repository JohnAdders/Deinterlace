/////////////////////////////////////////////////////////////////////////////
// $Id: DeinterlaceInputPin.cpp,v 1.1 2001-11-13 13:51:43 adcockj Exp $
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2001 John Adcock.  All rights reserved.
/////////////////////////////////////////////////////////////////////////////
//
//  This file is subject to the terms of the GNU General Public License as
//  published by the Free Software Foundation.  A copy of this license is
//  included with this software distribution in the file COPYING.txt.  If you
//  do not have a copy, you may obtain a copy by writing to the Free
//  Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details
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
// CVS Log
//
// $Log: not supported by cvs2svn $
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "DeinterlaceInputPin.h"

CDeinterlaceInputPin::CDeinterlaceInputPin( 
                                            TCHAR* pObjectName,
                                            CDeinterlaceFilter* pFilter,
                                            HRESULT* phr,
                                            LPCWSTR pName
                                            ) :
    CTransformInputPin(pObjectName, pFilter, phr, pName)
{
    DbgLog((LOG_TRACE, 2, TEXT("CDeinterlaceInputPin::CDeinterlaceInputPin")));

    m_pDeinterlaceFilter = pFilter;
}

////////////////////////////////////////////////////////////////////////////
// GetAllocator
// 
// If the downstream filter has one then offer that (even if our own output
// pin is not using it yet.  If the upstream filter chooses it then we will
// tell our output pin to ReceiveAllocator).
// Else if our output pin is using an allocator then offer that.
//     ( This could mean offering the upstream filter his own allocator,
//       it could mean offerring our own
//     ) or it could mean offering the one from downstream
// Else fail to offer any allocator at all.
/////////////////////////////////////////////////////////////////////////////

STDMETHODIMP CDeinterlaceInputPin::GetAllocator(IMemAllocator ** ppAllocator)
{
    if (!IsConnected())
    {
        return VFW_E_NO_ALLOCATOR;
    }

    HRESULT hr = CTransformInputPin::GetAllocator(ppAllocator);

    if (SUCCEEDED(hr)) 
    {
        ALLOCATOR_PROPERTIES Props, Actual;
        (*ppAllocator)->GetProperties(&Props);
        if (Props.cBuffers < MAX_FRAMES_IN_HISTORY+1)
        {
            Props.cBuffers = MAX_FRAMES_IN_HISTORY+1;
        }
        if (Props.cbAlign < 8)
        {
            Props.cbAlign = 8;
        }
        if (Props.cbBuffer < (long)CurrentMediaType().GetSampleSize())
        {
            Props.cbBuffer = (long)CurrentMediaType().GetSampleSize();
        }

        hr = (*ppAllocator)->SetProperties(&Props,&Actual);
        if (SUCCEEDED(hr) && Actual.cBuffers < MAX_FRAMES_IN_HISTORY+1)
        {
            hr = E_FAIL;
        }
    }

    return hr;

}

/////////////////////////////////////////////////////////////////////////////
// NotifyAllocator
// 
// Get told which allocator the upstream output pin is actually going to use 
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDeinterlaceInputPin::NotifyAllocator(IMemAllocator * pAllocator, BOOL bReadOnly)
{
    HRESULT hr = CTransformInputPin::NotifyAllocator(pAllocator, bReadOnly);
    if (SUCCEEDED(hr)) 
    {
        ALLOCATOR_PROPERTIES Props;
        m_pAllocator->GetProperties(&Props);
        if (Props.cBuffers < MAX_FRAMES_IN_HISTORY+1)
        {
            hr = E_FAIL;
        }
    }

    return hr;

}

/////////////////////////////////////////////////////////////////////////////
// GetAllocatorRequirements
/////////////////////////////////////////////////////////////////////////////
STDMETHODIMP CDeinterlaceInputPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES *pProps)
{
    DbgLog((LOG_TRACE, 2, TEXT("CDeinterlaceInputPin::GetAllocatorRequirements")));

    pProps->cbAlign = 8;
    pProps->cbBuffer = 0;
    pProps->cbPrefix = 0;
    pProps->cBuffers = MAX_FRAMES_IN_HISTORY+4;  // JBC 9/20/01 we drop frames if to few buffers
    
    return NOERROR;
}
