/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2001 Peter Gubanov.  All rights reserved.
/////////////////////////////////////////////////////////////////////////////
//
//	This file is subject to the terms of the GNU General Public License as
//	published by the Free Software Foundation.  A copy of this license is
//	included with this software distribution in the file COPYING.  If you
//	do not have a copy, you may obtain a copy by writing to the Free
//	Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
//	This software is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details
///////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include "DS_Plugin.h"

DDeinterlaceGreedyH::DDeinterlaceGreedyH() {

	USES_CONVERSION;
	GETDEINTERLACEPLUGININFO* pfnGetDeinterlacePluginInfo;
	DEINTERLACE_METHOD* pMethod;
	HMODULE hPlugInMod;
	HRESULT hr;

	hPlugInMod = LoadLibrary(szFileName);
	if(hPlugInMod==NULL) {
		char *str=new char[strlen(szFileName)+30];
		wsprintf(str,"Failed to load plugin %s",szFileName);
		CreateErrorInfoObj(str);
		delete [] str;
		return E_FAIL;
	}

	pfnGetDeinterlacePluginInfo=(GETDEINTERLACEPLUGININFO*)GetProcAddress(hPlugInMod, "GetDeinterlacePluginInfo");
	if(pfnGetDeinterlacePluginInfo==NULL)
	{
		CreateErrorInfoObj("GetProcAddress() failed, Are you sure this is a valid plugin?");
		return E_FAIL;
	}

	pMethod=pfnGetDeinterlacePluginInfo(CpuFeatureFlags);
	if(pMethod!=NULL)
	{
		//check that the plugin has the right api version
		if(pMethod->SizeOfStructure!=sizeof(DEINTERLACE_METHOD) || pMethod->DeinterlaceStructureVersion!=DEINTERLACE_CURRENT_VERSION)
		{
			CreateErrorInfoObj("This version of the plugin is not supported");
			FreeLibrary(hPlugInMod);
			return E_FAIL;
		}

		if(pMethod->bNeedFieldDiff==TRUE ||
			pMethod->bNeedCombFactor==TRUE ||
			pMethod->bIsHalfHeight==TRUE)
		{
			CreateErrorInfoObj("Plugins that need FieldDiff, CombFactor or IsHalfHeight is currently not supported");
			FreeLibrary(hPlugInMod);
			return E_FAIL;
		}
		
		//is a plugin already loaded?
		if(m_DIPlugin!=NULL)
		{
			//remove it
			hr=UnloadPlugin();
			if(FAILED(hr))
			{
				FreeLibrary(hPlugInMod);
				return hr;
			}
		}
		
		//lock dmo
		LockIt lck(this);
		
		m_DIPlugin=pMethod;
		m_DIPlugin->hModule=hPlugInMod;

		if(m_DIPlugin->pfnPluginStart!=NULL)
		{
			//FIXME: last param, status callbacks
			m_DIPlugin->pfnPluginStart(0, NULL,NULL);
		}
		
		return S_OK;
	}
}