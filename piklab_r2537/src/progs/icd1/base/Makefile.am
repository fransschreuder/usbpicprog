INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libicd1.la
libicd1_la_SOURCES = icd1.cpp icd1_prog.cpp icd1_serial.cpp icd1_data.cpp
libicd1_la_DEPENDENCIES = icd1_data.cpp

noinst_DATA = icd1.xml
icd1_data.cpp: ../xml/xml_icd1_parser icd1.xml
	../xml/xml_icd1_parser
CLEANFILES = icd1_data.cpp
