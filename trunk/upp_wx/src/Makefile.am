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
	uppmainwindow.h \
	main.cpp \
	uppmainwindow_base.cpp \
	uppmainwindow_base.h \
	hardware.cpp \
	hardware.h \
	pictype.cpp \
	pictype.h \
	hexfile.cpp \
	hexfile.h \
	main.h \
	hexview.cpp \
	hexview.h \
	packageview.cpp \
	packageview.h \
	configview.cpp \
	configview.h \
	preferences_base.cpp \
	preferences_base.h \
	preferences.cpp \
	preferences.h \
	io_test_base.cpp \
	io_test_base.h \
	io_test.cpp \
	io_test.h

usbpicprog_LDFLAGS =

usbpicprog_LDADD = \
	$(USBPICPROG_LIBS) \
	$(WX_LIBS) \
	$(LIBUSB_LIBS)

