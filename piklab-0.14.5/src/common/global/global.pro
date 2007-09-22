STOPDIR = ../../..
include($${STOPDIR}/lib.pro)

TARGET   = global
HEADERS += about.h generic_config.h log.h process.h purl.h pfile.h
SOURCES += about.cpp generic_config.cpp log.cpp process.cpp purl.cpp pfile.cpp

unix:system(cd svn_revision && sh svn_revision.sh)
