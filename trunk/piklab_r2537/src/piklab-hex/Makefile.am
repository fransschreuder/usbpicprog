INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

bin_PROGRAMS = piklab-hex
OBJ =   $(top_builddir)/src/common/cli/libcli.la \
        $(top_builddir)/src/common/global/libglobal.la $(top_builddir)/src/devices/list/libdevicelistnoui.la \
	$(top_builddir)/src/devices/pic/pic/libpic.la $(top_builddir)/src/devices/pic/base/libpicbase.la \
	$(top_builddir)/src/devices/pic/xml_data/libpicxml.la \
	$(top_builddir)/src/devices/mem24/mem24/libmem24.la $(top_builddir)/src/devices/mem24/base/libmem24base.la \
	$(top_builddir)/src/devices/mem24/xml_data/libmem24xml.la \
	$(top_builddir)/src/devices/base/libdevicebase.la $(top_builddir)/src/common/common/libcommon.la
piklab_hex_DEPENDENCIES = $(OBJ)
piklab_hex_LDADD = $(OBJ) $(LIB_KIO)
piklab_hex_LDFLAGS = $(all_libraries) $(KDE_RPATH)
piklab_hex_SOURCES = main.cpp
