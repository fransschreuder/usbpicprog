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

; ---------------------
; Include Modern UI

  !include "MUI.nsh"

; --------------------------------
; General

  ; NOTE: the version should be the same as the one in svn_revision.sh (see the UPP_VERSION #define)
  !define PRODUCT_NAME            "UsbPicProg"
  !define PRODUCT_VERSION         "0.3.0"
  !define PRODUCT_PUBLISHER       "UsbPicProg Team"

  ; are we packaging the 32bit or the 64bit version of the usbpicprog?
  ; allowed values: "x86" or "amd64"
  !define ARCH                    "x86"

  ; Name and file
  Name "UsbPicProg ${PRODUCT_VERSION} ${ARCH} Installer"
  OutFile "${PRODUCT_NAME}-${ARCH}-${PRODUCT_VERSION}.exe"
  Icon "..\..\usbpicprog.ico"

  ; Default installation folder
  InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
  
  ; Vista redirects $SMPROGRAMS to all users without this
  ;RequestExecutionLevel admin
  
  LicenseData "gnugpl.txt"

; --------------------------------
; Pages

  !insertmacro MUI_PAGE_LICENSE "gnugpl.txt"
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
; --------------------------------
; Interface Settings

  !define MUI_ABORTWARNING
  
; --------------------------------
; Languages
 
  !insertmacro MUI_LANGUAGE "English"

; --------------------------------
; Installer Sections

Section "" ; No components page, name is not important

  ; Set files to be extracted in the user-chosen installation path:

  SetOutPath "$INSTDIR"
  File gnugpl.txt
  File ..\..\index.xml
  File ${ARCH}\usbpicprog.exe
  File ..\..\win\deps\${ARCH}\*.dll
  
  SetOutPath "$INSTDIR\xml_data"
  File ..\..\xml_data\*.xml

  SetOutPath "$INSTDIR\driver"
  File ..\..\win\driver\*.inf
  File ..\..\win\driver\dpinst.exe
  File ..\..\win\driver\dpinst.xml

  SetOutPath "$INSTDIR\driver\amd64"
  File ..\..\win\driver\amd64\*.dll

  SetOutPath "$INSTDIR\driver\x86"
  File ..\..\win\driver\x86\*.dll

  ; Store installation folder
  ;WriteRegStr HKLM "Software\UsbPicProg" "Path" "$INSTDIR"
  
  ; Create uninstaller
  WriteUninstaller "$INSTDIR\uninstall.exe"
  
  ; run the Microsoft driver installer redistributable
  ExecWait '"$INSTDIR\driver\dpinst.exe"' $0
   
  ; check the higher byte of the return value of DPINST; it can assume the values:
  ; 0x80 if a driver package could NOT be installed
  ; 0x40 if a computer restart is necessary
  ; 0x00 if everything was ok
  ; or a combination of them (the only possible one in this case is 0xC0)
  ; see http://msdn.microsoft.com/en-us/library/ms791066.aspx for more info
  IntOp $1 $0 & 0xFF000000
  IntCmp $1 0x00 installed_ok
  IntCmp $1 0x40 installed_ok_need_reboot
  IntCmp $1 0x80 install_failed
  IntCmp $1 0xC0 install_failed
installed_ok_need_reboot:
  ;MessageBox MB_YESNO|MB_ICONQUESTION "The driver installation Do you wish to reboot the system?" IDNO +2
  ;Reboot
  Goto installed_ok
install_failed:
  Abort "Couldn't install the programmer's drivers"
installed_ok:

  ; Create shortcuts
  CreateDirectory "$SMPROGRAMS\UsbPicProg"
  CreateShortCut "$SMPROGRAMS\UsbPicProg\UsbPicProg.lnk" "$INSTDIR\usbpicprog.exe"
  CreateShortCut "$SMPROGRAMS\UsbPicProg\Uninstall.lnk" "$INSTDIR\uninstall.exe"

SectionEnd

; --------------------------------
; Uninstaller Section

Section "Uninstall"

  ; clean installation folder

  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\gnugpl.txt"
  Delete "$INSTDIR\index.xml"
  Delete "$INSTDIR\usbpicprog.exe"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\xml_data\*.xml"
  Delete "$INSTDIR\driver\*.inf"
  Delete "$INSTDIR\driver\dpinst.exe"
  Delete "$INSTDIR\driver\dpinst.xml"
  Delete "$INSTDIR\driver\amd64\*.dll"
  Delete "$INSTDIR\driver\x86\*.dll"
  RMDir "$INSTDIR\xml_data"
  RMDir "$INSTDIR\driver\amd64"
  RMDir "$INSTDIR\driver\x86"
  RMDir "$INSTDIR\driver"
  RMDir "$INSTDIR"

  ; clean start menu

  Delete "$SMPROGRAMS\UsbPicProg\Uninstall.lnk"
  Delete "$SMPROGRAMS\UsbPicProg\UsbPicProg.lnk"
  RMDir "$SMPROGRAMS\UsbPicProg"
  
  ;DeleteRegKey /ifempty HKLM "Software\EmfPrinter"

SectionEnd

