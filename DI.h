///////////////////////////////////////////////////////////////////////////////
// $Id: DI.h,v 1.5 2001-12-11 17:31:58 adcockj Exp $
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

#ifndef __DI_H__
#define __DI_H__

#include "../DScaler/Api/DS_Deinterlace.h"

DEINTERLACE_FUNC Bob;
DEINTERLACE_FUNC Weave;
DEINTERLACE_FUNC DeinterlaceFieldWeave;
DEINTERLACE_FUNC DeinterlaceFieldBob;
DEINTERLACE_FUNC DeinterlaceFieldTwoFrame;
DEINTERLACE_FUNC BlendedClipping;

void memcpyBOBMMX(void *Dest1, void *Dest2, void *Src, size_t nBytes);
long GetCombFactor(short** pLines1, short** pLines2);
long CompareFields(short** pLines1, short** pLines2, RECT *rect);

extern long EdgeDetect;
extern long JaggieThreshold;
extern long DiffThreshold;
extern long SpatialTolerance;
extern long TemporalTolerance;
extern long SimilarityThreshold;


#endif
