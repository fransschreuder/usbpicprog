INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_PROGRAMS = mem24_xml_to_data

mem24_xml_to_data_SOURCES = mem24_xml_to_data.cpp
mem24_xml_to_data_DEPENDENCIES = $(top_builddir)/src/devices/mem24/base/libmem24base.la \
	$(top_builddir)/src/xml_to_data/libxmltodata.la $(top_builddir)/src/devices/base/libdevicebase.la \
	$(top_builddir)/src/common/common/libcommon.la
mem24_xml_to_data_LDADD = $(top_builddir)/src/devices/mem24/base/libmem24base.la \
	$(top_builddir)/src/xml_to_data/libxmltodata.la $(top_builddir)/src/devices/base/libdevicebase.la \
	$(top_builddir)/src/common/common/libcommon.la $(LIB_KDECORE)
