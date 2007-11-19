STOPDIR = ../../../..
include($${STOPDIR}/lib.pro)

TARGET   = picdem_bootloader
HEADERS += picdem_bootloader.h picdem_bootloader_prog.h picdem_bootloader_data.h
SOURCES += picdem_bootloader.cpp picdem_bootloader_prog.cpp picdem_bootloader_data.cpp
