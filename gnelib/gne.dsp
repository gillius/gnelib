# Microsoft Developer Studio Project File - Name="gne" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gne - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gne.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gne.mak" CFG="gne - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gne - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gne - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gne - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX"gneintern.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\msvc\gne.lib"

!ELSEIF  "$(CFG)" == "gne - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX"gneintern.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"lib\msvc\gned.lib"

!ENDIF 

# Begin Target

# Name "gne - Win32 Release"
# Name "gne - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\ClientConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConditionVariable.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Connection.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConnectionEventGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConnectionEventListener.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Console.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Counter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GNE.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ListServerConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Mutex.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Packet.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PacketParser.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PacketStream.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RawPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ServerConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ServerConnectionCreator.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ServerConnectionListener.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Thread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Time.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Timer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TimerCallback.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\ClientConnection.h
# End Source File
# Begin Source File

SOURCE=.\src\ConditionVariable.h
# End Source File
# Begin Source File

SOURCE=.\src\conioport.h
# End Source File
# Begin Source File

SOURCE=.\src\Connection.h
# End Source File
# Begin Source File

SOURCE=.\src\ConnectionEventGenerator.h
# End Source File
# Begin Source File

SOURCE=.\src\ConnectionEventListener.h
# End Source File
# Begin Source File

SOURCE=.\src\Console.h
# End Source File
# Begin Source File

SOURCE=.\src\Counter.h
# End Source File
# Begin Source File

SOURCE=.\src\GNE.h
# End Source File
# Begin Source File

SOURCE=.\src\gneintern.h
# End Source File
# Begin Source File

SOURCE=.\src\ListServerConnection.h
# End Source File
# Begin Source File

SOURCE=.\src\Mutex.h
# End Source File
# Begin Source File

SOURCE=.\src\Packet.h
# End Source File
# Begin Source File

SOURCE=.\src\PacketParser.h
# End Source File
# Begin Source File

SOURCE=.\src\PacketStream.h
# End Source File
# Begin Source File

SOURCE=.\src\RawPacket.h
# End Source File
# Begin Source File

SOURCE=.\src\ServerConnection.h
# End Source File
# Begin Source File

SOURCE=.\src\ServerConnectionCreator.h
# End Source File
# Begin Source File

SOURCE=.\src\ServerConnectionListener.h
# End Source File
# Begin Source File

SOURCE=.\src\Thread.h
# End Source File
# Begin Source File

SOURCE=.\src\Time.h
# End Source File
# Begin Source File

SOURCE=.\src\Timer.h
# End Source File
# Begin Source File

SOURCE=.\src\TimerCallback.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Authors
# End Source File
# Begin Source File

SOURCE=.\Changes
# End Source File
# Begin Source File

SOURCE=.\doxygen.cfg
# End Source File
# Begin Source File

SOURCE=.\examples\examples.txt
# End Source File
# Begin Source File

SOURCE=.\docs\gnereq.doc
# End Source File
# Begin Source File

SOURCE=.\Readme
# End Source File
# Begin Source File

SOURCE=.\readme.vc
# End Source File
# End Target
# End Project
