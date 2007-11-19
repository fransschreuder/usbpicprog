INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libpickit2bootloader.la
libpickit2bootloader_la_SOURCES = pickit2_bootloader_data.cpp pickit2_bootloader_prog.cpp pickit2_bootloader.cpp
libpickit2bootloader_la_DEPENDENCIES = pickit2_bootloader_data.cpp

noinst_DATA = pickit2_bootloader.xml
pickit2_bootloader_data.cpp: ../xml/xml_pickit2_bootloader_parser pickit2_bootloader.xml
	../xml/xml_pickit2_bootloader_parser
CLEANFILES = pickit2_bootloader_data.cpp
