INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libport.la
libport_la_SOURCES = parallel.cpp port.cpp serial.cpp usb_port.cpp \
	port_base.cpp
libport_la_LDFLAGS = $(all_libraries)
