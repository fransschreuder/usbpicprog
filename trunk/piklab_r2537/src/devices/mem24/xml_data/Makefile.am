INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libmem24xml.la
libmem24xml_la_LDFLAGS = $(all_libraries)
libmem24xml_la_SOURCES = mem24_data.cpp
libmem24xml_la_DEPENDENCIES = mem24_data.cpp

include deps.mak
mem24_data.cpp: ../xml/mem24_xml_to_data $(noinst_DATA)
	../xml/mem24_xml_to_data
CLEANFILES = mem24_data.cpp
