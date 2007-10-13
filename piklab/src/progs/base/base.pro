STOPDIR = ../../..
include($${STOPDIR}/lib.pro)

TARGET   = progbase
HEADERS += generic_prog.h generic_debug.h prog_config.h prog_group.h prog_specific.h hardware_config.h
SOURCES += generic_prog.cpp generic_debug.cpp prog_config.cpp prog_group.cpp prog_specific.cpp hardware_config.cpp
