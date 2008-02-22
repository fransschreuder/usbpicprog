INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_PROGRAMS = misc_check
OBJ = $(top_builddir)/src/piklab-test/base/libtest.la $(top_builddir)/src/common/cli/libcli.la \
      $(top_builddir)/src/common/global/libglobal.la $(top_builddir)/src/common/common/libcommon.la
misc_check_DEPENDENCIES = $(OBJ)
misc_check_LDADD = $(OBJ) $(LIB_KIO)
misc_check_LDFLAGS = $(all_libraries) $(KDE_RPATH)
misc_check_SOURCES = misc_check.cpp
