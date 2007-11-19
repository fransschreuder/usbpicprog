INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libpicxml.la
libpicxml_la_SOURCES = pic_data.cpp
libpicxml_la_DEPENDENCIES = pic_data.cpp

include deps.mak
noinst_DATA += registers/registers.xml registers/registers_missing.xml
pic_data.cpp: ../xml/pic_xml_to_data $(noinst_DATA)
	../xml/pic_xml_to_data
CLEANFILES = pic_data.cpp
