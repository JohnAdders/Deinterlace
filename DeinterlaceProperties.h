///////////////////////////////////////////////////////////////////////////////
// $Id: DeinterlaceProperties.h,v 1.6 2001-12-11 17:31:58 adcockj Exp $
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

#ifndef __DEINTERLACEPROPERTIES_H__
#define __DEINTERLACEPROPERTIES_H__

#include "Deinterlace.h"

class CDeinterlaceProperties : public CBasePropertyPage
{
public:
    static CUnknown* WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT* phr);

private:
    CDeinterlaceProperties(LPUNKNOWN lpunk, HRESULT* phr);
    BOOL OnReceiveMessage(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
    HRESULT OnConnect(IUnknown* pUnknown);
    HRESULT OnDisconnect();
    HRESULT OnActivate();
    HRESULT OnDeactivate();
    HRESULT OnApplyChanges();
    void    GetControlValues();


private:
    void FillComboBox();

private:
    BOOL m_bIsInitialized;      // Used to ignore startup messages
    IDeinterlace2* m_pIDeinterlace;  // The custom interface on the filter

    int m_DeinterlaceType;
    VARIANT_BOOL m_OddFieldFirst;
    VARIANT_BOOL m_RateDouble;
    BSTR m_PluginName;
};


#endif