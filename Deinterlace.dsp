# Microsoft Developer Studio Project File - Name="Deinterlace" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Deinterlace - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Deinterlace.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Deinterlace.mak" CFG="Deinterlace - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Deinterlace - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Deinterlace - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Deinterlace - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Gz /MD /W3 /Gy /I "..\..\..\..\..\classes\base" /D DBG=0 /D WINVER=0x400 /D _X86_=1 /D "_DLL" /D "_MT" /D "_WIN32" /D "WIN32" /D "STRICT" /D "INC_OLE2" /D try=__try /D except=__except /D leave=__leave /D finally=__finally /Oxs /GF /D_WIN32_WINNT=-0x0400 /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 quartz.lib strmbase.lib msvcrt.lib winmm.lib vfw32.lib kernel32.lib advapi32.lib version.lib largeint.lib user32.lib gdi32.lib comctl32.lib ole32.lib olepro32.lib oleaut32.lib uuid.lib /nologo /base:"0x1d1c0000" /entry:"DllEntryPoint@12" /dll /pdb:none /machine:I386 /nodefaultlib /out:"Deinterlace.ax" /subsystem:windows,4.0 /opt:ref /release /debug:none
# Begin Custom Build - Performing registration
OutDir=.
TargetPath=.\Deinterlace.ax
InputPath=.\Deinterlace.ax
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "Deinterlace - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /Gz /MDd /W3 /Z7 /Gy /I "..\..\..\..\..\classes\base" /D "INC_OLE2" /D "STRICT" /D _WIN32_WINNT=0x0400 /D "WIN32" /D "_WIN32" /D "_MT" /D "_DLL" /D _X86_=1 /D WINVER=0x0400 /D DBG=1 /D "DEBUG" /D "_DEBUG" /D try=__try /D except=__except /D leave=__leave /D finally=__finally /FR /Oid /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 strmbasd.lib quartz.lib msvcrtd.lib winmm.lib vfw32.lib kernel32.lib advapi32.lib version.lib largeint.lib user32.lib gdi32.lib comctl32.lib ole32.lib olepro32.lib oleaut32.lib uuid.lib winmm.lib /nologo /base:"0x1d1c0000" /entry:"DllEntryPoint@12" /dll /pdb:none /machine:I386 /nodefaultlib /out:".\Deinterlace.ax" /debug:mapped,full /subsystem:windows,4.0
# Begin Custom Build - Performing registration
OutDir=.
TargetPath=.\Deinterlace.ax
InputPath=.\Deinterlace.ax
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "Deinterlace - Win32 Release"
# Name "Deinterlace - Win32 Debug"
# Begin Source File

SOURCE=.\Copying.txt
# End Source File
# Begin Source File

SOURCE=.\Deinterlace.cpp
DEP_CPP_DEINT=\
	".\Deinterlace.h"\
	".\DeinterlaceGuids.h"\
	".\DeinterlaceProperties.h"\
	".\IDeinterlace.h"\
	{$(INCLUDE)}"amextra.h"\
	{$(INCLUDE)}"amfilter.h"\
	{$(INCLUDE)}"audevcod.h"\
	{$(INCLUDE)}"cache.h"\
	{$(INCLUDE)}"combase.h"\
	{$(INCLUDE)}"cprop.h"\
	{$(INCLUDE)}"ctlutil.h"\
	{$(INCLUDE)}"dllsetup.h"\
	{$(INCLUDE)}"fourcc.h"\
	{$(INCLUDE)}"measure.h"\
	{$(INCLUDE)}"msgthrd.h"\
	{$(INCLUDE)}"mtype.h"\
	{$(INCLUDE)}"outputq.h"\
	{$(INCLUDE)}"pstream.h"\
	{$(INCLUDE)}"refclock.h"\
	{$(INCLUDE)}"reftime.h"\
	{$(INCLUDE)}"renbase.h"\
	{$(INCLUDE)}"Schedule.h"\
	{$(INCLUDE)}"source.h"\
	{$(INCLUDE)}"streams.h"\
	{$(INCLUDE)}"strmctl.h"\
	{$(INCLUDE)}"sysclock.h"\
	{$(INCLUDE)}"transfrm.h"\
	{$(INCLUDE)}"transip.h"\
	{$(INCLUDE)}"videoctl.h"\
	{$(INCLUDE)}"vtrans.h"\
	{$(INCLUDE)}"winctrl.h"\
	{$(INCLUDE)}"winutil.h"\
	{$(INCLUDE)}"wxdebug.h"\
	{$(INCLUDE)}"wxlist.h"\
	{$(INCLUDE)}"wxutil.h"\
	
# End Source File
# Begin Source File

SOURCE=.\Deinterlace.def
# End Source File
# Begin Source File

SOURCE=.\Deinterlace.h
# End Source File
# Begin Source File

SOURCE=.\Deinterlace.rc
# End Source File
# Begin Source File

SOURCE=.\DeinterlaceGuids.h
# End Source File
# Begin Source File

SOURCE=.\DeinterlaceProperties.Cpp
DEP_CPP_DEINTE=\
	".\Deinterlace.h"\
	".\DeinterlaceGuids.h"\
	".\DeinterlaceProperties.h"\
	".\IDeinterlace.h"\
	{$(INCLUDE)}"amextra.h"\
	{$(INCLUDE)}"amfilter.h"\
	{$(INCLUDE)}"audevcod.h"\
	{$(INCLUDE)}"cache.h"\
	{$(INCLUDE)}"combase.h"\
	{$(INCLUDE)}"cprop.h"\
	{$(INCLUDE)}"ctlutil.h"\
	{$(INCLUDE)}"dllsetup.h"\
	{$(INCLUDE)}"fourcc.h"\
	{$(INCLUDE)}"measure.h"\
	{$(INCLUDE)}"msgthrd.h"\
	{$(INCLUDE)}"mtype.h"\
	{$(INCLUDE)}"outputq.h"\
	{$(INCLUDE)}"pstream.h"\
	{$(INCLUDE)}"refclock.h"\
	{$(INCLUDE)}"reftime.h"\
	{$(INCLUDE)}"renbase.h"\
	{$(INCLUDE)}"Schedule.h"\
	{$(INCLUDE)}"source.h"\
	{$(INCLUDE)}"streams.h"\
	{$(INCLUDE)}"strmctl.h"\
	{$(INCLUDE)}"sysclock.h"\
	{$(INCLUDE)}"transfrm.h"\
	{$(INCLUDE)}"transip.h"\
	{$(INCLUDE)}"videoctl.h"\
	{$(INCLUDE)}"vtrans.h"\
	{$(INCLUDE)}"winctrl.h"\
	{$(INCLUDE)}"winutil.h"\
	{$(INCLUDE)}"wxdebug.h"\
	{$(INCLUDE)}"wxlist.h"\
	{$(INCLUDE)}"wxutil.h"\
	
# End Source File
# Begin Source File

SOURCE=.\DeinterlaceProperties.h
# End Source File
# Begin Source File

SOURCE=.\IDeinterlace.h
# End Source File
# Begin Source File

SOURCE=.\News.txt
# End Source File
# Begin Source File

SOURCE=.\README.txt
# End Source File
# Begin Source File

SOURCE=.\Thanks.txt
# End Source File
# Begin Source File

SOURCE=.\Todo.txt
# End Source File
# End Target
# End Project
