Steps for releasing UsbPicProg on Windows
-----------------------------------------

1) Download and install NSIS.
2) Copy GetVersion.dll in your NSIS\Plugins folder
3) Compile UsbPicProg as a 32bit application and copy the final .exe file in the "x86" folder.
4) Compile UsbPicProg as a 64bit application and copy the final .exe file in the "amd64" folder.
5) Set ARCH=x86 in setup.nsi and compile it with NSIS; set ARCH=amd64 in setup.nsi and compile it with NSIS.
   Note that you can also use the build_installers.bat batch file to do this step automatically.
7) Upload the two installers on the release file server.
   They should install and work fine on both 32bit and 64bit versions of Windows XP, Windows Vista,
   Windows Server 2003, Windows 7 (and higher).

