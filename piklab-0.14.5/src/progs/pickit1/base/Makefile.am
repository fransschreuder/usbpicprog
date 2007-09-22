INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libpickit1.la
libpickit1_la_SOURCES = pickit1.cpp pickit1_prog.cpp pickit1_data.cpp
libpickit1_la_DEPENDENCIES = pickit1_data.cpp

noinst_DATA = pickit1.xml
pickit1_data.cpp: ../xml/xml_pickit1_parser pickit1.xml
	../xml/xml_pickit1_parser
CLEANFILES = pickit1_data.cpp
