STOPDIR = ../../../..
include($${STOPDIR}/app.pro)

TARGET   = mem24_xml_to_data
SOURCES += mem24_xml_to_data.cpp
LIBS    += ../../../devices/mem24/base/libmem24base.a ../../../xml_to_data/libxmltodata.a \
           ../../../devices/base/libdevicebase.a ../../../common/global/libglobal.a \
           ../../../common/nokde/libnokde.a ../../../common/common/libcommon.a

unix:QMAKE_POST_LINK = cd ../xml_data && ../xml/mem24_xml_to_data
unix:QMAKE_CLEAN += ../xml_data/mem24_data.cpp
win32:QMAKE_POST_LINK = cd ..\xml_data && ..\xml\mem24_xml_to_data.exe
win32:QMAKE_CLEAN += ..\xml_data\mem24_data.cpp
