INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libicd2.la
libicd2_la_SOURCES = microchip.cpp icd2.cpp icd2_prog.cpp icd2_serial.cpp \
		icd2_usb.cpp icd2_usb_firmware.cpp icd2_data.cpp icd2_debug.cpp icd.cpp \
		icd_prog.cpp icd2_debug_specific.cpp
libicd2_la_DEPENDENCIES = icd2_data.cpp

noinst_DATA = icd2.xml
icd2_data.cpp: ../xml/xml_icd2_parser icd2.xml
	../xml/xml_icd2_parser
CLEANFILES = icd2_data.cpp
