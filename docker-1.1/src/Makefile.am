AM_CPPFLAGS=-DDATADIR="\"$(pkgdatadir)\"" -DSYSCONFDIR="\"$(sysconfdir)\""
AM_CXXFLAGS=-g

bin_PROGRAMS = docker
docker_SOURCES = main.cxx DataBuffer.h DataBuffer.cxx HexFile.h \
					   HexFile_ihx16.cxx HexFile_ihx8.cxx HexFile.cxx \
					   usbdevice.cxx usbdevice.h
docker_LDADD = @libusb_LIBS@
