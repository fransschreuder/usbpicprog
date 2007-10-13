STOPDIR = ../../..
include($${STOPDIR}/lib.pro)

TARGET   = coff
HEADERS += gpopcode.h disassembler.h coff.h text_coff.h coff_data.h
SOURCES += gpopcode.cpp gpdis.cpp disassembler.cpp coff.cpp text_coff.cpp coff_data.cpp
