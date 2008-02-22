STOPDIR = ../../../..
include($${STOPDIR}/lib.pro)

TARGET   = icd2
HEADERS += microchip.h icd.h icd_prog.h icd2.h icd2_data.h \
           icd2_prog.h icd2_debug_specific.h icd2_debug.h icd2_serial.h icd2_usb.h
SOURCES += microchip.cpp icd.cpp icd_prog.cpp icd2.cpp icd2_data.cpp \
           icd2_prog.cpp icd2_debug_specific.cpp icd2_debug.cpp icd2_serial.cpp icd2_usb.cpp icd2_usb_firmware.cpp
