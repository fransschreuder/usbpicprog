STOPDIR = ../../../..
include($${STOPDIR}/lib.pro)

TARGET   = usbpicprogprog
HEADERS += usbpicprog.h usbpicprog_data.h usbpicprog_pic.h usbpicprog_mem24.h usbpicprog_prog.h \
           usbpicprog_prog_config.h usbpicprog_baseline.h usbpicprog_16.h usbpicprog_16F.h \
           usbpicprog_18.h usbpicprog_18F.h
SOURCES += usbpicprog.cpp usbpicprog_data.cpp usbpicprog_pic.cpp usbpicprog_mem24.cpp usbpicprog_prog.cpp \
           usbpicprog_prog_config.cpp usbpicprog_baseline.cpp usbpicprog_16.cpp usbpicprog_16F.cpp \
           usbpicprog_18.cpp usbpicprog_18F.cpp
