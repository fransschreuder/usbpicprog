STOPDIR = ../../..
include($${STOPDIR}/lib.pro)

TARGET   = devicebase
HEADERS += generic_device.h hex_buffer.h generic_memory.h device_group.h register.h
SOURCES += generic_device.cpp hex_buffer.cpp generic_memory.cpp register.cpp
