#!/bin/bash

OUTPUTPATH=$(pwd)

cp -R osx/* .

make clean

rm -rf src/usbpicprog.app
mkdir -p src/usbpicprog.app
mkdir -p src/usbpicprog.app/Contents
mkdir -p src/usbpicprog.app/Contents/MacOS
mkdir -p src/usbpicprog.app/Contents/Resources
mkdir -p src/usbpicprog.app/Contents/SharedSupport
mkdir -p src/usbpicprog.app/Contents/MacOS/output

./configure CC=gcc-4.0 CXX=g++-4.0 LD=g++-4.0 --prefix=${OUTPUTPATH}/src/usbpicprog.app/Contents/MacOS/output
make -j 2
make install

cp src/usbpicprog.app/Contents/MacOS/output/bin/usbpicprog src/usbpicprog.app/Contents/MacOS
cp -r src/usbpicprog.app/Contents/MacOS/output/lib/locale src/usbpicprog.app/po
cp src/Info.plist src/usbpicprog.app/Contents
cp icons/usbpicprog.icns src/usbpicprog.app/Contents/Resources
echo -n "APPL????" > src/usbpicprog.app/Contents/PkgInfo
cp -R xml_data src/usbpicprog.app/xml_data
cp index.xml src/usbpicprog.app/xml_data

rm -rf src/usbpicprog.app/Contents/MacOS/output

EXECFILE="src/usbpicprog.app/Contents/MacOS/usbpicprog"
LIBPATH="libs/"
NEWLIBPATH="@executable_path/../SharedSupport"

# space separated list of libraries
TARGETS="libusb.dylib"
for TARGET in ${TARGETS} ; do
LIBFILE=${LIBPATH}/${TARGET}
cp ${LIBFILE} src/usbpicprog.app/Contents/SharedSupport
LIBFILE=src/usbpicprog.app/Contents/SharedSupport/${TARGET}
TARGETID=`otool -DX ${LIBPATH}/$TARGET`
NEWTARGETID=${NEWLIBPATH}/${TARGET}
install_name_tool -id ${NEWTARGETID} ${LIBFILE}
install_name_tool -change ${TARGETID} ${NEWTARGETID} ${EXECFILE}
done
