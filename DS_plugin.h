/////////////////////////////////////////////////////////////////////////////
// $Id: DS_plugin.h,v 1.2 2001-11-13 13:51:43 adcockj Exp $
/////////////////////////////////////////////////////////////////////////////
// A generic DScaler Deinterlace plugin wrapper class
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

#ifndef _DS_PLUGIN_H
#define _DS_PLUGIN_H

#include "DI.h"

//
// Define basic interface for generic DScaler plugin
//
class DGenericDSPlugin 
{
public:
	virtual ~DGenericDSPlugin() 
    {};
	virtual void startStreaming() = NULL;
	virtual void process(DEINTERLACE_INFO *Info) = NULL;
	virtual void stopStreaming() = NULL;
	virtual void showSettingsDialog(HWND hwnd) = NULL;
};

//
// GreedyH Implementation
//
class DDeinterlaceGreedyH : public DGenericDSPlugin, public CCritSec 
{
public:
	DDeinterlaceGreedyH();
	~DDeinterlaceGreedyH();

	void startStreaming();
	void process(DEINTERLACE_INFO *Info);
	void stopStreaming();
	void showSettingsDialog(HWND hwnd);
private:
	HMODULE	m_hModule;
	DEINTERLACE_METHOD* m_pMethod;
	BOOL m_fStreaming;

};

#endif _DS_PLUGIN_H
