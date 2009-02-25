/////////////////////////////////////////////////////////////////////////////
// $Id$
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

#ifndef __DEINTERLACEINPUTPIN_H__
#define __DEINTERLACEINPUTPIN_H__

#include "DeinterlaceFilter.h"

class CDeinterlaceInputPin : public CTransformInputPin
{
public:
    CDeinterlaceInputPin(
                            TCHAR* pObjectName,
                            CDeinterlaceFilter* pFilter,
                            HRESULT* phr,
                            LPCWSTR pName
                        );

    // IMemInputPin
    STDMETHOD(GetAllocator)(IMemAllocator ** ppAllocator);
    STDMETHOD(NotifyAllocator)(IMemAllocator * pAllocator, BOOL bReadOnly);
    STDMETHOD(GetAllocatorRequirements)(ALLOCATOR_PROPERTIES *pProps);

    // Allow the filter to see what allocator we have
    // N.B. This does NOT AddRef
    IMemAllocator * PeekAllocator()
    {  
        return m_pAllocator; 
    }

    // Pass this on downstream if it ever gets called.

protected:
    CDeinterlaceFilter* m_pDeinterlaceFilter;
};

#endif
