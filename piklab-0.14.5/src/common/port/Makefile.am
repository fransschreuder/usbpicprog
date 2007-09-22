INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO
libport_la_LDFLAGS = $(all_libraries)
noinst_LTLIBRARIES = libport.la
libport_la_SOURCES = parallel.cpp port.cpp serial.cpp usb_port.cpp \
	port_base.cpp
noinst_HEADERS = port_base.h
libport_la_LIBADD = $(LIB_KDECORE)
