/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2001 Peter Gubanov.  All rights reserved.
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
// DS_plugin.h
/////////////////////////////////////////////////////////////////////////////
// A generic DScaler Deinterlace plugin wrapper class

#ifndef _DS_PLUGIN_H
#define _DS_PLUGIN_H

#include "../DScaler/Api/DS_Deinterlace.h"
#include "../DScaler/Api/DS_Filter.h"
#include "DI.h"

//
// Define basic interface for generic DScaler plugin
//
class DGenericDSPlugin {
public:
	virtual ~DGenericDSPlugin() {};
	virtual void startStreaming() = NULL;
	virtual void process(MY_DEINTERLACE_INFO *Info) = NULL;
	virtual void stopStreaming() = NULL;
	virtual void showSettingsDialog(HWND hwnd) = NULL;
};

//
// GreedyH Implementation
//
class DDeinterlaceGreedyH : public DGenericDSPlugin, public CCritSec {
	HMODULE	m_hModule;
	DEINTERLACE_METHOD	*m_pMethod;

	BOOL m_fStreaming;

public:
	DDeinterlaceGreedyH();
	~DDeinterlaceGreedyH();

	void startStreaming();
	void process(MY_DEINTERLACE_INFO *Info);
	void stopStreaming();
	void showSettingsDialog(HWND hwnd);
};

#endif _DS_PLUGIN_H
