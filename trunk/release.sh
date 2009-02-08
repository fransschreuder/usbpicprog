#!/bin/bash


if [ $# -ne 0 ]
then
  RELEASE=$1
else
  RELEASE=$(svnversion -n)-$(date +%Y%m%d)
fi
echo Building release: $RELEASE

if [ -d release ]
then
	echo Directory release exists, removing ...
	rm -rf release
fi
echo making release directories release/usbpicprog and release/firmware...
mkdir release
mkdir release/usbpicprog
mkdir release/firmware

echo exporting svn tree...
svn export uc_code release/firmware/firmware-$RELEASE
svn export boot release/firmware/boot1.0
svn export upp_wx release/usbpicprog/usbpicprog-$RELEASE

echo copy hexfiles...
cp release/firmware/firmware-$RELEASE/uc_code.hex release/firmware/firmware-$RELEASE.hex
cp release/firmware/boot1.0/Boot.hex release/firmware/boot1.0.hex

echo making firmware tar.gz archive...
cd release/firmware
tar -zcvhf firmware-$RELEASE.tar.gz *
mv firmware-$RELEASE.tar.gz ..
cd ..

echo creating autogen files
cd usbpicprog/usbpicprog-$RELEASE
./autogen.sh
cp po/*.mo ..
make distclean
echo restoring language files...
mv ../*.mo po
rm -rf m4
rm -rf autom4te.cache
cd ..
echo creating source archive...
tar -zcvhf usbpicprog-$RELEASE.tar.gz usbpicprog-$RELEASE
mv usbpicprog-$RELEASE.tar.gz ..

cd usbpicprog-$RELEASE
echo creating deb package...
./mkdeb.sh
mv usbpicprog0.2.1.deb ../../usbpicprog-$RELEASE.deb
cd ../..
rm -rf firmware
rm -rf usbpicprog
echo done...
