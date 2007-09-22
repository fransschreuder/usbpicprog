STOPDIR = ../..
include($${STOPDIR}/lib.pro)

TARGET   = cli
HEADERS += cli_global.h cli_log.h cli_main.h
SOURCES += cli_global.cpp cli_log.cpp cli_purl.cpp cli_pfile.cpp cli_main.cpp
