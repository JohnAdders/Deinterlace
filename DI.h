///////////////////////////////////////////////////////////////////////////////
// $Id: DI.h,v 1.3 2001-11-09 15:34:27 pgubanov Exp $
/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2000 John Adcock.  All rights reserved.
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
/////////////////////////////////////////////////////////////////////////////
// Change Log
//
// Date          Developer             Changes
//
// 24 Jul 2000   John Adcock           Put all my deinterlacing code into this
//                                     file
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __DEINTERLACE_H___
#define __DEINTERLACE_H___

#define MAX_FIELD_HISTORY 5

/////////////////////////////////////////////////////////////////////////////
// Describes the inputs to a deinterlacing algorithm.  Some of these values
// are also available in global variables, but they're duplicated here in
// anticipation of eventually supporting deinterlacing plugins.
typedef struct {
	// Data from the most recent several odd and even fields, from newest
	// to oldest, i.e., OddLines[0] is always the most recent odd field.
	// Pointers are NULL if the field in question isn't valid, e.g. because
	// the program just started or a field was skipped.
	short **OddLines[MAX_FIELD_HISTORY];
	short **EvenLines[MAX_FIELD_HISTORY];

	// Current overlay buffer pointer.
	BYTE *Overlay;

	// True if the most recent field is an odd one; false if it was even.
	BOOL IsOdd;

	// Overlay pitch (number of bytes between scanlines).
	DWORD OverlayPitch;

	// Number of bytes of actual data in each scanline.  May be less than
	// OverlayPitch since the overlay's scanlines might have alignment
	// requirements.  Generally equal to FrameWidth * 2.
	DWORD LineLength;

	// Number of pixels in each scanline.
	int FrameWidth;

	// Number of scanlines per frame.
	int FrameHeight;

	// Number of scanlines per field.  FrameHeight / 2, mostly for
	// cleanliness so we don't have to keep dividing FrameHeight by 2.
	int FieldHeight;
} MY_DEINTERLACE_INFO;

// Deinterlace functions return true if the overlay is ready to be displayed.
typedef BOOL (MY_DEINTERLACE_FUNC)(MY_DEINTERLACE_INFO *info);

MY_DEINTERLACE_FUNC Bob;
MY_DEINTERLACE_FUNC Weave;
MY_DEINTERLACE_FUNC DeinterlaceFieldWeave;
MY_DEINTERLACE_FUNC DeinterlaceFieldBob;
MY_DEINTERLACE_FUNC DeinterlaceFieldTwoFrame;
MY_DEINTERLACE_FUNC BlendedClipping;

void memcpyBOBMMX(void *Dest1, void *Dest2, void *Src, size_t nBytes);
long GetCombFactor(short** pLines1, short** pLines2);
long CompareFields(short** pLines1, short** pLines2, RECT *rect);

extern long BitShift;
extern long EdgeDetect;
extern long JaggieThreshold;
extern long DiffThreshold;
extern long SpatialTolerance;
extern long TemporalTolerance;
extern long SimilarityThreshold;


#endif
