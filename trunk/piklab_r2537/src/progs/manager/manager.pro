STOPDIR = ../../..
include($${STOPDIR}/lib.pro)

TARGET   = progmanager
HEADERS += breakpoint.h prog_manager.h debug_manager.h
SOURCES += breakpoint.cpp prog_manager.cpp debug_manager.cpp
