STOPDIR = ../..
include($${STOPDIR}/app.pro)

TARGET   = piklab-hex
HEADERS += main.h
SOURCES += main.cpp
LIBS    += ../libcli/libcli.a ../devices/list/libdevicelist.a \
           ../devices/mem24/mem24/libmem24.a ../devices/mem24/xml_data/libmem24xml.a \
           ../devices/mem24/base/libmem24base.a \
           ../devices/pic/pic/libpic.a ../devices/pic/xml_data/libpicxml.a \
           ../devices/pic/base/libpicbase.a ../xml_to_data/libxmltodata.a \
           ../devices/base/libdevicebase.a ../common/global/libglobal.a \
           ../common/nokde/libnokde.a ../common/common/libcommon.a
