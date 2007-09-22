INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libdirectprog.la
libdirectprog_la_SOURCES = direct_pic.cpp direct_baseline.cpp direct_16.cpp \
	direct_16F.cpp direct_18.cpp direct_18F.cpp direct_prog.cpp direct_prog_config.cpp \
	direct_data.cpp direct_mem24.cpp direct.cpp
libdirectprog_la_DEPENDENCIES = direct_data.cpp

noinst_DATA = direct.xml
direct_data.cpp: ../xml/xml_direct_parser direct.xml
	../xml/xml_direct_parser
CLEANFILES = direct_data.cpp
