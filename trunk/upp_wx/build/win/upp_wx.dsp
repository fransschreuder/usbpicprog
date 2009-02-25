# Microsoft Developer Studio Project File - Name="upp_wx" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=upp_wx - Win32 Static ANSI Release Multilib
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "upp_wx.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "upp_wx.mak" CFG="upp_wx - Win32 Static ANSI Release Multilib"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "upp_wx - Win32 DLL Unicode Debug Monolithic" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 DLL Unicode Debug Multilib" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 DLL Unicode Release Monolithic" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 DLL Unicode Release Multilib" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 DLL ANSI Debug Monolithic" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 DLL ANSI Debug Multilib" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 DLL ANSI Release Monolithic" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 DLL ANSI Release Multilib" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 Static Unicode Debug Monolithic" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 Static Unicode Debug Multilib" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 Static Unicode Release Monolithic" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 Static Unicode Release Multilib" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 Static ANSI Debug Monolithic" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 Static ANSI Debug Multilib" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 Static ANSI Release Monolithic" (based on "Win32 (x86) Application")
!MESSAGE "upp_wx - Win32 Static ANSI Release Multilib" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "upp_wx - Win32 DLL Unicode Debug Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vcmswud_dll"
# PROP BASE Intermediate_Dir "vcmswud_dll\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vcmswud_dll"
# PROP Intermediate_Dir "vcmswud_dll\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswud" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswud_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswud" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswud_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswud" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswud" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswud_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /debug /pdb:"vcmswud_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswud_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /debug /pdb:"vcmswud_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 DLL Unicode Debug Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vcmswud_dll"
# PROP BASE Intermediate_Dir "vcmswud_dll\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vcmswud_dll"
# PROP Intermediate_Dir "vcmswud_dll\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswud" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswud_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswud" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswud_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswud" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswud" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29ud_adv.lib wxmsw29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswud_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /debug /pdb:"vcmswud_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29ud_adv.lib wxmsw29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswud_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /debug /pdb:"vcmswud_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 DLL Unicode Release Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vcmswu_dll"
# PROP BASE Intermediate_Dir "vcmswu_dll\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vcmswu_dll"
# PROP Intermediate_Dir "vcmswu_dll\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswu" /I "$(WXWIN)\include" /O2 /Fdvcmswu_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswu" /I "$(WXWIN)\include" /O2 /Fdvcmswu_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "_UNICODE" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswu" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "_UNICODE" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswu" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswu_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /pdb:"vcmswu_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswu_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /pdb:"vcmswu_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 DLL Unicode Release Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vcmswu_dll"
# PROP BASE Intermediate_Dir "vcmswu_dll\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vcmswu_dll"
# PROP Intermediate_Dir "vcmswu_dll\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswu" /I "$(WXWIN)\include" /O2 /Fdvcmswu_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswu" /I "$(WXWIN)\include" /O2 /Fdvcmswu_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "_UNICODE" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswu" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "_UNICODE" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswu" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29u_adv.lib wxmsw29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswu_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /pdb:"vcmswu_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29u_adv.lib wxmsw29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswu_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /pdb:"vcmswu_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 DLL ANSI Debug Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vcmswd_dll"
# PROP BASE Intermediate_Dir "vcmswd_dll\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vcmswd_dll"
# PROP Intermediate_Dir "vcmswd_dll\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswd_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswd_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswd_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /debug /pdb:"vcmswd_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswd_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /debug /pdb:"vcmswd_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 DLL ANSI Debug Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vcmswd_dll"
# PROP BASE Intermediate_Dir "vcmswd_dll\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vcmswd_dll"
# PROP Intermediate_Dir "vcmswd_dll\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswd_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\mswd" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswd_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\mswd" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29d_adv.lib wxmsw29d_core.lib wxbase29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswd_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /debug /pdb:"vcmswd_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29d_adv.lib wxmsw29d_core.lib wxbase29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswd_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /debug /pdb:"vcmswd_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 DLL ANSI Release Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vcmsw_dll"
# PROP BASE Intermediate_Dir "vcmsw_dll\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vcmsw_dll"
# PROP Intermediate_Dir "vcmsw_dll\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /O2 /Fdvcmsw_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /O2 /Fdvcmsw_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmsw_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /pdb:"vcmsw_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmsw_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /pdb:"vcmsw_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 DLL ANSI Release Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vcmsw_dll"
# PROP BASE Intermediate_Dir "vcmsw_dll\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vcmsw_dll"
# PROP Intermediate_Dir "vcmsw_dll\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /O2 /Fdvcmsw_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_dll\msw" /I "$(WXWIN)\include" /O2 /Fdvcmsw_dll\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "WXUSINGDLL" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "WXUSINGDLL" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "WXUSINGDLL" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "WXUSINGDLL" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_dll\msw" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29_adv.lib wxmsw29_core.lib wxbase29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmsw_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /pdb:"vcmsw_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29_adv.lib wxmsw29_core.lib wxbase29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmsw_dll\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_dll" /subsystem:windows /pdb:"vcmsw_dll\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 Static Unicode Debug Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vcmswud"
# PROP BASE Intermediate_Dir "vcmswud\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vcmswud"
# PROP Intermediate_Dir "vcmswud\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswud" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswud\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswud" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswud\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswud" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswud" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswud\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /debug /pdb:"vcmswud\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswud\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /debug /pdb:"vcmswud\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 Static Unicode Debug Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vcmswud"
# PROP BASE Intermediate_Dir "vcmswud\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vcmswud"
# PROP Intermediate_Dir "vcmswud\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswud" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswud\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswud" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswud\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_UNICODE" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswud" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "_UNICODE" /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswud" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29ud_adv.lib wxmsw29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswud\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /debug /pdb:"vcmswud\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29ud_adv.lib wxmsw29ud_core.lib wxbase29ud.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexud.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswud\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /debug /pdb:"vcmswud\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 Static Unicode Release Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vcmswu"
# PROP BASE Intermediate_Dir "vcmswu\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vcmswu"
# PROP Intermediate_Dir "vcmswu\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswu" /I "$(WXWIN)\include" /O2 /Fdvcmswu\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswu" /I "$(WXWIN)\include" /O2 /Fdvcmswu\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_UNICODE" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswu" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "_UNICODE" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswu" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswu\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /pdb:"vcmswu\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswu\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /pdb:"vcmswu\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 Static Unicode Release Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vcmswu"
# PROP BASE Intermediate_Dir "vcmswu\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vcmswu"
# PROP Intermediate_Dir "vcmswu\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswu" /I "$(WXWIN)\include" /O2 /Fdvcmswu\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswu" /I "$(WXWIN)\include" /O2 /Fdvcmswu\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "_UNICODE" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_UNICODE" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswu" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "_UNICODE" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswu" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29u_adv.lib wxmsw29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswu\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /pdb:"vcmswu\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29u_adv.lib wxmsw29u_core.lib wxbase29u.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregexu.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswu\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /pdb:"vcmswu\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 Static ANSI Debug Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vcmswd"
# PROP BASE Intermediate_Dir "vcmswd\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vcmswd"
# PROP Intermediate_Dir "vcmswd\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswd" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswd\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswd" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswd\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswd" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswd" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswd\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /debug /pdb:"vcmswd\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswd\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /debug /pdb:"vcmswd\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 Static ANSI Debug Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "vcmswd"
# PROP BASE Intermediate_Dir "vcmswd\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "vcmswd"
# PROP Intermediate_Dir "vcmswd\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswd" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswd\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD CPP /nologo /FD /MDd /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\mswd" /I "$(WXWIN)\include" /Od /Gm /Zi /Fdvcmswd\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXDEBUG__" /D "__WXMSW__" /D "_WINDOWS" /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswd" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "__WXDEBUG__" /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\mswd" /i "$(WXWIN)\include" /d "_WINDOWS" /d "_DEBUG" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29d_adv.lib wxmsw29d_core.lib wxbase29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswd\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /debug /pdb:"vcmswd\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29d_adv.lib wxmsw29d_core.lib wxbase29d.lib wxtiffd.lib wxjpegd.lib wxpngd.lib wxzlibd.lib wxregexd.lib wxexpatd.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmswd\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /debug /pdb:"vcmswd\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 Static ANSI Release Monolithic"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vcmsw"
# PROP BASE Intermediate_Dir "vcmsw\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vcmsw"
# PROP Intermediate_Dir "vcmsw\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\msw" /I "$(WXWIN)\include" /O2 /Fdvcmsw\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\msw" /I "$(WXWIN)\include" /O2 /Fdvcmsw\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\msw" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\msw" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmsw\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /pdb:"vcmsw\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmsw\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /pdb:"vcmsw\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ELSEIF  "$(CFG)" == "upp_wx - Win32 Static ANSI Release Multilib"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "vcmsw"
# PROP BASE Intermediate_Dir "vcmsw\upp_wx"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "vcmsw"
# PROP Intermediate_Dir "vcmsw\upp_wx"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\msw" /I "$(WXWIN)\include" /O2 /Fdvcmsw\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD CPP /nologo /FD /MD /W1 /GR /EHsc /I "$(WXWIN)\lib\vc_lib\msw" /I "$(WXWIN)\include" /O2 /Fdvcmsw\usbpicprog.pdb /I "..\..\src" /I "$(LIBUSB)\src" /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /c
# ADD BASE MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD MTL /nologo /D "WIN32" /D "__WXMSW__" /D "_WINDOWS" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\msw" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
# ADD RSC /l 0x409 /d "__WXMSW__" /i "$(WXWIN)\lib\vc_lib\msw" /i "$(WXWIN)\include" /d "_WINDOWS" /i "..\..\src" /i $(LIBUSB)\src
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libusb.lib wxmsw29_adv.lib wxmsw29_core.lib wxbase29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmsw\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /pdb:"vcmsw\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"
# ADD LINK32 libusb.lib wxmsw29_adv.lib wxmsw29_core.lib wxbase29.lib wxtiff.lib wxjpeg.lib wxpng.lib wxzlib.lib wxregex.lib wxexpat.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib winspool.lib winmm.lib shell32.lib comctl32.lib ole32.lib oleaut32.lib uuid.lib rpcrt4.lib advapi32.lib wsock32.lib oleacc.lib /nologo /machine:i386 /out:"vcmsw\usbpicprog.exe" /libpath:"$(WXWIN)\lib\vc_lib" /subsystem:windows /pdb:"vcmsw\usbpicprog.pdb" /libpath:"$(LIBUSB)\lib\msvc"

