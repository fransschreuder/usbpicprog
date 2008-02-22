STOPDIR = ../../../..
include($${STOPDIR}/lib.pro)

TARGET   = directprog
HEADERS += direct.h direct_data.h direct_pic.h direct_mem24.h direct_prog.h \
           direct_prog_config.h direct_baseline.h direct_16.h direct_16F.h \
           direct_18.h direct_18F.h
SOURCES += direct.cpp direct_data.cpp direct_pic.cpp direct_mem24.cpp direct_prog.cpp \
           direct_prog_config.cpp direct_baseline.cpp direct_16.cpp direct_16F.cpp \
           direct_18.cpp direct_18F.cpp
