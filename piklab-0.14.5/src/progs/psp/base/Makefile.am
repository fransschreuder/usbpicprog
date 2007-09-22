INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libpsp.la
libpsp_la_SOURCES = psp_prog.cpp psp_serial.cpp psp_data.cpp psp.cpp
libpsp_la_DEPENDENCIES = psp_data.cpp

noinst_DATA = psp.xml
psp_data.cpp: ../xml/xml_psp_parser psp.xml
	../xml/xml_psp_parser
CLEANFILES = psp_data.cpp
