/////////////////////////////////////////////////////////////////////////////
// $Id$
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

#include "stdafx.h"
#include "resource.h"
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

    HRESULT hr = pUnknown->QueryInterface(IID_IDeinterlace2, (void **) &m_pIDeinterlace);
    if (FAILED(hr))
    {
        return E_NOINTERFACE;
    }

    ASSERT(m_pIDeinterlace);

    // Get the initial properties
    m_pIDeinterlace->get_DeinterlaceType(&m_DeinterlaceType);
    m_pIDeinterlace->get_IsOddFieldFirst(&m_OddFieldFirst);
    m_pIDeinterlace->get_DScalerPluginName(&m_PluginName);
    m_pIDeinterlace->get_RefreshRateDouble(&m_RateDouble);
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
    CheckRadioButton(m_Dlg, IDC_WEAVE, IDC_TYPE4, m_DeinterlaceType);
    if(m_OddFieldFirst)
    {
        CheckDlgButton(m_Dlg, IDC_ODDFIELDFIRST, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_Dlg, IDC_ODDFIELDFIRST, BST_UNCHECKED);
    }
    if(m_RateDouble)
    {
        CheckDlgButton(m_Dlg, IDC_RATEDOUBLE, BST_CHECKED);
    }
    else
    {
        CheckDlgButton(m_Dlg, IDC_RATEDOUBLE, BST_UNCHECKED);
    }
    FillComboBox();
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
    m_pIDeinterlace->put_IsOddFieldFirst(m_OddFieldFirst);
    m_pIDeinterlace->put_DScalerPluginName(m_PluginName);
    m_pIDeinterlace->put_RefreshRateDouble(m_RateDouble);
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
    for (int i = IDC_WEAVE; i <= IDC_TYPE4; i++)
    {
        if (IsDlgButtonChecked(m_Dlg, i))
        {
            m_DeinterlaceType = i;
            break;
        }
    }
    if(IsDlgButtonChecked(m_Dlg, IDC_ODDFIELDFIRST))
    {
        m_OddFieldFirst = VARIANT_TRUE;
    }
    else
    {
        m_OddFieldFirst = VARIANT_FALSE;
    }
    if(IsDlgButtonChecked(m_Dlg, IDC_RATEDOUBLE))
    {
        m_RateDouble = VARIANT_TRUE;
    }
    else
    {
        m_RateDouble = VARIANT_FALSE;
    }
    if(ComboBox_GetCount(GetDlgItem(m_Dlg, IDC_PLUGINS)) > 0 &&
        ComboBox_GetCurSel(GetDlgItem(m_Dlg, IDC_PLUGINS)) >= 0)
    {
        char Text[MAX_PATH] = "";
        ComboBox_GetText(GetDlgItem(m_Dlg, IDC_PLUGINS), Text, MAX_PATH);
        CComBSTR bText(Text);
        m_PluginName = bText;
    }
    else
    {
        m_PluginName = L"";
    }
}

void CDeinterlaceProperties::FillComboBox()
{
    char szDriverPath[MAX_PATH];
    char* pszName;
    WIN32_FIND_DATA FindFileData;
    HANDLE hFindFile;

    if (!GetModuleFileName(g_hInst, szDriverPath, sizeof(szDriverPath)))
    {
        return;
    }

    pszName = szDriverPath + strlen(szDriverPath);
    while (pszName >= szDriverPath && *pszName != '\\')
    {
        *pszName-- = 0;
    }

    strcat(szDriverPath, "\\DI_*.dll");
    hFindFile = FindFirstFile(szDriverPath, &FindFileData);

    if (hFindFile != INVALID_HANDLE_VALUE)
    {
        BOOL RetVal = TRUE;
        while(RetVal != 0)
        {
            int i = ComboBox_AddString(GetDlgItem(m_Dlg, IDC_PLUGINS), FindFileData.cFileName);
            CComBSTR Plugin(FindFileData.cFileName);
            if(wcscmp(m_PluginName, Plugin) == 0)
            {
                ComboBox_SetCurSel(GetDlgItem(m_Dlg, IDC_PLUGINS), i);
            }
            RetVal = FindNextFile(hFindFile, &FindFileData);
        }
    }


}
