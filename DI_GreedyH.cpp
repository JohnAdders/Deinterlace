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
#include <streams.h>
#include "cpu.h"
#include "DS_plugin.h"
#include "memcpy.h"

DDeinterlaceGreedyH::DDeinterlaceGreedyH() :
	m_pMethod(NULL),
	m_hModule(NULL),
	m_fStreaming(FALSE)
{
	HMODULE hModule = ::LoadLibrary("DI_GreedyH.dll");
	if(hModule == NULL) {
		return;
	}

	GETDEINTERLACEPLUGININFO *pfnGetDeinterlacePluginInfo =
		(GETDEINTERLACEPLUGININFO*)GetProcAddress(hModule, "GetDeinterlacePluginInfo");

	if (pfnGetDeinterlacePluginInfo == NULL) {
		return;
	}

	DEINTERLACE_METHOD *pMethod = pfnGetDeinterlacePluginInfo(get_feature_flags());
	if(pMethod != NULL) {
		//
		// check that the plugin has the right api version
		//
		if(pMethod->SizeOfStructure!=sizeof(DEINTERLACE_METHOD) ||
			pMethod->DeinterlaceStructureVersion!=DEINTERLACE_CURRENT_VERSION) {

			FreeLibrary(hModule);
			return;
		}

		if(pMethod->bNeedFieldDiff==TRUE ||
			pMethod->bNeedCombFactor==TRUE ||
			pMethod->bIsHalfHeight==TRUE)
		{
			FreeLibrary(hModule);
			return;
		}
		
		m_pMethod = pMethod;
		m_hModule = hModule;
	}
}

DDeinterlaceGreedyH::~DDeinterlaceGreedyH()
{
	if (m_hModule)
		FreeLibrary(m_hModule);
	m_hModule = NULL;
	m_pMethod = NULL;
}

void
DDeinterlaceGreedyH::startStreaming() {

	CAutoLock lck(this);

	if(m_pMethod && !m_fStreaming && (m_pMethod->pfnPluginStart != NULL)) {
		// FIXME: last param, status callbacks
		m_pMethod->pfnPluginStart(0, NULL,NULL);
	}

	m_fStreaming = TRUE;
}

void
DDeinterlaceGreedyH::stopStreaming() {

	CAutoLock lck(this);

	// call plugin exit function if present
	if(m_pMethod && m_fStreaming && (m_pMethod->pfnPluginExit != NULL)) {
		m_pMethod->pfnPluginExit();
	}

	m_fStreaming = FALSE;
}

void
DDeinterlaceGreedyH::process(MY_DEINTERLACE_INFO *pInfo) {

	CAutoLock lck(this);

	DEINTERLACE_INFO DIInfo;

	for (int i=0;i<MAX_FIELD_HISTORY;i++) {
		DIInfo.OddLines[i] = NULL;
		DIInfo.EvenLines[i] = NULL;
	}

	DIInfo.OddLines[0] = pInfo->OddLines[0];
	DIInfo.EvenLines[0] = pInfo->EvenLines[0];
	DIInfo.OddLines[1] = pInfo->OddLines[1];
	DIInfo.EvenLines[1] = pInfo->EvenLines[1];

	DIInfo.Overlay = pInfo->Overlay;
	DIInfo.SourceRect.top = DIInfo.SourceRect.left = 0;
	DIInfo.SourceRect.bottom = 0;
	DIInfo.SourceRect.right = 0;
	DIInfo.IsOdd = pInfo->IsOdd;
	DIInfo.CurrentFrame = 0;
	DIInfo.OverlayPitch = pInfo->OverlayPitch;
	DIInfo.LineLength = pInfo->FrameWidth*2;		// Assume 16 bpp, YUY2
	DIInfo.FrameWidth = pInfo->FrameWidth;
	DIInfo.FrameHeight = pInfo->FrameHeight;
	DIInfo.FieldHeight = 8; //pInfo->FieldHeight/2;

	DIInfo.FieldDiff = 0;
	DIInfo.CombFactor = 0;

    DIInfo.CpuFeatureFlags = get_feature_flags();
	if(DIInfo.CpuFeatureFlags & FEATURE_SSE) {
		DIInfo.pMemcpy = memcpySSE;
	}
	else {
		DIInfo.pMemcpy = memcpyMMX;
	}

    DIInfo.bRunningLate = FALSE;
    DIInfo.bMissedFrame = FALSE;
    DIInfo.bDoAccurateFlips = FALSE;
	DIInfo.DestRect.top = DIInfo.DestRect.left = 0;
	DIInfo.DestRect.bottom = 0;
	DIInfo.DestRect.right = 0;

	if (m_pMethod && m_fStreaming) {
		m_pMethod->pfnAlgorithm(&DIInfo);
	}

}

void
DDeinterlaceGreedyH::showSettingsDialog(HWND hwnd) {

	if (m_pMethod && (m_pMethod->pfnPluginShowUI != NULL)) {
		m_pMethod->pfnPluginShowUI(hwnd);
	}
}
