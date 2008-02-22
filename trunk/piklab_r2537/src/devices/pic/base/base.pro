STOPDIR = ../../../..
include($${STOPDIR}/lib.pro)

TARGET   = picbase
HEADERS += pic_protection.h pic_config.h pic_register.h pic.h
SOURCES += pic_protection.cpp pic_config.cpp pic_register.cpp pic.cpp
