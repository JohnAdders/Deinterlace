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
// DeinterlaceProperties.cpp
/////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <windowsx.h>
#include <streams.h>
#include <commctrl.h>
#include <olectl.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include "resource.h"
#include "DeinterlaceGuids.h"
#include "IDeinterlace.h"
#include "DeinterlaceProperties.h"
#include "Deinterlace.h"

/////////////////////////////////////////////////////////////////////////////
// CreateInstance
// Used by the ActiveMovie base classes to create instances
/////////////////////////////////////////////////////////////////////////////
CUnknown *CDeinterlaceProperties::CreateInstance(LPUNKNOWN lpunk, HRESULT *phr)
{
    CUnknown *punk = new CDeinterlaceProperties(lpunk, phr);
    if (punk == NULL)
	{
		*phr = E_OUTOFMEMORY;
    }
    return punk;
}

/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
CDeinterlaceProperties::CDeinterlaceProperties(LPUNKNOWN pUnk, HRESULT *phr) :
    CBasePropertyPage(NAME("Special Effects Property Page"), pUnk,IDD_DEINTERLACEPROP,IDS_TITLE),
    m_pIDeinterlace(NULL),
    m_bIsInitialized(FALSE)
{
    ASSERT(phr);
}

/////////////////////////////////////////////////////////////////////////////
// OnReceiveMessage
// Handles the messages for our property window
/////////////////////////////////////////////////////////////////////////////
BOOL CDeinterlaceProperties::OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_COMMAND:
        {
            if (m_bIsInitialized)
            {
                m_bDirty = TRUE;
                if (m_pPageSite)
                {
                    m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
                }
            }
            return (LRESULT) 1;
        }
    }
    return CBasePropertyPage::OnReceiveMessage(hwnd,uMsg,wParam,lParam);
}

/////////////////////////////////////////////////////////////////////////////
// OnConnect
// Called when we connect to a transform filter
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlaceProperties::OnConnect(IUnknown *pUnknown)
{
    ASSERT(m_pIDeinterlace == NULL);

    HRESULT hr = pUnknown->QueryInterface(IID_IDeinterlace, (void **) &m_pIDeinterlace);
    if (FAILED(hr))
	{
        return E_NOINTERFACE;
    }

    ASSERT(m_pIDeinterlace);

    // Get the initial image FX property
    m_pIDeinterlace->get_DeinterlaceType(&m_DeinterlaceType);
    m_bIsInitialized = FALSE ;
    return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// OnDisconnect
// Likewise called when we disconnect from a filter
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlaceProperties::OnDisconnect()
{
    // Release of Interface after setting the appropriate old effect value
    if (m_pIDeinterlace == NULL)
	{
        return E_UNEXPECTED;
    }

    m_pIDeinterlace->Release();
    m_pIDeinterlace = NULL;
    return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// OnActivate
// We are being activated
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlaceProperties::OnActivate()
{
	CheckRadioButton(m_Dlg, IDC_WEAVE, IDC_TYPE3, m_DeinterlaceType);
	m_bIsInitialized = TRUE;
	return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// OnDeactivate
// We are being deactivated
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlaceProperties::OnDeactivate(void)
{
    ASSERT(m_pIDeinterlace);
    m_bIsInitialized = FALSE;
    GetControlValues();
    return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// OnApplyChanges
// Apply any changes so far made
/////////////////////////////////////////////////////////////////////////////
HRESULT CDeinterlaceProperties::OnApplyChanges()
{
    GetControlValues();
    m_pIDeinterlace->put_DeinterlaceType(m_DeinterlaceType);
    return NOERROR;
}

/////////////////////////////////////////////////////////////////////////////
// GetControlValues
// Update the type variables
/////////////////////////////////////////////////////////////////////////////
void CDeinterlaceProperties::GetControlValues()
{
    ASSERT(m_pIDeinterlace);
    
	// Find which Deinterlace type we have selected
    for (int i = IDC_WEAVE; i <= IDC_TYPE3; i++)
	{
        if (IsDlgButtonChecked(m_Dlg, i))
		{
            m_DeinterlaceType = i;
            break;
        }
    }
}
