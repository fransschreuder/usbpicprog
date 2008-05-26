#!/bin/sh -e
#in upp_wx:
#help2man needs to be edited (change "2>/dev/null" into "2>&1")
cd deb_package
if [ -d usr ]
then
	echo usr exists, removing ...
	rm -rf usr
fi
cd ..
./configure --prefix=$HOME/usbpicprog/trunk/upp_wx/deb_package/usr/local
make 
make install
mkdir deb_package/usr/local/man/
mkdir deb_package/usr/local/man/man1
help2man deb_package/usr/local/bin/usbpicprog>deb_package/usr/local/man/man1/usbpicprog.1
gzip --best deb_package/usr/local/man/man1/usbpicprog.1
gzip --best deb_package/usr/local/doc/usbpicprog/COPYING
gzip --best deb_package/usr/local/doc/usbpicprog/ChangeLog
md5deep -r -l usr>deb_package/DEBIAN/md5sums
dpkg-deb --build deb_package
mv deb_package.deb usbpicprog0.1.deb
echo FINISHED

