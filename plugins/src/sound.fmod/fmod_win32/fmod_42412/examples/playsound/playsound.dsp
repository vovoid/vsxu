# Microsoft Developer Studio Project File - Name="playsound" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=playsound - Win32 Release C
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "playsound.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "playsound.mak" CFG="playsound - Win32 Release C"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "playsound - Win32 Release C" (based on "Win32 (x86) Console Application")
!MESSAGE "playsound - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "playsound - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "playsound - Win32 Debug C" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cwcl.exe
RSC=rc.exe

!IF  "$(CFG)" == "playsound - Win32 Release C"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_C"
# PROP BASE Intermediate_Dir "Release_C"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX- /O1 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=cwlink.exe
# ADD BASE LINK32 wsock32.lib winmm.lib ole32.lib user32.lib msacm32.lib /nologo /subsystem:console /machine:I386 /out:"playsound.exe"
# SUBTRACT BASE LINK32 /incremental:yes /map
# ADD LINK32 ../../api/lib/fmodex_vc.lib /nologo /subsystem:console /machine:I386 /out:"playsound.exe"
# SUBTRACT LINK32 /incremental:yes /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\api\fmodex.dll .
# End Special Build Tool

!ELSEIF  "$(CFG)" == "playsound - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release_CPP"
# PROP BASE Intermediate_Dir "Release_CPP"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX- /O1 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=cwlink.exe
# ADD BASE LINK32 wsock32.lib winmm.lib ole32.lib user32.lib msacm32.lib /nologo /subsystem:console /machine:I386 /out:"playsound.exe"
# SUBTRACT BASE LINK32 /incremental:yes /map
# ADD LINK32 ../../api/lib/fmodex_vc.lib /nologo /subsystem:console /machine:I386 /out:"playsound.exe"
# SUBTRACT LINK32 /incremental:yes /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\api\fmodex.dll .
# End Special Build Tool

!ELSEIF  "$(CFG)" == "playsound - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug_CPP"
# PROP BASE Intermediate_Dir "Debug_CPP"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=cwlink.exe
# ADD BASE LINK32 ../../api/lib/fmod4vc.lib /nologo /subsystem:console /machine:I386 /out:"playsound.exe"
# SUBTRACT BASE LINK32 /incremental:yes /map
# ADD LINK32 ../../api/lib/fmodex_vc.lib /nologo /subsystem:console /debug /machine:I386 /out:"playsound.exe"
# SUBTRACT LINK32 /incremental:yes /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\api\fmodex.dll .
# End Special Build Tool

!ELSEIF  "$(CFG)" == "playsound - Win32 Debug C"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Debug_C"
# PROP BASE Intermediate_Dir "Debug_C"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O1 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT BASE CPP /YX
# ADD CPP /nologo /MD /W3 /GX /Zi /Od /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=cwlink.exe
# ADD BASE LINK32 ../../api/lib/fmod4vc.lib /nologo /subsystem:console /machine:I386 /out:"playsound.exe"
# SUBTRACT BASE LINK32 /incremental:yes /map
# ADD LINK32 ../../api/lib/fmodex_vc.lib /nologo /subsystem:console /debug /machine:I386 /out:"playsound.exe"
# SUBTRACT LINK32 /incremental:yes /map
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy ..\..\api\fmodex.dll .
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "playsound - Win32 Release C"
# Name "playsound - Win32 Release"
# Name "playsound - Win32 Debug"
# Name "playsound - Win32 Debug C"
# Begin Source File

SOURCE=.\main.c

!IF  "$(CFG)" == "playsound - Win32 Release C"

!ELSEIF  "$(CFG)" == "playsound - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "playsound - Win32 Debug"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "playsound - Win32 Debug C"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\main.cpp

!IF  "$(CFG)" == "playsound - Win32 Release C"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "playsound - Win32 Release"

!ELSEIF  "$(CFG)" == "playsound - Win32 Debug"

!ELSEIF  "$(CFG)" == "playsound - Win32 Debug C"

# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Target
# End Project
