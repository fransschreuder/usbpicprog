INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libpicdembootloader.la
libpicdembootloader_la_SOURCES = picdem_bootloader_data.cpp picdem_bootloader.cpp picdem_bootloader_prog.cpp
libpicdembootloader_la_DEPENDENCIES = picdem_bootloader_data.cpp

noinst_DATA = picdem_bootloader.xml
picdem_bootloader_data.cpp: ../xml/xml_picdem_bootloader_parser picdem_bootloader.xml
	../xml/xml_picdem_bootloader_parser
CLEANFILES = picdem_bootloader_data.cpp
