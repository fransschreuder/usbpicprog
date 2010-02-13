Steps for releasing UsbPicProg on Windows
-----------------------------------------

1) Download and install NSIS.
2) Copy GetVersion.dll in your NSIS\Plugins folder
3) Compile UsbPicProg as a 32bit application and copy the final .exe file in the "x86" folder.
4) Compile UsbPicProg as a 64bit application and copy the final .exe file in the "amd64" folder.
5) Set ARCH=x86 in setup.nsi and compile it with NSIS.
6) Set ARCH=amd64 in setup.nsi and compile it with NSIS.
7) Upload the two installers on the release file server.


