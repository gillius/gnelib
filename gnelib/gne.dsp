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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX"../include/gnelib/gneintern.h" /FD /c
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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Fr /YX"../include/gnelib/gneintern.h" /FD /GZ /c
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

SOURCE=.\src\Address.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ClientConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConditionVariable.cpp
# End Source File
# Begin Source File

SOURCE=.\src\conioport.c
# End Source File
# Begin Source File

SOURCE=.\src\Connection.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConnectionEventGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConnectionListener.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConnectionParams.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Console.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConsoleBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConsoleBufferLock.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConsoleStream.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ConsoleStreambuf.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Counter.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CustomPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Error.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Errors.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EventThread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ExitPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GNE.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GNEDebug.cpp
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

SOURCE=.\src\PingPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RateAdjustPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RawPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ReceiveEventListener.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ServerConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ServerConnectionListener.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SocketPair.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SyncConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\src\TextConsole.cpp
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

SOURCE=.\include\gnelib\Address.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ClientConnection.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ConditionVariable.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\conioport.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\Connection.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ConnectionEventGenerator.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ConnectionListener.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ConnectionParams.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ConnectionStats.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\Console.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ConsoleBuffer.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ConsoleBufferLock.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ConsoleStream.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ConsoleStreambuf.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\Counter.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\CustomPacket.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\Error.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\Errors.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\EventThread.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ExitPacket.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\GNE.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\GNEDebug.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\gneintern.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ListServerConnection.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\Mutex.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\Packet.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\PacketFeeder.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\PacketParser.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\PacketStream.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\PingPacket.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\RateAdjustPacket.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\RawPacket.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ReceiveEventListener.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ServerConnection.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\ServerConnectionListener.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\SocketPair.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\SyncConnection.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\SynchronizedObject.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\TextConsole.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\Thread.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\Time.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\Timer.h
# End Source File
# Begin Source File

SOURCE=.\include\gnelib\TimerCallback.h
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

SOURCE=.\src\mainpage
# End Source File
# Begin Source File

SOURCE=.\Readme
# End Source File
# Begin Source File

SOURCE=.\readme.vc
# End Source File
# Begin Source File

SOURCE=.\status.txt
# End Source File
# End Target
# End Project
