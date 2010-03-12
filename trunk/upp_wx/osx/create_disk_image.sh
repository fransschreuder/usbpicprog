#!/opt/local/bin/bash

# Get our settings
source settings

# Get the release
REL=`pwd`
arr=$(echo $REL |sed -e 's/usbpicprog-/ /g' )

for x in $arr 
do
    RELEASE=`echo $x`
done

hdiutil create -srcfolder src/usbpicprog.app "$PREFIX_app/usbpicprog_Build-$RELEASE.dmg"

