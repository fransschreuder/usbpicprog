STOPDIR = ../../..
include($${STOPDIR}/lib.pro)

TARGET   = port
HEADERS += port.h port_base.h parallel.h serial.h usb_port.h
SOURCES += port.cpp port_base.cpp parallel.cpp serial.cpp usb_port.cpp
contains(DEFINES, HAVE_USB) {
  win32:INCLUDEPATH += $${LIBUSB_PATH}\include
}

