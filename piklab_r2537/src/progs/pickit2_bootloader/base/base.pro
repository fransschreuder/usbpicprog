STOPDIR = ../../../..
include($${STOPDIR}/lib.pro)

TARGET   = pickit2_bootloader
HEADERS += pickit2_bootloader.h pickit2_bootloader_prog.h pickit2_bootloader_data.h
SOURCES += pickit2_bootloader.cpp pickit2_bootloader_prog.cpp pickit2_bootloader_data.cpp
