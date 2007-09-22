INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libusbpicprogprog.la
libusbpicprogprog_la_SOURCES = usbpicprog_pic.cpp usbpicprog_baseline.cpp usbpicprog_16.cpp \
	usbpicprog_16F.cpp usbpicprog_18.cpp usbpicprog_18F.cpp usbpicprog_prog.cpp usbpicprog_prog_config.cpp \
	usbpicprog_data.cpp usbpicprog_mem24.cpp usbpicprog.cpp
libusbpicprogprog_la_DEPENDENCIES = usbpicprog_data.cpp

noinst_DATA = usbpicprog.xml
usbpicprog_data.cpp: ../xml/xml_usbpicprog_parser usbpicprog.xml
	../xml/xml_usbpicprog_parser
CLEANFILES = usbpicprog_data.cpp
