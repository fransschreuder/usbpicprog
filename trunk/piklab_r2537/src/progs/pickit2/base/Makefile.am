INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libpickit2.la
libpickit2_la_SOURCES = pickit2_data.cpp pickit.cpp pickit_prog.cpp \
	pickit2.cpp pickit2_prog.cpp
libpickit2_la_DEPENDENCIES = pickit2_data.cpp

noinst_DATA = pickit2.xml
pickit2_data.cpp: ../xml/xml_pickit2_parser pickit2.xml
	../xml/xml_pickit2_parser
CLEANFILES = pickit2_data.cpp
