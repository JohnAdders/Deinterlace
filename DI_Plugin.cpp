/////////////////////////////////////////////////////////////////////////////
// $Id: DI_Plugin.cpp,v 1.1 2001-11-14 16:42:18 adcockj Exp $
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2001 Peter Gubanov.  All rights reserved.
/////////////////////////////////////////////////////////////////////////////
//
//  This file is subject to the terms of the GNU General Public License as
//  published by the Free Software Foundation.  A copy of this license is
//  included with this software distribution in the file COPYING.  If you
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
///////////////////////////////////////////////////////////////////////////////
// CVS Log
//
// $Log: not supported by cvs2svn $
// Revision 1.3  2001/11/13 13:51:43  adcockj
// Tidy up code and made to mostly conform to coding standards
// Changed history behaviour
// Made to use DEINTERLACE_INFO throughout
//
///////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "cpu.h"
#include "DI_Plugin.h"
#include "memcpy.h"

CDeinterlacePlugin::CDeinterlacePlugin(LPCSTR PluginName) :
    m_pMethod(NULL),
    m_hModule(NULL),
    m_fStreaming(FALSE)
{
    HMODULE hModule = ::LoadLibrary(PluginName);
    if(hModule == NULL) 
    {
        return;
    }

    GETDEINTERLACEPLUGININFO *pfnGetDeinterlacePluginInfo =
        (GETDEINTERLACEPLUGININFO*)GetProcAddress(hModule, "GetDeinterlacePluginInfo");

    if (pfnGetDeinterlacePluginInfo == NULL) 
    {
        return;
    }

    DEINTERLACE_METHOD *pMethod = pfnGetDeinterlacePluginInfo(get_feature_flags());
    if(pMethod != NULL) 
    {
        //
        // check that the plugin has the right api version
        //
        if(pMethod->SizeOfStructure!=sizeof(DEINTERLACE_METHOD) ||
            pMethod->DeinterlaceStructureVersion != DEINTERLACE_CURRENT_VERSION) 
        {

            ::FreeLibrary(hModule);
            return;
        }

        if(pMethod->bNeedFieldDiff==TRUE ||
            pMethod->bNeedCombFactor==TRUE ||
            pMethod->bIsHalfHeight==TRUE)
        {
            ::FreeLibrary(hModule);
            return;
        }
        
        m_pMethod = pMethod;
        m_hModule = hModule;
    }
}

CDeinterlacePlugin::~CDeinterlacePlugin()
{
    if (m_hModule)
    {
        ::FreeLibrary(m_hModule);
    }
    m_hModule = NULL;
    m_pMethod = NULL;
}

void CDeinterlacePlugin::StartStreaming()
{
    CAutoLock lck(this);

    if(m_pMethod && !m_fStreaming && (m_pMethod->pfnPluginStart != NULL)) 
    {
        // FIXME: last param, status callbacks
        m_pMethod->pfnPluginStart(0, NULL, NULL);
    }

    m_fStreaming = TRUE;
}

void CDeinterlacePlugin::StopStreaming() 
{
    CAutoLock lck(this);

    // call plugin exit function if present
    if(m_pMethod && m_fStreaming && (m_pMethod->pfnPluginExit != NULL)) 
    {
        m_pMethod->pfnPluginExit();
    }

    m_fStreaming = FALSE;
}

void CDeinterlacePlugin::Process(DEINTERLACE_INFO *pInfo) 
{
    // can't use CAutolock as we're
    // going to use SEH
    Lock();

    if(m_fStreaming)
    {
        if(m_pMethod) 
        {
            __try 
            {
                m_pMethod->pfnAlgorithm(pInfo);
            }
            __except(EXCEPTION_EXECUTE_HANDLER) 
            {
                DbgLog((LOG_ERROR, 1, TEXT("Exception in deinterlacing module")));
            }
        }
        else
        {
            Bob(pInfo);
        }
    }
    Unlock();
}

void CDeinterlacePlugin::ShowSettingsDialog(HWND hwnd) 
{
    if (m_pMethod && (m_pMethod->pfnPluginShowUI != NULL))
    {
        m_pMethod->pfnPluginShowUI(hwnd);
    }
}
