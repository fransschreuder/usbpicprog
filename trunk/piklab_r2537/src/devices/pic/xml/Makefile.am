INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_PROGRAMS = pic_xml_to_data

pic_xml_to_data_SOURCES = pic_xml_to_data.cpp
pic_xml_to_data_DEPENDENCIES = $(top_builddir)/src/devices/pic/base/libpicbase.la \
	$(top_builddir)/src/xml_to_data/libxmltodata.la $(top_builddir)/src/devices/base/libdevicebase.la \
	$(top_builddir)/src/common/common/libcommon.la
pic_xml_to_data_LDADD = $(top_builddir)/src/devices/pic/base/libpicbase.la \
	$(top_builddir)/src/xml_to_data/libxmltodata.la $(top_builddir)/src/devices/base/libdevicebase.la \
	$(top_builddir)/src/common/common/libcommon.la $(LIB_KDECORE)
