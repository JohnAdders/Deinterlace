/////////////////////////////////////////////////////////////////////////////
// globals.h
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
//
// ########### IMPORTANT ###########
//
// Globals are ugly and complicates keeping track of code.
// If you see variables that are used by only one module, please 
// delete it from here, and declare the variable at the top of the module 
// and then delete the 'extern' declaration.  Your Computer Science class
// should have taught you to avoid excessive global variables ;-)
// It will also make it easier to rewrite to C++ as well in the future.
//
// This software was based on Multidec 5.6 Those portions are
// Copyright (C) 1999/2000 Espresso (echter_espresso@hotmail.com)
//
/////////////////////////////////////////////////////////////////////////////
// Change Log
//
// Date          Developer             Changes
//
// 24 Jul 2000   John Adcock           Original Release
//                                     Translated most code from German
//                                     Combined Header files
//                                     Cut out all decoding
//                                     Cut out digital hardware stuff
//
// 02 Jan 2001   John Adcock           Added CurrentVBILines and removed BurstDMA
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __GLOBALS_H___
#define __GLOBALS_H___

#include "defines.h"
#include "structs.h"

//------------------------------------------------------------------------
// ######### IMPORTANT! READ ME FIRST! ##########
//
// Only put variables here IF it is necessary to share the same variable
// between multiple modules.  If it is not necessary, please instead
// put your variable declaration at the top of the module you are editing.
// This makes other programmers' lives easier keeping track of variables.
//------------------------------------------------------------------------

extern BITMAPINFO *VTCharSetLarge ;
extern BITMAPINFO *VTCharSetSmall;
extern BITMAPINFO *VTScreen[MAXVTDIALOG];
extern BOOL AutoStereoSelect;
extern BOOL bFilterBothRedBlue;
extern BOOL bIsOddField;
extern BOOL bNoBlue,bNoRed,bNoGreen;
extern BOOL bSaveSettings;
extern BOOL Capture_VBI;
extern BOOL Display_Status_Bar;
extern BOOL Has_MSP;
extern BOOL MSPToneControl;
extern BOOL System_In_Mute;
extern BOOL USE_MIXER;
extern BOOL VD_RAW;
extern BOOL VT_ALWAYS_EXPORT;
extern BOOL VT_HEADERS;
extern BOOL VT_NL;
extern BOOL VT_REVEAL;
extern BOOL VT_STRIPPED;
extern BOOL VTLarge;
extern BYTE* pDisplay[5];
extern BYTE* pVBILines[5];
extern BYTE VT_Header_Line[40];
extern char BTTyp[30];
extern char IC_BASE_DIR[128];
extern BOOL InitialSuperBass;
extern char InitialBalance;
extern char InitialBass;
extern char InitialEqualizer[5];
extern char InitialLoudness;
extern char InitialMixer[MAXPNAMELEN];
extern char InitialMixerConnect[MIXER_LONG_NAME_CHARS];
extern char InitialMixerLine[MIXER_LONG_NAME_CHARS];
extern char InitialSpatial;
extern char InitialTreble;
extern char MSPStatus[30];
extern char VD_DIR[128];
extern char VPS_chname[9];
extern char VPS_lastname[9];
extern char VT_BASE_DIR[128];
extern HANDLE FilterEvent[MAXFILTER];
extern HANDLE hInst;
extern HMIXER hMixer;
extern HWND hWnd;
extern HWND hwndAudioField;
extern HWND hwndFPSField;
extern HWND hwndKeyField,hwndFPSField;
extern HWND hwndTextField, hwndPalField;
extern HWND ShowPDCInfo;
extern HWND ShowVPSInfo;
extern HWND ShowVTInfo;
extern HWND SplashWnd;
extern HWND hwndStatusBar;
extern int NumberOfProcessors;
extern int DecodeProcessor;
extern int BytesOut;
extern int CurrentConnect;
extern int CurrentLine;
extern int CurrentMixer;
extern int CurrentMode;
extern int CurrentPitch;
extern int CurrentProgramm;
extern int CurrentX;
extern int CurrentY;
extern int CurrentVBILines;
extern int DeviceNumber;
extern int FirstWidthEven;
extern int FirstWidthOdd;
extern int InitialContrast;
extern int InitialProg;
extern int InitialSaturationU;
extern int InitialSaturationV;
extern int InitialBrightness;
extern int InitialHue;

// MAE 2 Nov 2000 - Start of change for Macrovision fix
extern int InitialBDelay;
// MAE 2 Nov 2000 - End of change for Macrovision fix

