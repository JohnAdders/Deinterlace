/////////////////////////////////////////////////////////////////////////////
// $Id: memcpy.cpp,v 1.5 2001-12-18 15:56:28 adcockj Exp $
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000 John Adcock.  All rights reserved.
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
/////////////////////////////////////////////////////////////////////////////
// CVS Log
//
// $Log: not supported by cvs2svn $
// Revision 1.4  2001/11/14 08:03:42  adcockj
// Fixed alignment problems
//
// Revision 1.3  2001/11/13 13:51:43  adcockj
// Tidy up code and made to mostly conform to coding standards
// Changed history behaviour
// Made to use DEINTERLACE_INFO throughout
//
// Revision 1.2  2001/11/10 10:35:01  pgubanov
// Correct handling of interlace flags, GreedyH now works fine.
//
// Revision 1.1  2001/11/09 15:31:43  pgubanov
// Adopted from DScaler source tree
//
// Revision 1.1  2001/08/08 15:37:02  tobbej
// moved dmo filter to new directory
//
// Revision 1.1.1.1  2001/07/30 16:14:44  tobbej
// initial import of new dmo filter
//
//
//////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "memcpy.h"

/////////////////////////////////////////////////////////////////////////////
// memcpyMMX
// Uses MMX instructions to move memory around
// does as much as we can in 64 byte chunks (128-byte on SSE machines)
// using MMX instructions
// then copies any extra bytes
// assumes there will be at least 64 bytes to copy
// This code was originally from Borg's bTV plugin SDK 
/////////////////////////////////////////////////////////////////////////////
void __cdecl memcpyMMX(void *Dest, void *Src, size_t nBytes)
{
    __asm 
    {
        mov     esi, dword ptr[Src]
        mov     edi, dword ptr[Dest]
        mov     ecx, nBytes
        shr     ecx, 6                      // nBytes / 64
align 8
CopyLoop:
        movq    mm0, qword ptr[esi]
        movq    mm1, qword ptr[esi+8*1]
        movq    mm2, qword ptr[esi+8*2]
        movq    mm3, qword ptr[esi+8*3]
        movq    mm4, qword ptr[esi+8*4]
        movq    mm5, qword ptr[esi+8*5]
        movq    mm6, qword ptr[esi+8*6]
        movq    mm7, qword ptr[esi+8*7]
        movq    qword ptr[edi], mm0
        movq    qword ptr[edi+8*1], mm1
        movq    qword ptr[edi+8*2], mm2
        movq    qword ptr[edi+8*3], mm3
        movq    qword ptr[edi+8*4], mm4
        movq    qword ptr[edi+8*5], mm5
        movq    qword ptr[edi+8*6], mm6
        movq    qword ptr[edi+8*7], mm7
        add     esi, 64
        add     edi, 64
        loop CopyLoop
        mov     ecx, nBytes
        and     ecx, 63
        cmp     ecx, 0
        je EndCopyLoop
align 8
CopyLoop2:
        mov dl, byte ptr[esi] 
        mov byte ptr[edi], dl
        inc esi
        inc edi
        dec ecx
        jne CopyLoop2
EndCopyLoop:
    }
}

/////////////////////////////////////////////////////////////////////////////
// memcpySSE
// On SSE machines, we can use the 128-bit floating-point registers and
// bypass write caching to copy a bit faster.  The destination has to be
// 16-byte aligned.  
/////////////////////////////////////////////////////////////////////////////
void __cdecl memcpySSE(void *Dest, void *Src, size_t nBytes)
{
    __asm 
    {
        mov     esi, dword ptr[Src]
        mov     edi, dword ptr[Dest]
        mov     ecx, nBytes
        shr     ecx, 7                      // nBytes / 128
align 8
CopyLoopSSE:
        // movaps should be slightly more efficient
        // as the data is 16 byte aligned
        movaps  xmm0, xmmword ptr[esi]
        movaps  xmm1, xmmword ptr[esi+16*1]
        movaps  xmm2, xmmword ptr[esi+16*2]
        movaps  xmm3, xmmword ptr[esi+16*3]
        movaps  xmm4, xmmword ptr[esi+16*4]
        movaps  xmm5, xmmword ptr[esi+16*5]
        movaps  xmm6, xmmword ptr[esi+16*6]
        movaps  xmm7, xmmword ptr[esi+16*7]
        movntps xmmword ptr[edi], xmm0
        movntps xmmword ptr[edi+16*1], xmm1
        movntps xmmword ptr[edi+16*2], xmm2
        movntps xmmword ptr[edi+16*3], xmm3
        movntps xmmword ptr[edi+16*4], xmm4
        movntps xmmword ptr[edi+16*5], xmm5
        movntps xmmword ptr[edi+16*6], xmm6
        movntps xmmword ptr[edi+16*7], xmm7
        add     esi, 128
        add     edi, 128
        loop CopyLoopSSE
        mov     ecx, nBytes
        and     ecx, 127
        cmp     ecx, 0
        je EndCopyLoopSSE
align 8
CopyLoop2SSE:
        mov dl, byte ptr[esi] 
        mov byte ptr[edi], dl
        inc esi
        inc edi
        dec ecx
        jne CopyLoop2SSE
EndCopyLoopSSE:
    }
}
