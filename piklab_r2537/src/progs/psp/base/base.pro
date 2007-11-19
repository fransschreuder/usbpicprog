STOPDIR = ../../../..
include($${STOPDIR}/lib.pro)

TARGET   = psp
HEADERS += psp.h psp_prog.h psp_data.h psp_serial.h
SOURCES += psp.cpp psp_prog.cpp psp_data.cpp psp_serial.cpp