extern int InitialOverscan;
extern int InitialVolume;
extern int LineFlag;
extern int MainProcessor;
extern int MasterTestzeilen;
extern int MIXER_LINKER_KANAL;
extern int MIXER_RECHTER_KANAL;
extern int MixerVolumeMax;
extern int MixerVolumeStep;
extern int MSPMajorMode;
extern int MSPMinorMode;
extern int MSPMode;
extern int MSPStereo;
extern int TVTYPE;
extern int VideoSource;
extern int VT_Cache;
extern int VT_LATIN;
extern LPDIRECTDRAW lpDD;
extern LPDIRECTDRAWSURFACE lpDDOverlay;
extern LPDIRECTDRAWSURFACE lpDDOverlayBack;
extern LPDIRECTDRAWSURFACE lpDDSurface;
extern BYTE* lpOverlay;
extern BYTE* lpOverlayBack;
extern long OverlayPitch;
extern MIXERCAPS *MixerDev;
extern MIXERLINE *MixerLine;
extern short nLevelHigh;
extern short nLevelLow;
extern struct SOTREC SOTInfoRec;
extern struct TInterCast InterCast;
extern struct TLNB LNB;
extern struct TMixerAccess Mute;
extern struct TMixerAccess Volume;
extern struct TMixerLoad MixerLoad[64];
extern struct TPacket30 Packet30;
extern struct TSoundSystem SoundSystem;
extern struct TVDat VDat;
extern struct TVT VTFrame[800];
extern struct TVTDialog VTDialog[MAXVTDIALOG];
extern struct TTVSetting TVSettings[];
extern unsigned int ManuellAudio[8];
extern unsigned short cp_odd[256][285];
extern unsigned short UTCount;
extern unsigned short UTCount;
extern unsigned short UTPages[12];
extern unsigned short UTPages[12];
extern unsigned short VTColourTable[9];
extern PMemStruct Display_dma[5];
extern PMemStruct Risc_dma;
extern PMemStruct Vbi_dma[5];

extern int emsizex;
extern int emsizey;
extern int emstartx;
extern int emstarty;
extern BOOL bAlwaysOnTop;
extern BOOL bIsFullScreen;
extern BOOL bDisplaySplashScreen;

extern int pgsizex;
extern int pgsizey;
extern int pgstartx;
extern int pgstarty;

extern struct TBL ButtonList[15];

extern int PriorClassId;
extern int ThreadClassId;
extern BOOL bDisplayStatusBar;

extern BOOL Show_Menu;
extern int AudioSource;

extern int CountryCode;

extern struct TProgramm Programm[MAXPROGS+1];
extern HFONT currFont;
extern HBITMAP RedBulb;
extern HBITMAP GreenBulb;

extern char BTVendorID[10];
extern char BTDeviceID[10];
extern char MSPVersion[16];

extern BOOL	Wait_For_Flip;          // User parm, default=FALSE
extern BOOL	Hurry_When_Late;        // " , default=TRUE, skip processing if behind
extern long	Sleep_Interval;         // " , default=0, how long to wait for BT chip
extern int	Back_Buffers;			// " , nuber of video back buffers
extern COLORREF OverlayColor;

// Add some global fields for Blended Clip Deinterlace - Tom Barry 11/05/00
// These should be treated as part of that, if it ever becomes a class.
extern	int		BlcMinimumClip;
extern	UINT	BlcPixelMotionSense;
extern	int		BlcRecentMotionSense;
extern	UINT	BlcMotionAvgPeriod;
extern	UINT	BlcPixelCombSense;
extern	UINT	BlcRecentCombSense;
extern	UINT	BlcCombAvgPeriod;
extern	UINT	BlcHighCombSkip;
extern	UINT	BlcLowMotionSkip;
extern  BOOL	BlcUseInterpBob;
extern  BOOL	BlcBlendChroma;
extern  BOOL	BlcWantsToFlip;
extern  UINT	BlcAverageMotions[5][2];
extern  UINT	BlcTotalAverageMotion;
extern  UINT	BlcAverageCombs[5][2];
extern  UINT	BlcTotalAverageComb;
extern  BOOL	BlcShowControls;
extern  UINT	BlcVerticalSmoothing;

// Add some global fields for Adv Video Flags - Tom Barry 12/15/00
extern  BYTE	BtAgcDisable;
extern  BYTE	BtCrush;
extern  BYTE	BtEvenChromaAGC;
extern  BYTE	BtOddChromaAGC;
extern  BYTE	BtEvenLumaPeak;
extern  BYTE	BtOddLumaPeak;
extern  BYTE	BtFullLumaRange;
extern  BYTE	BtEvenLumaDec;
extern  BYTE	BtOddLumaDec;
extern  BYTE	BtEvenComb;
extern  BYTE	BtOddComb;
extern  BYTE	BtColorBars;
extern  BYTE	BtGammaCorrection;
extern	BYTE    BtCoring;
extern  BYTE    BtHorFilter;
extern	BYTE    BtVertFilter;
extern	BYTE    BtColorKill;
extern	BYTE    BtWhiteCrushUp;
extern	BYTE    BtWhiteCrushDown;

extern  UINT    CpuFeatureFlags;		// TRB 12/20/00 Processor capability flags

//------------------------------------------------------------------------
// ######### IMPORTANT! READ ME FIRST! ##########
//
// Only put variables here IF it is necessary to share the same variable
// between multiple modules.  If it is not necessary, please instead
// put your variable declaration at the top of the module you are editing.
// This makes other programmers' lives easier keeping track of variables.
//------------------------------------------------------------------------

#endif
