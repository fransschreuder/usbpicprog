#!/bin/bash
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
svn export uc_code release/firmware/firmware-$1
svn export boot release/firmware/boot1.0
svn export upp_wx release/usbpicprog/usbpicprog-$1

echo copy hexfiles...
cp release/firmware/firmware-$1/uc_code.hex release/firmware/firmware-$1.hex
cp release/firmware/boot1.0/Boot.hex release/firmware/boot1.0.hex

echo making firmware tar.gz archive...
cd release/firmware
tar -zcvhf firmware-$1.tar.gz *
mv firmware-$1.tar.gz ..
cd ..

echo creating autogen files
cd usbpicprog/usbpicprog-$1
./autogen.sh
cp po/*.mo ..
make distclean
echo restoring language files...
mv ../*.mo po
rm -rf m4
rm -rf autom4te.cache
cd ..
echo creating source archive...
tar -zcvhf usbpicprog-$1.tar.gz usbpicprog-$1
mv usbpicprog-$1.tar.gz ..

cd usbpicprog-$1
echo creating deb package...
./mkdeb.sh
mv usbpicprog0.2.0.deb ../../usbpicprog-$1.deb
cd ../..
rm -rf firmware
rm -rf usbpicprog
echo done...
