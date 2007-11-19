INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_PROGRAMS = gputils_config_generator_check gputils_template_generator_check \
                  sdcc_config_generator_check

OBJ = $(top_builddir)/src/piklab-test/base/libtest.la \
      $(top_builddir)/src/common/cli/libcli.la \
      $(top_builddir)/src/tools/sdcc/libsdcc.la $(top_builddir)/src/tools/gputils/libgputils.la \
      $(top_builddir)/src/coff/base/libcoff.la \
      $(top_builddir)/src/common/global/libglobal.la $(top_builddir)/src/devices/list/libdevicelistnoui.la \
      $(top_builddir)/src/devices/pic/pic/libpic.la $(top_builddir)/src/devices/pic/base/libpicbase.la \
      $(top_builddir)/src/devices/pic/xml_data/libpicxml.la \
      $(top_builddir)/src/devices/mem24/mem24/libmem24.la $(top_builddir)/src/devices/mem24/base/libmem24base.la \
      $(top_builddir)/src/devices/mem24/xml_data/libmem24xml.la \
      $(top_builddir)/src/devices/base/libdevicebase.la $(top_builddir)/src/common/common/libcommon.la

gputils_config_generator_check_DEPENDENCIES = $(OBJ)
gputils_config_generator_check_LDADD = $(OBJ) $(LIB_KIO)
gputils_config_generator_check_LDFLAGS = $(all_libraries) $(KDE_RPATH)
gputils_config_generator_check_SOURCES = gputils_config_generator_check.cpp

gputils_template_generator_check_DEPENDENCIES = $(OBJ)
gputils_template_generator_check_LDADD = $(OBJ) $(LIB_KIO)
gputils_template_generator_check_LDFLAGS = $(all_libraries) $(KDE_RPATH)
gputils_template_generator_check_SOURCES = gputils_template_generator_check.cpp

sdcc_config_generator_check_DEPENDENCIES = $(OBJ)
sdcc_config_generator_check_LDADD = $(OBJ) $(LIB_KIO)
sdcc_config_generator_check_LDFLAGS = $(all_libraries) $(KDE_RPATH)
sdcc_config_generator_check_SOURCES = sdcc_config_generator_check.cpp
