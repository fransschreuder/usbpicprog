#!/bin/bash

OUTPUTPATH=$(pwd)

make clean

rm -rf src/usbpicprog.app
mkdir -p src/usbpicprog.app
mkdir -p src/usbpicprog.app/Contents
mkdir -p src/usbpicprog.app/Contents/MacOS
mkdir -p src/usbpicprog.app/Contents/Resources
mkdir -p src/usbpicprog.app/Contents/SharedSupport
mkdir -p src/usbpicprog.app/Contents/MacOS/output

./configure --prefix=${OUTPUTPATH}/src/usbpicprog.app/Contents/MacOS/output
make -j 2
make install

cp src/usbpicprog.app/Contents/MacOS/output/bin/usbpicprog src/usbpicprog.app/Contents/MacOS
cp -r src/usbpicprog.app/Contents/MacOS/output/lib/locale src/usbpicprog.app/Contents/MacOS/po
cp src/Info.plist src/usbpicprog.app/Contents
cp icons/usbpicprog.icns src/usbpicprog.app/Contents/Resources
echo -n "APPL????" > src/usbpicprog.app/Contents/PkgInfo

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