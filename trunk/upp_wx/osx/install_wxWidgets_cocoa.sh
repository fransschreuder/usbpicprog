#!/opt/local/bin/bash
# This file install wxWidgets 2.9.0 for both ppc and i386
# This will allow the building of usbpicprog as a universal application.
#
# All build files assumes that MacPorts is installed. You can change
# settings below if necessary.
#
# Created by Gustav Johansson 2010-03-11
#

# Load the settings file. Change anything you want in that file ONLY!
source settings

# Create all directories needed.
if [ -d "$PREFIX_ppc_cocoa" ]; then
	echo "$PREFIX_ppc_cocoa exists!"
else 	
	echo "Creating $PREFIX_ppc_cocoa"
	mkdir -p "$PREFIX_ppc_cocoa"
fi
if [ -d "$PREFIX_i386_cocoa" ]; then
	echo "$PREFIX_i386_cocoa exists!"
else 	
	echo "Creating $PREFIX_i386_cocoa"
	mkdir -p "$PREFIX_i386_cocoa"
fi
if [ -d "$PREFIX_x86_64_cocoa" ]; then
	echo "$PREFIX_x86_64_cocoa exists!"
else 	
	echo "Creating $PREFIX_x86_64_cocoa"
	mkdir -p "$PREFIX_x86_64_cocoa"
fi
if [ -d "$PREFIX_app" ]; then
	echo "$PREFIX_app exists!"
else 	
	echo "Creating $PREFIX_app"
	mkdir -p "$PREFIX_app"
fi
if [ -d "$WORKDIR" ]; then
	echo "$WORKDIR exists!"
else 	
	echo "Creating $WORKDIR"
	mkdir -p "$WORKDIR"
fi

# Download wxWidgets
cd "$WORKDIR"
# Get in there and create a builddir
cd wxWidgets
svn up

rm -fr build-mac
mkdir build-mac
cd build-mac

# Setup without QT (doesn't exist for x86_64) and no libtiff as it is buggy
qt_flags="--enable-mediactrl=no --enable-sound=no --with-libtiff=no"

# Configure for ppc  
# NOTE: This is the magic line to make it work:
# --with-libiconv-prefix=/opt/local/
# This forces wxWidgets to use the MacPorts installation of libiconv and
# not the one bundled with Apple. THIS IS NECESSARY OR usbpicprog WILL FAIL
# WHEN BUILDING!
arch_flags="-arch ppc $sdk_flags"
../configure "$qt_flags" CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --disable-shared --with-libiconv-prefix=/opt/local/ --prefix="$PREFIX_ppc_cocoa" --with-osx_cocoa CC=gcc-4.0 CXX=g++-4.0 LD=g++-4.0

make -j 2
make install
make clean
# Configure for i386 
# NOTE: This is the magic line to make it work:
# --with-libiconv-prefix=/opt/local/
# This forces wxWidgets to use the MacPorts installation of libiconv and
# not the one bundled with Apple. THIS IS NECESSARY OR usbpicprog WILL FAIL
# WHEN BUILDING!
arch_flags="-arch i386 $sdk_flags"
../configure "$qt_flags" CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --disable-shared --with-libiconv-prefix=/opt/local/ --prefix="$PREFIX_i386_cocoa" --with-osx_cocoa CC=gcc-4.0 CXX=g++-4.0 LD=g++-4.0
make -j 2
make install
make clean

# Configure for x86_64 
# NOTE: This is the magic line to make it work:
# --with-libiconv-prefix=/opt/local/
# This forces wxWidgets to use the MacPorts installation of libiconv and
# not the one bundled with Apple. THIS IS NECESSARY OR usbpicprog WILL FAIL
# WHEN BUILDING!
arch_flags="-arch x86_64 $sdk_flags"
../configure "$qt_flags" CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --disable-shared --with-libiconv-prefix=/opt/local/ --prefix="$PREFIX_x86_64_cocoa" --with-osx_cocoa CC=gcc-4.0 CXX=g++-4.0 LD=g++-4.0
make -j 2
make install
make clean

