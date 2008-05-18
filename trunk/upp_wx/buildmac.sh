make clean
./autogen.sh --with-macosx-sdk=/Developer/SDKs/MacOSX10.4u.sdk --with-macosx-version-min=10.4 --disable-debug
make
./mac_copytoapp.sh