#!/opt/local/bin/bash

# Get our settings
source settings

# Save current dir
CURR_DIR=`pwd`

# Get the release

if [ $# -ne 0 ]
then
  RELEASE=$1
else
  cd ..
  RELEASE=$(date +"%y%m%d")
  RELEASE=${RELEASE:0:6}
fi


#cd ../../..
#RELEASE=`svnversion -n`
cd "$CURR_DIR"

# Create the disk image 
mkdir TMP
cd TMP
ln -s /Applications
cd ..
cp -r build/src/usbpicprog.app TMP/.
yoursway-create-dmg-1.0.0.2/create-dmg --window-size 480 240 --volname UsbPicProg --background bgr.png --icon "usbpicprog.app" 120 120 --icon "Applications" 380 120 "$PREFIX_app/UsbPicProg-OSX-r$RELEASE.dmg" TMP/

# Clean up
rm -fr TMP
