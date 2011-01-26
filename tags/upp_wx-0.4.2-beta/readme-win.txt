***************************************************************************************************
Compiling and releasing Usbpicprog for Windows
By Francesco Montorsi
***************************************************************************************************


1. Compiling
   You can compile upp_wx with a variety of tools: command-like make programs (see build\win\makefile*),
   MSVC project files (see build\win\*sln) and wxDevCpp (see build\win\upp_wx_devcpp.dev).
   
   In the "win\deps" folder you can find the precompiled binaries of libusb (a dependency of upp_wx). 
   Note that you'll also need to compile wxWidgets before you can start compiling upp_wx 
   (since it's another dependency - although it's not provided in "win\deps" given its size).
   
   Once you're done with all the dependencies, you can compile upp_wx.
   It supports both 32bit and 64bit build modes (the 64bit version is needed because in order to be 
   able to use an USB device under 64bit versions of MS Windows you will need a 64bit application!).
   
   The easiest way to compile upp_wx is probably to download and install the MSVC++ Express Edition
   (which is provided for free by Microsoft) and use the MSVC project files.
   Note however that the Express Edition is currently only able to build 32bit binaries; you will need
   the Professional edition to build 64bit binaries of upp_wx.

2. Releasing
   All infos for releasing upp_wx under Windows are provided in the "win\installer\readme.txt" file.
   Refer to that file for more info.

Good luck!