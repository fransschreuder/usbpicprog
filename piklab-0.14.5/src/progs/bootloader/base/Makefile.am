INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libbootloader.la
libbootloader_la_SOURCES = bootloader_prog.cpp bootloader.cpp \
			bootloader_data.cpp bootloader_tbl.cpp bootloader_pickit2.cpp bootloader_picdem.cpp
libbootloader_la_DEPENDENCIES = bootloader_data.cpp

noinst_DATA = bootloader.xml
bootloader_data.cpp: ../xml/xml_bootloader_parser bootloader.xml
	../xml/xml_bootloader_parser
CLEANFILES = bootloader_data.cpp
