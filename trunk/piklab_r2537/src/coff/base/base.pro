STOPDIR = ../../..
include($${STOPDIR}/lib.pro)

TARGET   = coff
HEADERS += gpopcode.h disassembler.h coff.h coff_object.h coff_archive.h text_coff.h coff_data.h cdb_parser.h
SOURCES += gpopcode.cpp gpdis.cpp disassembler.cpp coff.cpp coff_object.cpp coff_archive.cpp text_coff.cpp coff_data.cpp cdb_parser.cpp
