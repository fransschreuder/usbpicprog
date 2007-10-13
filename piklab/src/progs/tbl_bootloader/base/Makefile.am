INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libtblbootloader.la
libtblbootloader_la_SOURCES = tbl_bootloader_data.cpp tbl_bootloader.cpp tbl_bootloader_prog.cpp
libtblbootloader_la_DEPENDENCIES = tbl_bootloader_data.cpp

noinst_DATA = tbl_bootloader.xml
tbl_bootloader_data.cpp: ../xml/xml_tbl_bootloader_parser tbl_bootloader.xml
	../xml/xml_tbl_bootloader_parser
CLEANFILES = tbl_bootloader_data.cpp
