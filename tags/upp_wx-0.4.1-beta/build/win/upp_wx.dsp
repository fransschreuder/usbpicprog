# Microsoft Developer Studio Project File - Name="upp_wx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=upp_wx - Win32 Static Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "upp_wx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "upp_wx.mak" CFG="upp_wx - Win32 Static Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "upp_wx - Win32 DLL Debug" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 DLL Release" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 Static Debug" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 Static Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "upp_wx - Win32 DLL Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\.."
# PROP BASE Intermediate_Dir "vcmswud_dll\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\.."
# PROP Intermediate_Dir "vcmswud_dll\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswud" /I "$(WXWIN)\include" /Od /Gm /Zi /Fd..\..\usbpicprog.pdb /I "..\..\src" /I "..\..\win\deps" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswud" /I "$(WXWIN)\include" /Od /Gm /Zi /Fd..\..\usbpicprog.pdb /I "..\..\src" /I "..\..\win\deps" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswud" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i ..\..\win\deps
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswud" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i ..\..\win\deps
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb-1.0.lib setupapi.lib wxmsw29ud_adv.lib wxbase29ud_xml.lib wxmsw29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"..\..\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /debug /pdb:"..\..\usbpicprog.pdb" /libpath:"..\..\win\deps\debug\x86"
# ADD LINK32 libusb-1.0.lib setupapi.lib wxmsw29ud_adv.lib wxbase29ud_xml.lib wxmsw29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"..\..\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /debug /pdb:"..\..\usbpicprog.pdb" /libpath:"..\..\win\deps\debug\x86"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 DLL Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\.."
# PROP BASE Intermediate_Dir "vcmswu_dll\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\.."
# PROP Intermediate_Dir "vcmswu_dll\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswu" /I "$(WXWIN)\include" /O2 /Fd..\..\usbpicprog.pdb /I "..\..\src" /I "..\..\win\deps" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswu" /I "$(WXWIN)\include" /O2 /Fd..\..\usbpicprog.pdb /I "..\..\src" /I "..\..\win\deps" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "_UNICODE" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswu" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i ..\..\win\deps
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "_UNICODE" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswu" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i ..\..\win\deps
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb-1.0.lib setupapi.lib wxmsw29u_adv.lib wxbase29u_xml.lib wxmsw29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"..\..\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /pdb:"..\..\usbpicprog.pdb" /libpath:"..\..\win\deps\release\x86"
# ADD LINK32 libusb-1.0.lib setupapi.lib wxmsw29u_adv.lib wxbase29u_xml.lib wxmsw29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"..\..\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /pdb:"..\..\usbpicprog.pdb" /libpath:"..\..\win\deps\release\x86"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 Static Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "..\.."
# PROP BASE Intermediate_Dir "vcmswud\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\.."
# PROP Intermediate_Dir "vcmswud\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswud" /I "$(WXWIN)\include" /Od /Gm /Zi /Fd..\..\usbpicprog.pdb /I "..\..\src" /I "..\..\win\deps" /D "WIN32" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswud" /I "$(WXWIN)\include" /Od /Gm /Zi /Fd..\..\usbpicprog.pdb /I "..\..\src" /I "..\..\win\deps" /D "WIN32" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswud" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i ..\..\win\deps
# ADD RSC /l 0x409 /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswud" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i ..\..\win\deps
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb-1.0.lib setupapi.lib wxmsw29ud_adv.lib wxbase29ud_xml.lib wxmsw29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"..\..\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /debug /pdb:"..\..\usbpicprog.pdb" /libpath:"..\..\win\deps\debug\x86"
# ADD LINK32 libusb-1.0.lib setupapi.lib wxmsw29ud_adv.lib wxbase29ud_xml.lib wxmsw29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"..\..\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /debug /pdb:"..\..\usbpicprog.pdb" /libpath:"..\..\win\deps\debug\x86"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 Static Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "..\.."
# PROP BASE Intermediate_Dir "vcmswu\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\.."
# PROP Intermediate_Dir "vcmswu\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswu" /I "$(WXWIN)\include" /O2 /Fd..\..\usbpicprog.pdb /I "..\..\src" /I "..\..\win\deps" /D "WIN32" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswu" /I "$(WXWIN)\include" /O2 /Fd..\..\usbpicprog.pdb /I "..\..\src" /I "..\..\win\deps" /D "WIN32" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_UNICODE" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswu" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i ..\..\win\deps
# ADD RSC /l 0x409 /d "_UNICODE" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswu" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i ..\..\win\deps
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb-1.0.lib setupapi.lib wxmsw29u_adv.lib wxbase29u_xml.lib wxmsw29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"..\..\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /pdb:"..\..\usbpicprog.pdb" /libpath:"..\..\win\deps\release\x86"
# ADD LINK32 libusb-1.0.lib setupapi.lib wxmsw29u_adv.lib wxbase29u_xml.lib wxmsw29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"..\..\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /pdb:"..\..\usbpicprog.pdb" /libpath:"..\..\win\deps\release\x86"

!ENDIF

# Begin Target

# Name "upp_wx - Win32 DLL Debug"
# Name "upp_wx - Win32 DLL Release"
# Name "upp_wx - Win32 Static Debug"
# Name "upp_wx - Win32 Static Release"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\configview.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hardware.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hexfile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\hexview.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\io_test.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\io_test_base.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\packageview.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\pictype.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\preferences.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\preferences_base.cpp
# End Source File
# Begin Source File

SOURCE=..\..\icons\win\upp_wx.rc
# End Source File
# Begin Source File

SOURCE=..\..\src\uppmainwindow.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\uppmainwindow_base.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\configview.h
# End Source File
# Begin Source File

SOURCE=..\..\src\formbuilder_includes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\hardware.h
# End Source File
# Begin Source File

SOURCE=..\..\src\hexfile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\hexview.h
# End Source File
# Begin Source File

SOURCE=..\..\src\io_test.h
# End Source File
# Begin Source File

SOURCE=..\..\src\io_test_base.h
# End Source File
# Begin Source File

SOURCE=..\..\src\main.h
# End Source File
# Begin Source File

SOURCE=..\..\src\packageview.h
# End Source File
# Begin Source File

SOURCE=..\..\src\pictype.h
# End Source File
# Begin Source File

SOURCE=..\..\src\preferences.h
# End Source File
# Begin Source File

SOURCE=..\..\src\preferences_base.h
# End Source File
# Begin Source File

SOURCE=..\..\src\uppmainwindow.h
# End Source File
# Begin Source File

SOURCE=..\..\src\uppmainwindow_base.h
# End Source File
# End Group
# End Target
# End Project

