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
// IDeinterlace.h
/////////////////////////////////////////////////////////////////////////////
// A custom interface to allow outside applications control over settings

#ifndef __IEZ__
#define __IEZ__

#ifdef __cplusplus
extern "C" {
#endif

	// {463D645C-48F7-11d4-8464-0008C782A257}
	DEFINE_GUID(IID_IDeinterlace, 
	0x463d645c, 0x48f7, 0x11d4, 0x84, 0x64, 0x0, 0x8, 0xc7, 0x82, 0xa2, 0x57);

    DECLARE_INTERFACE_(IDeinterlace, IUnknown)
    {
        STDMETHOD(get_DeinterlaceType) (THIS_ int *effectNum) PURE;
        STDMETHOD(put_DeinterlaceType) (THIS_ int effectNum) PURE;
    };

#ifdef __cplusplus
}
#endif

#endif // __IEZ__

