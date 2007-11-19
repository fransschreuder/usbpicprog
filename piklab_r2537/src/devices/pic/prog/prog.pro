STOPDIR = ../../../..
include($${STOPDIR}/lib.pro)

TARGET   = picprog
HEADERS += pic_prog.h pic_debug.h pic_prog_specific.h
SOURCES += pic_prog.cpp pic_debug.cpp pic_prog_specific.cpp
