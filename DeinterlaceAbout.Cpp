/////////////////////////////////////////////////////////////////////////////
// $Id: DeinterlaceAbout.Cpp,v 1.1 2001-11-14 17:06:21 adcockj Exp $
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000 John Adcock.  All rights reserved.
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
// Revision 1.5  2001/11/14 16:42:18  adcockj
// Added support for any plugin
//
// Revision 1.4  2001/11/14 13:42:22  adcockj
// Fixed compile errors
//
// Revision 1.3  2001/11/13 13:51:43  adcockj
// Tidy up code and made to mostly conform to coding standards
// Changed history behaviour
// Made to use DEINTERLACE_INFO throughout
//
// Revision 1.2  2001/11/01 11:04:19  adcockj
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
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "DeinterlaceAbout.h"

/////////////////////////////////////////////////////////////////////////////
// CreateInstance
// Used by the ActiveMovie base classes to create instances
/////////////////////////////////////////////////////////////////////////////
CUnknown *CDeinterlaceAbout::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    CUnknown *punk = new CDeinterlaceAbout(lpunk, phr);
    if (punk == NULL)
    {
        *phr = E_OUTOFMEMORY;
    }
    return punk;
}

/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
CDeinterlaceAbout::CDeinterlaceAbout(LPUNKNOWN pUnk, HRESULT *phr) :
    CBasePropertyPage(NAME("Special Effects Property Page"), pUnk, IDD_DEINTERLACEABOUT, IDS_ABOUT)
{
    ASSERT(phr);
}

/////////////////////////////////////////////////////////////////////////////
// OnReceiveMessage
// Handles the messages for our property window
/////////////////////////////////////////////////////////////////////////////
BOOL CDeinterlaceAbout::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return CBasePropertyPage::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

/////////////////////////////////////////////////////////////////////////////
// OnConnect
// Called when we connect to a transform filter
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlaceAbout::OnConnect(IUnknown *pUnknown)
{
    return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// OnDisconnect
// Likewise called when we disconnect from a filter
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlaceAbout::OnDisconnect()
{
    return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// OnActivate
// We are being activated
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlaceAbout::OnActivate()
{
    return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// OnDeactivate
// We are being deactivated
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlaceAbout::OnDeactivate(void)
{
    return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// OnApplyChanges
// Apply any changes so far made
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlaceAbout::OnApplyChanges()
{
    return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// GetControlValues
// Update the type variables
/////////////////////////////////////////////////////////////////////////////
void CDeinterlaceAbout::GetControlValues()
{
}
