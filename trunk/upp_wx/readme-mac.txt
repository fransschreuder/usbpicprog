***************************************************************************************************
Compiling and releasing Usbpicprog for Mac OS X
By Jan Paul Posma
***************************************************************************************************


1. Compiling
   First time compiling for the mac is quite easy!
   Open a terminal and check if wxWidgets and libusb are installed by entering:
      wx-config --version
   and
      libusb-config --version

   If both are present you can continue by entering (in the upp_wx directory):
      ./buildmac.sh

   This script will cleanup files, do a configure, a make and copy everything into src/usbpicprog.app
   You can now start the application either by double-clicking on usbpicprog.app in the src folder, or 
   by entering on the terminal: 
      open src/usbpicprog.app

   *** WARNING ***
   This application will ONLY work on your computer and is NOT ready for releasing! 
   Please check the next chapter on releasing!


2. Releasing
   The .app you made is not yet ready for release because it depends on libraries that are most likely 
   not available or incompatible when you release it to end-users. First of all, libusb is not shipped 
   by default on OS X, and wxWidgets is actually shipped by default, but it's a debug version.

   So what we want to do is include these libraries in the .app. First make sure that all the libraries 
   are up-to-date. The most recent version of libusb is the SVN version, so checkout from the libusb SVN, 
   do a:
     ./configure && make && make install 
   and test if usbpicprog still works.
   Download the most recent wxMac from the wxWidgets website and check this wiki page on how to compile: 
     http://wiki.wxwidgets.org/Mac_OS_X_And_Xcode_For_Beginners#Configure
   You want a many libs built-in as possible. The configure command I used for wxWidgets is 
     ../configure --with-macosx-version-min=10.4 --with-macosx-sdk=/Developer/SDKs/MacOSX10.4u.sdk/ --enable-monolithic --with-libjpeg=builtin --with-libpng=builtin --with-regex=builtin --with-libtiff=builtin --with-zlib=builtin --with-expat=builtin
   
   Now run the ./buildmac.sh script again and check if usbpicprog is still working!

   If all is working fine, we are going to include the libs in the usbpicprog.app! In order to do so, 
   you want to check which libraries usbpicprog is dependent on. Run this command in the terminal:
     otool -L src/usbpicprog

   This will give a list of all the dependent libraries. Anything in /System and /usr/lib will most likely 
   be on the user system, but for example /usr/local/lib not, and /sw/lib also not. 
   Copy these libraries to the libs directory (upp_wx/libs). Make sure you copy the 'real' library, 
   not the symbolic link but rename it afterwards. The script will be looking for the following files:
     - libs/libusb.dylib
     - libs/libwx.dylib

   Now run ./buildmac.sh again and check if usbpicprog still works. 
   Then uninstall the libraries (go to the library source directory and do a 'make uninstall'), and check 
   if usbpicprog works. If it works, the libraries are successfully included in the .app. 
   Another indication is that the usbpicprog.app is a lot bigger now (>1MB).

Good luck!