#!/opt/local/bin/bash

# Get our settings
source settings

# Save current dir
CURR_DIR=`pwd`

# Get the release
cd ../../..
RELEASE=`svnversion -n`
cd "$CURR_DIR"

# Create the disk image 
hdiutil create -srcfolder build/src/usbpicprog.app "$PREFIX_app/UsbPicProg-OSX-r$RELEASE.dmg"

