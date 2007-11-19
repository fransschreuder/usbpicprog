INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO
SUBDIRS = svn_revision

noinst_LTLIBRARIES = libglobal.la
libglobal_la_SOURCES = about.cpp generic_config.cpp log.cpp pfile.cpp \
	process.cpp progress_monitor.cpp purl.cpp xml_data_file.cpp
libglobal_la_LDFLAGS = $(all_libraries)
