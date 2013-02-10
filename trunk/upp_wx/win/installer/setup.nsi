; *************************************************************************
;   This is the NSIS (http://nsis.sf.net) installer of UsbPicProg         *
;   Copyright (C) 2008-2010 by Frans Schreuder, Francesco Montorsi        *
;   usbpicprog.sourceforge.net                                            *
;                                                                         *
;   This program is free software; you can redistribute it and/or modify  *
;   it under the terms of the GNU General Public License as published by  *
;   the Free Software Foundation; either version 2 of the License, or     *
;   (at your option) any later version.                                   *
;                                                                         *
;   This program is distributed in the hope that it will be useful,       *
;   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
;   GNU General Public License for more details.                          *
;                                                                         *
;   You should have received a copy of the GNU General Public License     *
;   along with this program; if not, write to the                         *
;   Free Software Foundation, Inc.,                                       *
;   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
; *************************************************************************

; -------------------------------------------------------------------------------------------------
; Include Modern UI

  !include "MUI.nsh"

; -------------------------------------------------------------------------------------------------
; General

  ; NOTE: the version should be the same as the one in svn_revision.sh (see the UPP_VERSION #define)
  !define PRODUCT_NAME            "UsbPicProg"
  !define PRODUCT_VERSION         "0.5.0"
  !define PRODUCT_PUBLISHER       "UsbPicProg Team"
  !define INSTALLER_MODE          "release"     ; choose between "debug" and "release"
  !define SVN_TEST_INSTALLER      0           ; is this a SVN test?
  !define SVN_REVISION            "121018"

  ; are we packaging the 32bit or the 64bit version of the usbpicprog?
  ; allowed values: "x86" or "amd64"
  !ifndef ARCH                     ; see build_installers.bat
    !define ARCH                  "amd64"
  !else
    !if "${ARCH}" != "amd64" 
      !if "${ARCH}" != "x86"
        !error "Invalid value for the ARCH define"
      !endif
    !endif
  !endif

  ; Name and file
  Name "${PRODUCT_NAME} ${PRODUCT_VERSION} ${ARCH} Installer"
  !if ${SVN_TEST_INSTALLER}
    OutFile "${PRODUCT_NAME}-${ARCH}-r${SVN_REVISION}-${INSTALLER_MODE}.exe"
  !else
    OutFile "${PRODUCT_NAME}-${ARCH}-${PRODUCT_VERSION}.exe"
  !endif
  Icon "..\..\usbpicprog.ico"

  ; Default installation folder
  !if "${ARCH}" == "amd64"
    InstallDir "$PROGRAMFILES64\${PRODUCT_NAME}"
  !else
    InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
  !endif
  
  LicenseData "gnugpl.txt"
  SetCompressor /SOLID lzma    ; this was found to be the best compressor
  
  ; see http://nsis.sourceforge.net/Shortcuts_removal_fails_on_Windows_Vista for more info:
  RequestExecutionLevel admin
  
; -------------------------------------------------------------------------------------------------
; Pages

  !insertmacro MUI_PAGE_LICENSE "gnugpl.txt"
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
; -------------------------------------------------------------------------------------------------
; Interface Settings

  !define MUI_ABORTWARNING
  
; -------------------------------------------------------------------------------------------------
; Languages
 
  !insertmacro MUI_LANGUAGE "English"

  !macro UPP_INSTALL_LANG langID
    SetOutPath "$INSTDIR\po\${langID}\LC_MESSAGES"
    File /oname=usbpicprog.mo ..\..\po\${langID}.mo
  !macroend
  
; -------------------------------------------------------------------------------------------------
; Additional info (will appear in the "details" tab of the properties window for the installer)

  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName"      "${PRODUCT_NAME}"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments"         ""
  VIAddVersionKey /LANG=${LANG_ENGLISH} "CompanyName"      "${PRODUCT_NAME} Team"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalTrademarks"  "Application released under the GNU GPL"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright"   "© ${PRODUCT_NAME} Team"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription"  "USB PIC Programmer"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion"      "${PRODUCT_VERSION}"
  VIProductVersion                                         "${PRODUCT_VERSION}.0" 

; -------------------------------------------------------------------------------------------------
; Installer Sections

Section "" ; No components page, name is not important

  ; check if the architecture of the currently-running system is OK
  
  GetVersion::WindowsPlatformArchitecture
  Pop $R0
  DetailPrint "Detected a $R0 bit Windows architecture"
  !if "${ARCH}" == "amd64"
    StrCmp "$R0" "64" proceed
    MessageBox MB_OK|MB_ICONEXCLAMATION "This installer is for 64bit Windows versions but has detected a 32bit operating system! Please download the X86 version of this installer."
    Quit
  !else
    StrCmp "$R0" "32" proceed
    MessageBox MB_OK|MB_ICONEXCLAMATION "This installer is for 32bit Windows versions but has detected a 64bit operating system! Please download the AMD64 version of this installer."
    Quit
  !endif
  
proceed:

  ; Set files to be extracted in the user-chosen installation path:

  SetOutPath "$INSTDIR"
  File gnugpl.txt
  File ..\..\index.xml
  File ..\..\usbpicprog.ico    ; used by the DPINST utility
  File ${ARCH}\usbpicprog.exe
  File /nonfatal ${ARCH}\usbpicprog.exe.manifest        ; in case the manifest was not embedded in the .exe file, use /nonfatal
  File ..\..\win\deps\${INSTALLER_MODE}\${ARCH}\*.exe
  File ..\..\win\deps\${INSTALLER_MODE}\${ARCH}\*.dll
  ;File ..\..\win\deps\${INSTALLER_MODE}\${ARCH}\*.manifest
    ; CRT manifests always need to be copied to allow installations on WinXP systems
  
  ; Install the *.mo files
  !insertmacro UPP_INSTALL_LANG ar
  !insertmacro UPP_INSTALL_LANG bs
  !insertmacro UPP_INSTALL_LANG cs
  !insertmacro UPP_INSTALL_LANG de
  !insertmacro UPP_INSTALL_LANG el
  !insertmacro UPP_INSTALL_LANG es
  !insertmacro UPP_INSTALL_LANG fr
  !insertmacro UPP_INSTALL_LANG he
  !insertmacro UPP_INSTALL_LANG hu
  !insertmacro UPP_INSTALL_LANG it
  !insertmacro UPP_INSTALL_LANG ja
  !insertmacro UPP_INSTALL_LANG ms
  !insertmacro UPP_INSTALL_LANG nl
  !insertmacro UPP_INSTALL_LANG pa
  !insertmacro UPP_INSTALL_LANG pt
  !insertmacro UPP_INSTALL_LANG ro
  !insertmacro UPP_INSTALL_LANG ru
  !insertmacro UPP_INSTALL_LANG tr
  !insertmacro UPP_INSTALL_LANG uk
  
  SetOutPath "$INSTDIR"
  File /r /x .svn ..\..\examples
  
  SetOutPath "$INSTDIR"
  File ..\..\help\usbpicprog.htb

  SetOutPath "$INSTDIR"
  File ..\..\firmware.hex
  
  SetOutPath "$INSTDIR\xml_data"
  File ..\..\xml_data\*.xml

  SetOutPath "$INSTDIR\driver"
  ;File ..\..\win\driver\*.inf
  File ..\..\win\driver\wdi-simple.exe
  ;File ..\..\win\driver\dpinst.xml

  ;SetOutPath "$INSTDIR\driver\${ARCH}"
  ;File ..\..\win\driver\${ARCH}\*.dll
  
  ; Create uninstaller
  WriteUninstaller "$INSTDIR\uninstall.exe"
  
  ; Select correct registry view before using WriteRegStr
  !if "${ARCH}" == "amd64"
    SetRegView 64
  !else
    SetRegView 32
  !endif
  
  ; Add the uninstaller to the list of programs accessible from "Control Panel -> Programs and Features"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
                   "DisplayName" "${PRODUCT_NAME}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
                   "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
                   "DisplayIcon" "$\"$INSTDIR\usbpicprog.ico$\""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
                   "URLInfoAbout" "http://usbpicprog.org"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
                   "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
                   "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}" \
                     "EstimatedSize" 13312
                     ; the estimated must be expressed in Kb; for us it's about 13 MB!

  ; Create shortcuts
  SetShellVarContext all        ; see http://nsis.sourceforge.net/Shortcuts_removal_fails_on_Windows_Vista
  SetOutPath "$INSTDIR"         ; this will be the working directory for the shortcuts created below
  CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\UsbPicProg.lnk" "$INSTDIR\usbpicprog.exe"
  CreateShortCut "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  
  ; Last, run the libusb driver installer redistributable
  DetailPrint "Running the wdi_simple utility to install UsbPicProg's drivers"
  ExecWait '"$INSTDIR\driver\wdi-simple.exe -n usbpicprog.org -t 0 -p 0x04d8 -v 0x000e"' $0
  IntFmt $2 "0x%08X" $0
  DetailPrint "Return code was $2"
   
   
  ; check the higher byte of the return value of DPINST; it can assume the values:
  ; 0x80 if a driver package could NOT be installed
  ; 0x40 if a computer restart is necessary
  ; 0x00 if everything was ok
  ; or a combination of them (the only possible one in this case is 0xC0)
  ; see http://msdn.microsoft.com/en-us/library/ms791066.aspx for more info
  IntOp $1 $0 / 0x1000000                  ; fast way to keep only the higher byte
  IntFmt $2 "0x%X" $1
  DetailPrint "The higher byte of the return code was $2"
  IntCmp $1 0x00 installed_ok
  IntCmp $1 0x40 installed_ok_need_reboot
  IntCmp $1 0x80 install_failed
  IntCmp $1 0xC0 install_failed
  
; Last, run the libusb driver installer redistributable
  DetailPrint "Running the wdi_simple utility to install UsbPicProg bootloader drivers"
  ExecWait '"$INSTDIR\driver\wdi-simple.exe -n usbpicprog.org -t 0 -p 0x04d8 -v 0x000b"' $0
  IntFmt $2 "0x%08X" $0
  DetailPrint "Return code was $2"
   
   
  ; check the higher byte of the return value of DPINST; it can assume the values:
  ; 0x80 if a driver package could NOT be installed
  ; 0x40 if a computer restart is necessary
  ; 0x00 if everything was ok
  ; or a combination of them (the only possible one in this case is 0xC0)
  ; see http://msdn.microsoft.com/en-us/library/ms791066.aspx for more info
  IntOp $1 $0 / 0x1000000                  ; fast way to keep only the higher byte
  IntFmt $2 "0x%X" $1
  DetailPrint "The higher byte of the return code was $2"
  IntCmp $1 0x00 installed_ok
  IntCmp $1 0x40 installed_ok_need_reboot
  IntCmp $1 0x80 install_failed
  IntCmp $1 0xC0 install_failed
  
  ; unhandled return code ?!?
  DetailPrint "Unknown return value of the DPINST utility! Check %SYSTEMROOT%\DPINST.LOG for more info."
  MessageBox MB_OK|MB_ICONEXCLAMATION "Unknown return value of the DPINST utility! Check %SYSTEMROOT%\DPINST.LOG for more info."
  Abort "Couldn't install UsbPicProg's drivers! Check %SYSTEMROOT%\DPINST.LOG for more info."
  
installed_ok_need_reboot:
  DetailPrint "Drivers were installed successfully but require a reboot"
  MessageBox MB_YESNO|MB_ICONQUESTION "The driver installation finished but requires a system reboot. Do you wish to reboot now?" IDNO +2
  Reboot
  Goto installed_ok
install_failed:
  DetailPrint "Drivers could not be installed! Check %SYSTEMROOT%\DPINST.LOG for more info."
  MessageBox MB_OK|MB_ICONEXCLAMATION "Couldn't install UsbPicProg's drivers! Check %SYSTEMROOT%\DPINST.LOG for more info."
  Abort "Couldn't install UsbPicProg's drivers! Check %SYSTEMROOT%\DPINST.LOG for more info."
installed_ok:
  ; do nothing
  DetailPrint "Drivers were installed successfully."

; Last, run the Microsoft driver installer redistributable
  DetailPrint "Installing the VC redistributable files"
  ExecWait '"$INSTDIR\vcredist_${ARCH}"' $0
  IntFmt $2 "0x%08X" $0
  DetailPrint "Return code was $2"
  
SectionEnd

; -------------------------------------------------------------------------------------------------
; Uninstaller Section

Section "Uninstall"

  ; note that uninstalling the drivers installed by dpinst.exe is not easy 
  ; and currently it seems to be supported only through the "Add Programs and Features"
  ; panel of Control panel (see http://msdn.microsoft.com/en-us/library/ms791069.aspx),
  ; which we disable in dpinst.xml!
  
  ; Select correct registry view before using DeleteRegKey
  !if "${ARCH}" == "amd64"
    SetRegView 64
  !else
    SetRegView 32
  !endif
  
  ; clean the list accessible from "Control Panel -> Programs and Features"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

  ; clean start menu
  SetShellVarContext all
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\Uninstall.lnk"
  Delete "$SMPROGRAMS\${PRODUCT_NAME}\UsbPicProg.lnk"
  RMDir "$SMPROGRAMS\${PRODUCT_NAME}"

  ; clean installation folder
  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\gnugpl.txt"
  Delete "$INSTDIR\index.xml"
  Delete "$INSTDIR\usbpicprog.ico"
  Delete "$INSTDIR\usbpicprog.exe"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\*.manifest"
  ;Delete "$INSTDIR\po\*\LC_MESSAGES\usbpicprog.mo"    ; doesn't seem to work unfortunately; we'll use /r later then
  Delete "$INSTDIR\xml_data\*.xml"
  Delete "$INSTDIR\driver\*.inf"
  Delete "$INSTDIR\driver\dpinst.exe"
  Delete "$INSTDIR\driver\dpinst.xml"
  Delete "$INSTDIR\driver\${ARCH}\*.dll"
  RMDir /r "$INSTDIR\po"
  RMDir "$INSTDIR\xml_data"
  RMDir "$INSTDIR\driver\${ARCH}"
  RMDir "$INSTDIR\driver"
  RMDir "$INSTDIR"

SectionEnd

