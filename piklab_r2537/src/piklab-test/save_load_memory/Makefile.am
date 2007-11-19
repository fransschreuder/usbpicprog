INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_PROGRAMS = save_load_memory_check
OBJ = $(top_builddir)/src/piklab-test/base/libtest.la \
      $(top_builddir)/src/common/cli/libcli.la \
      $(top_builddir)/src/tools/gputils/libgputils.la \
      $(top_builddir)/src/coff/base/libcoff.la \
      $(top_builddir)/src/common/global/libglobal.la $(top_builddir)/src/devices/list/libdevicelistnoui.la \
      $(top_builddir)/src/devices/pic/pic/libpic.la $(top_builddir)/src/devices/pic/base/libpicbase.la \
      $(top_builddir)/src/devices/pic/xml_data/libpicxml.la \
      $(top_builddir)/src/devices/mem24/mem24/libmem24.la $(top_builddir)/src/devices/mem24/base/libmem24base.la \
      $(top_builddir)/src/devices/mem24/xml_data/libmem24xml.la \
      $(top_builddir)/src/devices/base/libdevicebase.la $(top_builddir)/src/common/common/libcommon.la
save_load_memory_check_DEPENDENCIES = $(OBJ)
save_load_memory_check_LDADD = $(OBJ) $(LIB_KIO)
save_load_memory_check_LDFLAGS = $(all_libraries) $(KDE_RPATH)
save_load_memory_check_SOURCES = save_load_memory_check.cpp
