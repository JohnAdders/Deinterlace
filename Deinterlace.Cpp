/////////////////////////////////////////////////////////////////////////////
// $Id: Deinterlace.Cpp,v 1.12 2001-11-14 17:06:21 adcockj Exp $
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
// Revision 1.11  2001/11/14 13:32:05  adcockj
// Moved COM definitions into IDL
//
// Revision 1.10  2001/11/13 13:51:43  adcockj
// Tidy up code and made to mostly conform to coding standards
// Changed history behaviour
// Made to use DEINTERLACE_INFO throughout
//
// Revision 1.9  2001/11/10 10:35:01  pgubanov
// Correct handling of interlace flags, GreedyH now works fine.
//
// Revision 1.8  2001/11/09 15:34:27  pgubanov
// Try to work with DScaler plugins. Some code adopted from DIDMO, but less general anyway. For some reason, plugin crashes...
//
// Revision 1.7  2001/11/09 11:42:39  pgubanov
// Output 2 frames for each inout frame. Doesn't care about input media type too much - just assume all input samples are frames. Never connect to anything except YUY2.
//
// Revision 1.6  2001/11/01 11:04:19  adcockj
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
// 20 Sep 2001   Michael Eskin         Changed to support use with video from bt878
//                                     Driver 
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <initguid.h>
#include "Deinterlace.h"
#include "Deinterlace_i.c"
#include "DeinterlaceFilter.h"
#include "DeinterlaceProperties.h"
#include "DeinterlaceAbout.h"

/////////////////////////////////////////////////////////////////////////////
// Setup information
// This is used when registering the filter
/////////////////////////////////////////////////////////////////////////////

const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
    &MEDIATYPE_Video,       // Major type
    &MEDIASUBTYPE_NULL      // Minor type
};

const AMOVIESETUP_PIN sudpPins[] =
{
    { 
        L"Input",             // Pins string name
        FALSE,                // Is it rendered
        FALSE,                // Is it an output
        FALSE,                // Are we allowed none
        FALSE,                // And allowed many
        &CLSID_NULL,          // Connects to filter
        NULL,                 // Connects to pin
        1,                    // Number of types
        &sudPinTypes          // Pin information
    },
    { 
        L"Output",            // Pins string name
        FALSE,                // Is it rendered
        TRUE,                 // Is it an output
        FALSE,                // Are we allowed none
        FALSE,                // And allowed many
        &CLSID_NULL,          // Connects to filter
        NULL,                 // Connects to pin
        1,                    // Number of types
        &sudPinTypes          // Pin information
    }
};

const AMOVIESETUP_FILTER sudDeinterlace =
{
    &CLSID_Deinterlace,     // Filter CLSID
    L"Deinterlace Filter",  // String name
    MERIT_DO_NOT_USE,       // Filter merit
    2,                      // Number of pins
    sudpPins                // Pin information
};


// List of class IDs and creator functions for the class factory. This
// provides the link between the OLE entry point in the DLL and an object
// being created. The class factory will call the static CreateInstance

CFactoryTemplate g_Templates[] = 
{
    { 
        L"Deinterlace",
        &CLSID_Deinterlace,
        CDeinterlaceFilter::CreateInstance,
        NULL,
        &sudDeinterlace,
    },
    { 
        L"Deinterlace Settings",
        &CLSID_DeinterlacePropertyPage,
        CDeinterlaceProperties::CreateInstance,
        NULL,
        NULL,
    },
    { 
        L"About Deinterlace",
        &CLSID_DeinterlaceAboutPage,
        CDeinterlaceAbout::CreateInstance,
        NULL,
        NULL,
    }
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer
// Pass off to base classes
// We don;t need anything extra here
/////////////////////////////////////////////////////////////////////////////
STDAPI DllRegisterServer()
{
    // Create entry in HKEY_CLASSES_ROOT\Filter
    OLECHAR szCLSID[CHARS_IN_GUID];
    TCHAR achTemp[MAX_PATH];
    HKEY hKey;

    HRESULT hr = AMovieDllRegisterServer2( TRUE );
    if (SUCCEEDED(hr))
    {
        // Incompatible way to register:
        // ActiveMovie uses Filter\* tree to find its filters
        StringFromGUID2(*g_Templates[0].m_ClsID, szCLSID, CHARS_IN_GUID);
        wsprintf(achTemp, TEXT("Filter\\%ls"), szCLSID);
        // create key
        RegCreateKey(HKEY_CLASSES_ROOT, (LPCTSTR)achTemp, &hKey);
        RegCloseKey(hKey);
    }

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer
// Pass off to base classes
// We don;t need anything extra here
/////////////////////////////////////////////////////////////////////////////
STDAPI DllUnregisterServer()
{
    HRESULT hr = AMovieDllRegisterServer2( FALSE );
    if (SUCCEEDED(hr))
    {
        // Incompatible way to unregister:
        // Delete entry in HKEY_CLASSES_ROOT\Filter
        OLECHAR szCLSID[CHARS_IN_GUID];
        TCHAR achTemp[MAX_PATH];

        StringFromGUID2(*g_Templates[0].m_ClsID, szCLSID, CHARS_IN_GUID);
        wsprintf(achTemp, TEXT("Filter\\%ls"), szCLSID);
        // remove key
        RegDeleteKey(HKEY_CLASSES_ROOT, (LPCTSTR)achTemp);
    }

    return hr;
}
