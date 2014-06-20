#!/opt/local/bin/bash
# This file install wxWidgets 2.9.4 for both ppc and i386
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
if [ -d "$PREFIX_ppc" ]; then
	echo "$PREFIX_ppc exists!"
else 	
	echo "Creating $PREFIX_ppc"
	mkdir -p "$PREFIX_ppc"
fi
if [ -d "$PREFIX_i386" ]; then
	echo "$PREFIX_i386 exists!"
else 	
	echo "Creating $PREFIX_i386"
	mkdir -p "$PREFIX_i386"
fi
if [ -d "$PREFIX_x86_64" ]; then
	echo "$PREFIX_x86_64 exists!"
else 	
	echo "Creating $PREFIX_x86_64"
	mkdir -p "$PREFIX_x86_64"
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

#Create the links to make the libtool(-ize) use glibtool(-ize) instead!
ln -s /opt/local/bin/glibtool "$PREFIX_ppc"/bin/libtool
ln -s /opt/local/bin/glibtoolize "$PREFIX_ppc"/bin/libtoolize
ln -s /opt/local/bin/glibtool "$PREFIX_i386"/bin/libtool
ln -s /opt/local/bin/glibtoolize "$PREFIX_i386"/bin/libtoolize
ln -s /opt/local/bin/glibtool "$PREFIX_x86_64"/bin/libtool
ln -s /opt/local/bin/glibtoolize "$PREFIX_x86_64"/bin/libtoolize

# Download wxWidgets
cd "$WORKDIR"
if [ -f wxWidgets-3.0.1.tar.bz2 ]; then
	if [ -d wxWidgets-3.0.1 ]; then
		rm -rf wxWidgets-3.0.1	
	fi
else
	wget http://prdownloads.sourceforge.net/wxwindows/wxWidgets-3.0.1.tar.bz2
fi
# Unpack it
tar xjf wxWidgets-3.0.1.tar.bz2
# Get in there and create a builddir
cd wxWidgets-3.0.1
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
../configure "$qt_flags" CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --disable-shared --with-libiconv-prefix=/opt/local/ --prefix="$PREFIX_ppc" CC=gcc-4.0 CXX=g++-4.0 LD=g++-4.0
make -j 2
make install
make distclean

# Configure for i386 
# NOTE: This is the magic line to make it work:
# --with-libiconv-prefix=/opt/local/
# This forces wxWidgets to use the MacPorts installation of libiconv and
# not the one bundled with Apple. THIS IS NECESSARY OR usbpicprog WILL FAIL
# WHEN BUILDING!
arch_flags="-arch i386 $sdk_flags"
../configure "$qt_flags" CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --disable-shared --with-libiconv-prefix=/opt/local/ --prefix="$PREFIX_i386" CC=gcc-4.0 CXX=g++-4.0 LD=g++-4.0
make -j 2
make install
make distclean

# Configure for x86_64 
# NOTE: This is the magic line to make it work:
# --with-libiconv-prefix=/opt/local/
# This forces wxWidgets to use the MacPorts installation of libiconv and
# not the one bundled with Apple. THIS IS NECESSARY OR usbpicprog WILL FAIL
# WHEN BUILDING!
arch_flags="-arch x86_64 $sdk_flags"
../configure "$qt_flags" CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --disable-shared --with-libiconv-prefix=/opt/local/ --prefix="$PREFIX_x86_64" CC=gcc-4.0 CXX=g++-4.0 LD=g++-4.0
make -j 2
make install
make distclean
