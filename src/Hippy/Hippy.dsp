# Microsoft Developer Studio Project File - Name="Hippy" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Hippy - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Hippy.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Hippy.mak" CFG="Hippy - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Hippy - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Hippy - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Hippy - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GR /GX /Zi /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41f /d "NDEBUG"
# ADD RSC /l 0x41f /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"bin/hippy.exe" /OPT:REF /ignore:4089
# SUBTRACT LINK32 /pdb:none /incremental:yes

!ELSEIF  "$(CFG)" == "Hippy - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I ".\devices" /I "../" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x41f /d "_DEBUG"
# ADD RSC /l 0x41f /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /out:"bin/hippy.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Hippy - Win32 Release"
# Name "Hippy - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "CPU View.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\baseWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\dasmwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\debugwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\memdump.cpp
# End Source File
# Begin Source File

SOURCE=.\registerwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\stackwnd.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\addrmng.cpp
# End Source File
# Begin Source File

SOURCE=.\alu.cpp
# End Source File
# Begin Source File

SOURCE=.\asmeditor.cpp
# End Source File
# Begin Source File

SOURCE=.\device.cpp
# End Source File
# Begin Source File

SOURCE=.\dialogs.cpp
# End Source File
# Begin Source File

SOURCE=.\disassembler.cpp
# End Source File
# Begin Source File

SOURCE=.\hippy.rc
# End Source File
# Begin Source File

SOURCE=.\m6800.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\xmlparser.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "CPU View.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\baseWnd.h
# End Source File
# Begin Source File

SOURCE=.\dasmwnd.h
# End Source File
# Begin Source File

SOURCE=.\debugwnd.h
# End Source File
# Begin Source File

SOURCE=.\memdump.h
# End Source File
# Begin Source File

SOURCE=.\registerwnd.h
# End Source File
# Begin Source File

SOURCE=.\stackwnd.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\addrmng.h
# End Source File
# Begin Source File

SOURCE=.\alu.h
# End Source File
# Begin Source File

SOURCE=.\asmeditor.h
# End Source File
# Begin Source File

SOURCE=.\device.h
# End Source File
# Begin Source File

SOURCE=.\dialogs.h
# End Source File
# Begin Source File

SOURCE=.\disassembler.h
# End Source File
# Begin Source File

SOURCE=.\environment.h
# End Source File
# Begin Source File

SOURCE=.\hippy.h
# End Source File
# Begin Source File

SOURCE=.\m6800.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\xmlparser.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\app.ico
# End Source File
# Begin Source File

SOURCE=.\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=.\toolbar2.bmp
# End Source File
# Begin Source File

SOURCE=.\toolbar3.bmp
# End Source File
# End Group
# End Target
# End Project
