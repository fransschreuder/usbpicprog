STOPDIR = ../../../..
include($${STOPDIR}/lib.pro)

TARGET   = pic
HEADERS += pic_memory.h pic_group.h pic_prog.h pic_debug.h pic_prog_specific.h
SOURCES += pic_memory.cpp pic_group.cpp pic_prog.cpp pic_debug.cpp pic_prog_specific.cpp
