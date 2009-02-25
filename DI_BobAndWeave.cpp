/////////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000 John Adcock.  All rights reserved.
// Based on code from Virtual Dub Plug-in by Gunnar Thalin
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
// Revision 1.4  2001/11/13 13:51:43  adcockj
// Tidy up code and made to mostly conform to coding standards
// Changed history behaviour
// Made to use DEINTERLACE_INFO throughout
//
// Revision 1.3  2001/11/09 15:34:27  pgubanov
// Try to work with DScaler plugins. Some code adopted from DIDMO, but less general anyway. For some reason, plugin crashes...
//
// Revision 1.2  2001/11/01 11:04:19  adcockj
// Updated headers
// Checked in changes by Micheal Eskin and Hauppauge
//
/////////////////////////////////////////////////////////////////////////////
// Change Log
//
// Date          Developer             Changes
//
// 30 Dec 2000   Mark Rejhon           Split into separate module
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "di.h"
#include "cpu.h"
#include "memcpy.h"

///////////////////////////////////////////////////////////////////////////////
// DeinterlaceFieldBob
//
// Deinterlaces a field with a tendency to bob rather than weave.  Best for
// high-motion scenes like sports.
//
// The algorithm for this was taken from the 
// Deinterlace - area based Vitual Dub Plug-in by
// Gunnar Thalin
///////////////////////////////////////////////////////////////////////////////
BOOL __cdecl DeinterlaceFieldBob(TDeinterlaceInfo* pInfo)
{
    int Line;
    BYTE* YVal1;
    BYTE* YVal2;
    BYTE* YVal3;
    BYTE* Dest = pInfo->Overlay;
    DWORD LineLength = pInfo->LineLength;
    DWORD Pitch = pInfo->InputPitch;
    
    __int64 qwEdgeDetect;
    __int64 qwThreshold;
    const __int64 Mask = 0xfefefefefefefefe;
    const __int64 YMask    = 0x00ff00ff00ff00ff;

    qwEdgeDetect = EdgeDetect;
    qwEdgeDetect += (qwEdgeDetect << 48) + (qwEdgeDetect << 32) + (qwEdgeDetect << 16);
    qwThreshold = JaggieThreshold;
    qwThreshold += (qwThreshold << 48) + (qwThreshold << 32) + (qwThreshold << 16);


    if(pInfo->PictureHistory[0]->Flags & PICTURE_INTERLACED_ODD)
    {
        YVal1 = pInfo->PictureHistory[0]->pData;
        YVal2 = pInfo->PictureHistory[1]->pData + Pitch;
        YVal3 = YVal1 + Pitch;

        pInfo->pMemcpy(Dest, pInfo->PictureHistory[1]->pData, pInfo->LineLength);
        Dest += pInfo->OverlayPitch;
        
        pInfo->pMemcpy(Dest, YVal1, pInfo->LineLength);
        Dest += pInfo->OverlayPitch;
    }
    else
    {
        YVal1 = pInfo->PictureHistory[0]->pData;
        YVal2 = pInfo->PictureHistory[1]->pData;
        YVal3 = YVal1 + Pitch;

        pInfo->pMemcpy(Dest, YVal1, pInfo->LineLength);
        Dest += pInfo->OverlayPitch;
    }

    for (Line = 0; Line < pInfo->FieldHeight - 1; ++Line)
    {
        // For ease of reading, the comments below assume that we're operating on an odd
        // field (i.e., that bIsOdd is true).  The exact same processing is done when we
        // operate on an even field, but the roles of the odd and even fields are reversed.
        // It's just too cumbersome to explain the algorithm in terms of "the next odd
        // line if we're doing an odd field, or the next even line if we're doing an
        // even field" etc.  So wherever you see "odd" or "even" below, keep in mind that
        // half the time this function is called, those words' meanings will invert.

        _asm
        {
            mov ecx, LineLength
            mov eax, dword ptr [YVal1]
            mov ebx, dword ptr [YVal2]
            mov edx, dword ptr [YVal3]
            mov edi, dword ptr [Dest]
            shr ecx, 3       // there are LineLength / 8 qwords

align 8
MAINLOOP_LABEL:         
            movq mm0, qword ptr[eax] 
            movq mm1, qword ptr[ebx] 
            movq mm2, qword ptr[edx]

            // get intensities in mm3 - 4
            movq mm3, mm0
            movq mm4, mm1
            movq mm5, mm2

            pand mm3, YMask
            pand mm4, YMask
            pand mm5, YMask

            // get average in mm0
            pand  mm0, Mask
            pand  mm2, Mask
            psrlw mm0, 01
            psrlw mm2, 01
            paddw mm0, mm2

            // work out (O1 - E) * (O2 - E) / 2 - EdgeDetect * (O1 - O2) ^ 2 >> 12
            // result will be in mm6

            psrlw mm3, 01
            psrlw mm4, 01
            psrlw mm5, 01

            movq mm6, mm3
            psubw mm6, mm4  //mm6 = O1 - E

            movq mm7, mm5
            psubw mm7, mm4  //mm7 = O2 - E

            pmullw mm6, mm7     // mm0 = (O1 - E) * (O2 - E)

            movq mm7, mm3
            psubw mm7, mm5      // mm7 = (O1 - O2)
            pmullw mm7, mm7     // mm7 = (O1 - O2) ^ 2
            psrlw mm7, 12       // mm7 = (O1 - O2) ^ 2 >> 12
            pmullw mm7, qwEdgeDetect        // mm7  = EdgeDetect * (O1 - O2) ^ 2 >> 12

            psubw mm6, mm7      // mm6 is what we want

            pcmpgtw mm6, qwThreshold

            movq mm7, mm6

            pand mm0, mm6

            pandn mm7, mm1

            por mm7, mm0

            movq qword ptr[edi], mm7

            add eax, 8
            add ebx, 8
            add edx, 8
            add edi, 8
            dec ecx
            jne near MAINLOOP_LABEL
        }

        Dest += pInfo->OverlayPitch;

        // Always use the most recent data verbatim.
        pInfo->pMemcpy(Dest, YVal3, pInfo->LineLength);
        Dest += pInfo->OverlayPitch;

        YVal1 += Pitch;
        YVal2 += Pitch;
        YVal3 += Pitch;
    }

    // Copy last odd line if we're processing an even field.
    if(pInfo->PictureHistory[0]->Flags & PICTURE_INTERLACED_EVEN)
    {
        pInfo->pMemcpy(Dest, YVal2, pInfo->LineLength);
    }

    // clear out the MMX registers ready for doing floating point
    // again
    _asm
    {
        emms
    }

    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// Deinterlace the latest field, with a tendency to weave rather than bob.
// Good for high detail on low-movement scenes.
//
// The algorithm is described in comments below.
//
BOOL __cdecl DeinterlaceFieldWeave(TDeinterlaceInfo* pInfo)
{
    int Line;
    BYTE* YVal1;
    BYTE* YVal2;
    BYTE* YVal3;
    BYTE* YVal4;
    BYTE* OldStack;
    BYTE* Dest = pInfo->Overlay;
    DWORD LineLength = pInfo->LineLength;
    DWORD Pitch = pInfo->InputPitch;

    const __int64 YMask    = 0x00ff00ff00ff00ff;

    __int64 qwSpatialTolerance;
    __int64 qwTemporalTolerance;
    __int64 qwThreshold;
    const __int64 Mask = 0xfefefefefefefefe;

    // Since the code uses MMX to process 4 pixels at a time, we need our constants
    // to be represented 4 times per quadword.
    qwSpatialTolerance = SpatialTolerance;
    qwSpatialTolerance += (qwSpatialTolerance << 48) + (qwSpatialTolerance << 32) + (qwSpatialTolerance << 16);
    qwTemporalTolerance = TemporalTolerance;
    qwTemporalTolerance += (qwTemporalTolerance << 48) + (qwTemporalTolerance << 32) + (qwTemporalTolerance << 16);
    qwThreshold = SimilarityThreshold;
    qwThreshold += (qwThreshold << 48) + (qwThreshold << 32) + (qwThreshold << 16);


    if(pInfo->PictureHistory[0]->Flags & PICTURE_INTERLACED_ODD)
    {
        YVal1 = pInfo->PictureHistory[1]->pData;
        YVal2 = pInfo->PictureHistory[0]->pData;
        YVal3 = YVal1 + Pitch;
        YVal4 = pInfo->PictureHistory[2]->pData + Pitch;

        pInfo->pMemcpy(Dest, YVal1, pInfo->LineLength);
        Dest += pInfo->OverlayPitch;
    }
    else
    {
        YVal1 = pInfo->PictureHistory[1]->pData;
        YVal2 = pInfo->PictureHistory[0]->pData + Pitch;
        YVal3 = YVal1 + Pitch;
        YVal4 = pInfo->PictureHistory[2]->pData + Pitch;

        pInfo->pMemcpy(Dest, pInfo->PictureHistory[0]->pData, pInfo->LineLength);
        Dest += pInfo->OverlayPitch;

        pInfo->pMemcpy(Dest, YVal1, pInfo->LineLength);
        Dest += pInfo->OverlayPitch;
    }


    for (Line = 0; Line < pInfo->FieldHeight - 1; ++Line)
    {
        // For ease of reading, the comments below assume that we're operating on an odd
        // field (i.e., that bIsOdd is true).  The exact same processing is done when we
        // operate on an even field, but the roles of the odd and even fields are reversed.
        // It's just too cumbersome to explain the algorithm in terms of "the next odd
        // line if we're doing an odd field, or the next even line if we're doing an
        // even field" etc.  So wherever you see "odd" or "even" below, keep in mind that
        // half the time this function is called, those words' meanings will invert.

        _asm
        {
            mov dword ptr[OldStack], esi

            mov ecx, LineLength
            mov eax, dword ptr [YVal1]
            mov ebx, dword ptr [YVal2]
            mov edx, dword ptr [YVal3]
            mov esi, dword ptr [YVal4]
            mov edi, dword ptr [Dest]
            shr ecx, 3       // there are LineLength / 8 qwords

align 8
MAINLOOP_LABEL:         
            movq mm0, qword ptr[eax]        // mm0 = E1
            movq mm1, qword ptr[ebx]        // mm1 = O
            movq mm2, qword ptr[edx]        // mm2 = E2

            movq mm3, mm0                   // mm3 = intensity(E1)
            movq mm4, mm1                   // mm4 = intensity(O)
            movq mm6, mm2                   // mm6 = intensity(E2)
            pand mm3, YMask
            pand mm4, YMask
            pand mm6, YMask


            // Average E1 and E2 for interpolated bobbing.
            // leave result in mm0
            pand mm0, Mask                  // mm0 = E1 with lower chroma bit stripped off
            pand mm2, Mask                  // mm2 = E2 with lower chroma bit stripped off
            psrlw mm0, 1                    // mm0 = E1 / 2
            psrlw mm2, 1                    // mm2 = E2 / 2
            paddb mm0, mm2                  // mm2 = (E1 + E2) / 2

            // The meat of the work is done here.  We want to see whether this pixel is
            // close in luminosity to ANY of: its top neighbor, its bottom neighbor,
            // or its predecessor.  To do this without branching, we use MMX's
            // saturation feature, which gives us Z(x) = x if x>=0, or 0 if x<0.
            //
            // The formula we're computing here is
            //      Z(ST - (E1 - O) ^ 2) + Z(ST - (E2 - O) ^ 2) + Z(TT - (Oold - O) ^ 2)
            // where ST is spatial tolerance and TT is temporal tolerance.  The idea
            // is that if a pixel is similar to none of its neighbors, the resulting
            // value will be pretty low, probably zero.  A high value therefore indicates
            // that the pixel had a similar neighbor.  The pixel in the same position
            // in the field before last (Oold) is considered a neighbor since we want
            // to be able to display 1-pixel-high horizontal lines.

            movq mm7, qwSpatialTolerance
            movq mm5, mm3                   // mm5 = E1
            psubsw mm5, mm4                 // mm5 = E1 - O
            psraw mm5, 1
            pmullw mm5, mm5                 // mm5 = (E1 - O) ^ 2
            psubusw mm7, mm5                // mm7 = ST - (E1 - O) ^ 2, or 0 if that's negative

            movq mm3, qwSpatialTolerance
            movq mm5, mm6                   // mm5 = E2
            psubsw mm5, mm4                 // mm5 = E2 - O
            psraw mm5, 1
            pmullw mm5, mm5                 // mm5 = (E2 - O) ^ 2
            psubusw mm3, mm5                // mm0 = ST - (E2 - O) ^ 2, or 0 if that's negative
            paddusw mm7, mm3                // mm7 = (ST - (E1 - O) ^ 2) + (ST - (E2 - O) ^ 2)

            movq mm3, qwTemporalTolerance
            movq mm5, qword ptr[esi]        // mm5 = Oold
            pand mm5, YMask
            psubsw mm5, mm4                 // mm5 = Oold - O
            psraw mm5, 1 // XXX
            pmullw mm5, mm5                 // mm5 = (Oold - O) ^ 2
            psubusw mm3, mm5                // mm0 = TT - (Oold - O) ^ 2, or 0 if that's negative
            paddusw mm7, mm3                // mm7 = our magic number

            // Now compare the similarity totals against our threshold.  The pcmpgtw
            // instruction will populate the target register with a bunch of mask bits,
            // filling words where the comparison is true with 1s and ones where it's
            // false with 0s.  A few ANDs and NOTs and an OR later, we have bobbed
            // values for pixels under the similarity threshold and weaved ones for
            // pixels over the threshold.

            pcmpgtw mm7, qwThreshold        // mm7 = 0xffff where we're greater than the threshold, 0 elsewhere
            movq mm6, mm7                   // mm6 = 0xffff where we're greater than the threshold, 0 elsewhere
            pand mm7, mm1                   // mm7 = weaved data where we're greater than the threshold, 0 elsewhere
            pandn mm6, mm0                  // mm6 = bobbed data where we're not greater than the threshold, 0 elsewhere
            por mm7, mm6                    // mm7 = bobbed and weaved data

            movq qword ptr[edi], mm7

            add eax, 8
            add ebx, 8
            add edx, 8
            add edi, 8
            add esi, 8
            dec ecx
            jne near MAINLOOP_LABEL

            mov esi, dword ptr[OldStack]
        }

        Dest += pInfo->OverlayPitch;
        // Copy the even scanline below this one to the overlay buffer, since we'll be
        // adapting the current scanline to the even lines surrounding it.  The scanline
        // above has already been copied by the previous pass through the loop.
        pInfo->pMemcpy(Dest, YVal3, LineLength);
        Dest += pInfo->OverlayPitch;

        YVal1 += Pitch;
        YVal2 += Pitch;
        YVal3 += Pitch;
        YVal4 += Pitch;
    }

    // Copy last odd line if we're processing an odd field.
    if(pInfo->PictureHistory[0]->Flags & PICTURE_INTERLACED_ODD)
    {
        pInfo->pMemcpy(Dest, YVal2, LineLength);
    }

    // clear out the MMX registers ready for doing floating point
    // again
    _asm
    {
        emms
    }

    return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// Copies memory to two locations using MMX registers for speed.
void memcpyBOBMMX(void *Dest1, void *Dest2, void *Src, size_t nBytes)
{
    __asm
    {
        mov     esi, dword ptr[Src]
        mov     edi, dword ptr[Dest1]
        mov     ebx, dword ptr[Dest2]
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
        movq    qword ptr[ebx], mm0
        movq    qword ptr[ebx+8*1], mm1
        movq    qword ptr[ebx+8*2], mm2
        movq    qword ptr[ebx+8*3], mm3
        movq    qword ptr[ebx+8*4], mm4
        movq    qword ptr[ebx+8*5], mm5
        movq    qword ptr[ebx+8*6], mm6
        movq    qword ptr[ebx+8*7], mm7
        add     esi, 64
        add     edi, 64
        add     ebx, 64
        dec ecx
        jne near CopyLoop

        mov     ecx, nBytes
        and     ecx, 63
        cmp     ecx, 0
        je EndCopyLoop
align 8
CopyLoop2:
        mov dl, byte ptr[esi] 
        mov byte ptr[edi], dl
        mov byte ptr[ebx], dl
        inc esi
        inc edi
        inc ebx
        dec ecx
        jne near CopyLoop2
EndCopyLoop:
        emms
    }
}


/////////////////////////////////////////////////////////////////////////////
// Simple Bob.  Copies the most recent field to the overlay, with each scanline
// copied twice.
/////////////////////////////////////////////////////////////////////////////
BOOL __cdecl Bob(TDeinterlaceInfo* pInfo)
{
    int i;
    BYTE* lpOverlay = pInfo->Overlay;
    BYTE* CurrentLine = pInfo->PictureHistory[0]->pData;
    DWORD Pitch = pInfo->InputPitch;

    // No recent data?  We can't do anything.
    if (CurrentLine == NULL)
    {
        return FALSE;
    }

    if (pInfo->PictureHistory[0]->Flags & PICTURE_INTERLACED_ODD)
    {
        pInfo->pMemcpy(lpOverlay, CurrentLine, pInfo->LineLength);   // extra copy of first line
        lpOverlay += pInfo->OverlayPitch;                    // and offset out output ptr
        for (i = 0; i < pInfo->FieldHeight - 1; i++)
        {
            memcpyBOBMMX(lpOverlay, lpOverlay + pInfo->OverlayPitch,
                CurrentLine, pInfo->LineLength);
            lpOverlay += 2 * pInfo->OverlayPitch;
            CurrentLine += Pitch;
        }
        pInfo->pMemcpy(lpOverlay, CurrentLine, pInfo->LineLength);   // only 1 copy of last line
    }
    else
    {
        for (i = 0; i < pInfo->FieldHeight; i++)
        {
            memcpyBOBMMX(lpOverlay, lpOverlay + pInfo->OverlayPitch,
                CurrentLine, pInfo->LineLength);
            lpOverlay += 2 * pInfo->OverlayPitch;
            CurrentLine += Pitch;
        }
    }

    // need to clear up MMX registers
    _asm
    {
        emms
    }
       
    return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// Simple Weave.  Copies alternating scanlines from the most recent fields.
BOOL __cdecl Weave(TDeinterlaceInfo* pInfo)
{
    int i;
    BYTE *lpOverlay = pInfo->Overlay;
    BYTE* CurrentOddLine;
    BYTE* CurrentEvenLine;
    DWORD Pitch = pInfo->InputPitch;

    if (pInfo->PictureHistory[0]->Flags & PICTURE_INTERLACED_ODD)
    {
        CurrentOddLine = pInfo->PictureHistory[0]->pData;
        CurrentEvenLine = pInfo->PictureHistory[1]->pData;
    }
    else
    {
        CurrentOddLine = pInfo->PictureHistory[1]->pData;
        CurrentEvenLine = pInfo->PictureHistory[0]->pData;
    }
    
    for (i = 0; i < pInfo->FieldHeight; i++)
    {
        pInfo->pMemcpy(lpOverlay, CurrentEvenLine, pInfo->LineLength);
        lpOverlay += pInfo->OverlayPitch;
        CurrentEvenLine += Pitch;

        pInfo->pMemcpy(lpOverlay, CurrentOddLine, pInfo->LineLength);
        lpOverlay += pInfo->OverlayPitch;
        CurrentOddLine += Pitch;
    }
    _asm
    {
        emms
    }
    return TRUE;
}
