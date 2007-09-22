INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libcoff.la
libcoff_la_LDFLAGS = $(all_libraries)
libcoff_la_SOURCES = coff.cpp gpdis.cpp gpopcode.cpp disassembler.cpp \
		coff_data.cpp text_coff.cpp
libcoff_la_DEPENDENCIES = coff_data.cpp

noinst_DATA = coff.xml
coff_data.cpp: ../xml/xml_coff_parser coff.xml
	../xml/xml_coff_parser
CLEANFILES = coff_data.cpp
