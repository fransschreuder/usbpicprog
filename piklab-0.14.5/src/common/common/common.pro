STOPDIR = ../../..
include($${STOPDIR}/lib.pro)

TARGET   = common
HEADERS += qflags.h misc.h group.h storage.h synchronous.h purl_base.h number.h
SOURCES += misc.cpp group.cpp storage.cpp synchronous.cpp purl_base.cpp number.cpp
