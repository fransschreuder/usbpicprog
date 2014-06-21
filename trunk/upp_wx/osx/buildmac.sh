#!/bin/bash

#Save output path
OUTPUTPATH=$(pwd)/build

# Load our settings
source settings

# Create a build dir
if [ -d "build" ]; then
	echo "'build' dir exists!"
	rm -rf build
	mkdir build
else 	
	echo "Creating 'build' dir..."
	mkdir build
fi
cd build

# Export path so that we use the correct version of wxWidgets
export PATH="$PREFIX_i386"/bin:"$PREFIX_i386"/sbin:$PATH

# Copy the Makefile
cp ../../Makefile.in.osx ../../Makefile.in

# Cleanup before building (for consecutive builds)
rm -rf src/usbpicprog.app
mkdir -p src/usbpicprog.app
mkdir -p src/usbpicprog.app/Contents
mkdir -p src/usbpicprog.app/Contents/MacOS
mkdir -p src/usbpicprog.app/Contents/Resources
mkdir -p src/usbpicprog.app/Contents/SharedSupport
mkdir -p src/usbpicprog.app/Contents/MacOS/output

# Copy in the universal library libusb
mkdir libs
cp /opt/local/lib/libusb-1.0.dylib libs
cp /opt/local/lib/libiconv.dylib libs
cp /opt/local/lib/libexpat.1.dylib libs

# Export path so that we use the correct version of wxWidgets
export PATH="$PREFIX_ppc"/bin:"$PREFIX_ppc"/sbin:$PATH
arch_flags="-arch ppc $sdk_flags"
../../configure CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --prefix=${OUTPUTPATH}/src/usbpicprog.app/Contents/MacOS/output CC=gcc-4.0 CXX=g++-4.0 LD=g++-4.0

# Build and install for ppc
make -j 2
make install

# Copy the ppc version to the target dir.
cp src/usbpicprog.app/Contents/MacOS/output/bin/usbpicprog "$PREFIX_app"/usbpicprog_ppc

# Cleanup before building again
make clean

# Export path so that we use the correct version of wxWidgets
export PATH="$PREFIX_i386"/bin:"$PREFIX_i386"/sbin:$PATH
arch_flags="-arch i386 $sdk_flags"
../../configure CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --prefix=${OUTPUTPATH}/src/usbpicprog.app/Contents/MacOS/output CC=gcc-4.0 CXX=g++-4.0 LD=g++-4.0

# Build and install for i386
make -j 2
make install

# Copy the i386 version to the target dir.
cp src/usbpicprog.app/Contents/MacOS/output/bin/usbpicprog "$PREFIX_app"/usbpicprog_i386

# Cleanup before building again
make clean

# Export path so that we use the correct version of wxWidgets
export PATH="$PREFIX_x86_64"/bin:"$PREFIX_x86_64"/sbin:$PATH
arch_flags="-arch x86_64 $sdk_flags"
../../configure CFLAGS="$arch_flags" CXXFLAGS="$arch_flags" CPPFLAGS="$arch_flags" LDFLAGS="$arch_flags" OBJCFLAGS="$arch_flags" OBJCXXFLAGS="$arch_flags" --prefix=${OUTPUTPATH}/src/usbpicprog.app/Contents/MacOS/output CC=gcc-4.0 CXX=g++-4.0 LD=g++-4.0

# Build and install for x86_64
make -j 2
make install

# Copy the x86_64 version to the target dir.
cp src/usbpicprog.app/Contents/MacOS/output/bin/usbpicprog "$PREFIX_app"/usbpicprog_x86_64

# Create the universal binary, everything else is "universal" anyways...
lipo "$PREFIX_app"/usbpicprog_ppc "$PREFIX_app"/usbpicprog_i386 "$PREFIX_app"/usbpicprog_x86_64 -create -output src/usbpicprog.app/Contents/MacOS/usbpicprog

# Cleanup
rm "$PREFIX_app"/usbpicprog_ppc 
rm "$PREFIX_app"/usbpicprog_i386
rm "$PREFIX_app"/usbpicprog_x86_64

# Make an app of everything
cp -r src/usbpicprog.app/Contents/MacOS/output/lib/locale src/usbpicprog.app/po
cp -r src/usbpicprog.app/Contents/MacOS/output/share/usbpicprog/examples src/usbpicprog.app/Contents/SharedSupport
cp src/usbpicprog.app/Contents/MacOS/output/share/usbpicprog/usbpicprog.htb src/usbpicprog.app/Contents/SharedSupport
cp src/usbpicprog.app/Contents/MacOS/output/share/usbpicprog/firmware.hex src/usbpicprog.app/Contents/SharedSupport

cp ../../src/Info.plist src/usbpicprog.app/Contents
cp ../../icons/usbpicprog.icns src/usbpicprog.app/Contents/Resources
echo -n "APPL????" > src/usbpicprog.app/Contents/PkgInfo
cp -R ../../xml_data src/usbpicprog.app/Contents/SharedSupport/xml_data
cp ../../index.xml src/usbpicprog.app/Contents/SharedSupport/xml_data

# Cleanup
rm -rf src/usbpicprog.app/Contents/MacOS/output

EXECFILE="src/usbpicprog.app/Contents/MacOS/usbpicprog"
LIBPATH="libs"
NEWLIBPATH="@executable_path/../SharedSupport"

# space separated list of libraries
TARGETS="libusb-1.0.dylib libiconv.dylib libexpat.1.dylib"
for TARGET in ${TARGETS} ; do
	LIBFILE=${LIBPATH}/${TARGET}
	cp ${LIBFILE} src/usbpicprog.app/Contents/SharedSupport
	LIBFILE=src/usbpicprog.app/Contents/SharedSupport/${TARGET}
	TARGETID=`otool -DX ${LIBPATH}/$TARGET`
	NEWTARGETID=${NEWLIBPATH}/${TARGET}
	install_name_tool -id ${NEWTARGETID} ${LIBFILE}
	install_name_tool -change ${TARGETID} ${NEWTARGETID} ${EXECFILE}
done
