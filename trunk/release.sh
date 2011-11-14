#!/bin/bash


if [ $# -ne 0 ]
then
  RELEASE=$1
else
  RELEASE=$(date +"%y%m%d")
  RELEASE=${RELEASE:0:6}
fi
echo Building release: $RELEASE

if [ -d release ]
then
	echo Directory release exists, removing ...
	rm -rf release
fi

echo making release directories release/usbpicprog and release/firmware...
mkdir release


echo exporting git tree...
git clone ../ release/
mkdir release/usbpicprog
mkdir release/firmware
mv release/trunk/upp_wx release/usbpicprog/usbpicprog-$RELEASE
mv release/trunk/uc_code release/firmware/firmware-$RELEASE
mv release/trunk/boot release/firmware/boot1.0
rm -r release/trunk
rm -r release/README.md
rm -rf release/.git

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
if [  ${RELEASE:0:2} = "0."  ]; then
( echo '// generated file';
    echo '#define UPP_VERSION "usbpicprog '$RELEASE'"'; )> svn_revision.h;
else
( echo '// generated file';
    echo '#define SVN_REVISION "usbpicprog '$RELEASE'"'; )> svn_revision.h;
fi

cd ..
echo creating source archive...
tar -zcvhf usbpicprog-$RELEASE.tar.gz usbpicprog-$RELEASE
mv usbpicprog-$RELEASE.tar.gz ..

cd ..
rm -rf firmware
rm -rf usbpicprog
echo done...
