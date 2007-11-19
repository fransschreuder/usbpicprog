STOPDIR = ../../..
include($${STOPDIR}/app.pro)

TARGET   = xml_coff_parser
SOURCES += gpprocessor.cpp xml_coff_parser.cpp
LIBS    += ../../devices/list/libdevicelist.a \
           ../../devices/mem24/mem24/libmem24.a ../../devices/mem24/xml_data/libmem24xml.a \
           ../../devices/mem24/base/libmem24base.a \
           ../../devices/pic/pic/libpic.a ../../devices/pic/xml_data/libpicxml.a \
           ../../devices/pic/base/libpicbase.a ../../xml_to_data/libxmltodata.a \
           ../../devices/base/libdevicebase.a ../../common/global/libglobal.a \
           ../../common/nokde/libnokde.a ../../common/common/libcommon.a

unix:QMAKE_POST_LINK = cd ../base && ../xml/xml_coff_parser
unix:QMAKE_CLEAN += ../base/coff_data.cpp
win32:QMAKE_POST_LINK = cd ..\base && ..\xml\xml_coff_parser.exe
win32:QMAKE_CLEAN += ..\base\coff_data.cpp