!ENDIF

# Begin Target

# Name "upp_wx - Win32 DLL Unicode Debug Monolithic"
# Name "upp_wx - Win32 DLL Unicode Debug Multilib"
# Name "upp_wx - Win32 DLL Unicode Release Monolithic"
# Name "upp_wx - Win32 DLL Unicode Release Multilib"
# Name "upp_wx - Win32 DLL ANSI Debug Monolithic"
# Name "upp_wx - Win32 DLL ANSI Debug Multilib"
# Name "upp_wx - Win32 DLL ANSI Release Monolithic"
# Name "upp_wx - Win32 DLL ANSI Release Multilib"
# Name "upp_wx - Win32 Static Unicode Debug Monolithic"
# Name "upp_wx - Win32 Static Unicode Debug Multilib"
# Name "upp_wx - Win32 Static Unicode Release Monolithic"
# Name "upp_wx - Win32 Static Unicode Release Multilib"
# Name "upp_wx - Win32 Static ANSI Debug Monolithic"
# Name "upp_wx - Win32 Static ANSI Debug Multilib"
# Name "upp_wx - Win32 Static ANSI Release Monolithic"
# Name "upp_wx - Win32 Static ANSI Release Multilib"
# Begin Group "Source Files"

# PROP Default_Filter ""
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

SOURCE=..\..\src\main.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\pictype.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\preferences.cpp
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

SOURCE=..\..\src\main.h
# End Source File
# Begin Source File

SOURCE=..\..\src\pictype.h
# End Source File
# Begin Source File

SOURCE=..\..\src\preferences.h
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

