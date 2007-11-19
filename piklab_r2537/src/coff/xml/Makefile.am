INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_PROGRAMS = xml_coff_parser
xml_coff_parser_SOURCES = xml_coff_parser.cpp gpprocessor.cpp
OBJECTS = $(top_builddir)/src/devices/list/libdevicelistnoui.la \
	$(top_builddir)/src/devices/pic/pic/libpic.la $(top_builddir)/src/devices/pic/base/libpicbase.la \
	$(top_builddir)/src/devices/pic/xml_data/libpicxml.la \
	$(top_builddir)/src/devices/mem24/mem24/libmem24.la $(top_builddir)/src/devices/mem24/base/libmem24base.la \
	$(top_builddir)/src/devices/mem24/xml_data/libmem24xml.la \
	$(top_builddir)/src/xml_to_data/libxmltodata.la $(top_builddir)/src/devices/base/libdevicebase.la \
	$(top_builddir)/src/common/common/libcommon.la
xml_coff_parser_LDADD = $(OBJECTS) $(LIB_KDECORE)
xml_coff_parser_DEPENDENCIES = $(OBJECTS)
