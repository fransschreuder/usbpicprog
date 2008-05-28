#!/bin/sh -e
#help2man needs to be edited (change "2>/dev/null" into "2>&1")
if [ -d deb_package ]
then
	echo Directory deb_package exists, removing ...
	rm -rf deb_package
fi
tar -zxvf deb_package.tar.gz
./configure --prefix=$(pwd)/deb_package/usr/local
make 
make install
mkdir deb_package/usr/local/man/
mkdir deb_package/usr/local/man/man1
mkdir deb_package/usr/share
mkdir deb_package/usr/share/pixmaps
mkdir deb_package/usr/share/applications
mkdir deb_package/etc
mkdir deb_package/etc/udev
mkdir deb_package/etc/udev/rules.d
cp icons/usbpicprog.xpm deb_package/usr/share/pixmaps
cp Usbpicprog.desktop deb_package/usr/share/applications
cp 26-microchip.rules deb_package/etc/udev/rules.d
./help2man deb_package/usr/local/bin/usbpicprog>deb_package/usr/local/man/man1/usbpicprog.1
gzip --best deb_package/usr/local/man/man1/usbpicprog.1
gzip --best deb_package/usr/local/doc/usbpicprog/COPYING
gzip --best deb_package/usr/local/doc/usbpicprog/ChangeLog
md5deep -r -l deb_package/usr>deb_package/DEBIAN/md5sums
dpkg-deb --build deb_package usbpicprog0.1.deb
echo FINISHED

