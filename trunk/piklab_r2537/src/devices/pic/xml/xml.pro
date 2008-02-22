STOPDIR = ../../../..
include($${STOPDIR}/app.pro)

TARGET   = pic_xml_to_data
SOURCES += pic_xml_to_data.cpp
LIBS    += ../../../devices/pic/base/libpicbase.a ../../../xml_to_data/libxmltodata.a \
           ../../../devices/base/libdevicebase.a ../../../common/global/libglobal.a \
           ../../../common/nokde/libnokde.a ../../../common/common/libcommon.a

unix:QMAKE_POST_LINK = cd ../xml_data && ../xml/pic_xml_to_data
unix:QMAKE_CLEAN += ../xml_data/pic_data.cpp
win32:QMAKE_POST_LINK = cd ..\xml_data && ..\xml\pic_xml_to_data.exe
win32:QMAKE_CLEAN += ..\xml_data\pic_data.cpp
