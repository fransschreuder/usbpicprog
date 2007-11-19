INCLUDES = -I$(top_srcdir)/src $(all_includes)
METASOURCES = AUTO

noinst_LTLIBRARIES = libpic30.la
libpic30_la_LDFLAGS = $(all_libraries)
libpic30_la_SOURCES = pic30_compile.cpp pic30_config.cpp pic30.cpp \
	pic30_generator.cpp

SUBDIRS = gui