## Process this file with automake to produce Makefile.in

## Created by Anjuta

INCLUDES = \
	$(WX_CXXFLAGS) \
	$(LIBUSB_CFLAGS) \
	-DPACKAGE_PREFIX=\""$(prefix)"\"

AM_CFLAGS =\
	 -Wall \
	 -g \
	 $(WX_CPPFLAGS)

bin_PROGRAMS = usbpicprog

usbpicprog_SOURCES = \
	uppmainwindow.cpp \
	main.cpp \
	uppmainwindow_base.cpp \
	hardware.cpp \
	hardware.h \
	include_icons.h \
	pictype.cpp \
	pictype.h \
	hexfile.cpp \
	hexfile.h \
	main.h \
	uppmainwindow_base.h \
	uppmainwindow.h \
	hexview.cpp \
	hexview.h \
	packageview.cpp \
	packageview.h \
	configview.cpp \
	configview.h \
	preferences.cpp \
	preferences.h

usbpicprog_LDFLAGS =

usbpicprog_LDADD = \
	$(USBPICPROG_LIBS) \
	$(WX_LIBS) \
	$(LIBUSB_LIBS)

