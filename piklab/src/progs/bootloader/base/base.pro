STOPDIR = ../../../..
include($${STOPDIR}/lib.pro)

TARGET   = bootloader
HEADERS += bootloader.h bootloader_prog.h bootloader_data.h bootloader_tbl.h bootloader_pickit2.h bootloader_picdem.h
SOURCES += bootloader.cpp bootloader_prog.cpp bootloader_data.cpp bootloader_tbl.cpp bootloader_pickit2.cpp bootloader_picdem.cpp
