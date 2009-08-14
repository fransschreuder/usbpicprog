#!/bin/sh -e
#help2man needs to be edited (change "2>/dev/null" into "2>&1")
ARCHITECTURE=$(uname -m)
if [ -d deb_package ]
then
	echo Directory deb_package exists, removing ...
	rm -rf deb_package
fi
mkdir deb_package
mkdir deb_package/DEBIAN
cp control-$ARCHITECTURE deb_package/DEBIAN/control
cp postinst deb_package/DEBIAN
cp prerm deb_package/DEBIAN
#first configure with the normal prefix in favour of xml_data of which the path is hard linked into the binary
./configure --prefix=/usr/local
make
./configure --prefix=$(pwd)/deb_package/usr/local 
make install
mkdir deb_package/usr/local/man/
mkdir deb_package/usr/local/man/man1
mkdir deb_package/usr/share
#cp -r deb_package/usr/local/share/locale/ deb_package/usr/share/
mkdir deb_package/usr/share/pixmaps
mkdir deb_package/usr/share/applications
mkdir deb_package/etc
mkdir deb_package/etc/udev
mkdir deb_package/etc/udev/rules.d
cp icons/usbpicprog.svg deb_package/usr/share/pixmaps
cp Usbpicprog.desktop deb_package/usr/share/applications
cp 26-microchip.rules deb_package/etc/udev/rules.d
./help2man deb_package/usr/local/bin/usbpicprog>deb_package/usr/local/man/man1/usbpicprog.1
gzip --best deb_package/usr/local/man/man1/usbpicprog.1
gzip --best deb_package/usr/local/doc/usbpicprog/COPYING
gzip --best deb_package/usr/local/doc/usbpicprog/ChangeLog
md5deep -r -l deb_package/usr>deb_package/DEBIAN/md5sums
dpkg-deb --build deb_package usbpicprog0.3.0.deb
echo FINISHED

