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

Introduction
------------

This filter provides a limited amout of the deinterlacing functionality of DScaler 
to applications using the DirectShow framework.  For more details about DScaler see our
homepage at http://www.dscaler.org/

Instructions
------------

To build the project you need the following
MS Visual C++ 6.0 with the processor pack add on

A copy of the platform SDK which includes the DirectShow SDK
 (I think that's April 2000 onwards but not recent versions that excude using samples in GPL code)

Add the platform sdk include and lib directories to the paths
  Check in Tools/Option.../Directories
  You need to add
  (Plat SDK Root)\include to Include Files 
  (Plat SDK Root)\lib to Library Files 
  (Plat SDK Root)\Classes\Base to Include Files
  (Plat SDK Root)\Classes\Base to Source Files

The project should then Build OK

See website for instruction on using filter
http://www.dscaler.org/

Files
-----

DeinterlaceProperties.cpp       A property page to control the video effects
DeinterlaceProperties.h         Class definition for the property page object
Deinterlace.rc					Dialog box template for the property page
Deinterlace.cpp					Main filter code that does the special effects
Deinterlace.def					What APIs we import and export from this DLL
Deinterlace.h					Class definition for the special effects filter
DeinterlaceGuids.h				Header file containing the filter CLSIDs
IDeinterlace.h					Defines the special effects custom interface
resource.h						Microsoft Visual C++ generated resource file


Base classes used

CTransformFilter     A transform filter with one input and output pin
CPersistStream       Handles the grunge of supporting IPersistStream


