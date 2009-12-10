#!/bin/bash


if [ $# -ne 0 ]
then
  RELEASE=$1
else
  RELEASE=$(svnversion -n)
#-$(date +%Y%m%d)
fi
echo Building release: $RELEASE
#ARCHITECTURE=$(uname -m)
#echo Building for architecture: $ARCHITECTURE

if [ -d release ]
then
	echo Directory release exists, removing ...
	rm -rf release
fi
echo making release directory
mkdir release
#mkdir release/usbpicprog
#mkdir release/firmware

echo exporting svn tree...
#svn export uc_code release/firmware/firmware-$RELEASE
#svn export boot release/firmware/boot1.0
svn export upp_wx release/usbpicprog-$RELEASE

#echo copy hexfiles...
#cp release/firmware/firmware-$RELEASE/uc_code.hex release/firmware/firmware-$RELEASE.hex
#cp release/firmware/boot1.0/Boot.hex release/firmware/boot1.0.hex

#echo making firmware tar.gz archive...
#cd release/firmware
#tar -zcvhf firmware-$RELEASE.tar.gz *
#mv firmware-$RELEASE.tar.gz ..
#cd ..

echo creating autogen files
cd release/usbpicprog-$RELEASE
./autogen.sh
make distclean
rm -rf m4
rm -rf autom4te.cache
if [ "$RELEASE" = "0.3.0" ]; then
( echo '// generated file';
    echo '#define UPP_VERSION "usbpicprog '$RELEASE'"'; )> svn_revision.h;
else
( echo '// generated file';
    echo '#define SVN_REVISION "usbpicprog '$RELEASE'"'; )> svn_revision.h;
fi

cd ..
echo creating source archive...
tar -zcvhf usbpicprog_$RELEASE.orig.tar.gz usbpicprog-$RELEASE
rm -rf usbpicprog-$RELEASE
tar -zxvf usbpicprog_$RELEASE.orig.tar.gz
cd usbpicprog-$RELEASE
dh_make -e fransschreuder@gmail.com -s
cp debian-src/* debian
cd debian
rm *.ex
rm *.EX
rm README.Debian
cd ..
debuild -S -k8AD5905E

