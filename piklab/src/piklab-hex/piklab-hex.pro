STOPDIR = ../..
include($${STOPDIR}/app.pro)

TARGET   = piklab-hex
HEADERS += main.h
SOURCES += main.cpp
LIBS    += ../devices/list/libdevicelist.a \
           ../devices/mem24/mem24/libmem24.a ../devices/mem24/xml_data/libmem24xml.a \
           ../devices/mem24/base/libmem24base.a \
           ../devices/pic/pic/libpic.a ../devices/pic/xml_data/libpicxml.a \
           ../devices/pic/base/libpicbase.a \
           ../devices/base/libdevicebase.a \
           ../common/cli/libcli.a ../common/global/libglobal.a \
           ../common/nokde/libnokde.a ../common/common/libcommon.a
