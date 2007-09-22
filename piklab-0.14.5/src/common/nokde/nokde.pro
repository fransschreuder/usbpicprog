STOPDIR = ../../..
include($${STOPDIR}/lib.pro)

TARGET   = nokde
HEADERS += nokde_klocale.h nokde_kaboutdata.h nokde_kcmdlineargs.h nokde_kprocess.h
SOURCES += nokde_klocale.cpp nokde_kaboutdata.cpp nokde_kcmdlineargs.cpp nokde_kprocess.cpp
win32:HEADERS += win32_utils.h
win32:SOURCES += win32_utils.c