## Process this file with automake to produce Makefile.in

## Created by Anjuta

INCLUDES = \
	$(WX_CXXFLAGS) \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" $(USBPICPROG_CFLAGS)\
	$(LIBUSB_CFLAGS)

AM_CFLAGS =\
	 -Wall \
	 -g \
	 $(WX_CPPFLAGS)

bin_PROGRAMS = usbpicprog

usbpicprog_SOURCES = \
	uppmainwindow.cpp \
	main.cpp \
	uppmainwindow_callback.cpp \
	hardware.cpp \
	hardware.h \
	include_icons.h \
	pictype.cpp \
	pictype.h \
	read_hexfile.cpp \
	read_hexfile.h \
	main.h \
	uppmainwindow_callback.h \
	uppmainwindow.h \
	hexview.cpp \
	hexview.h \
	preferences.cpp \
	preferences.h

usbpicprog_LDFLAGS = 

usbpicprog_LDADD = \
	$(USBPICPROG_LIBS) \
	$(WX_LIBS) \
	$(LIBUSB_LIBS)
	





