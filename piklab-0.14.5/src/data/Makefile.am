INCLUDES = $(all_includes)
SUBDIRS = app_data likeback

piklabdir = $(kde_datadir)/piklab/icons
piklab_ICON = AUTO

syntaxkatedir = $(kde_datadir)/katepart/syntax
syntaxkate_DATA = asm-pic.xml coff-pic.xml jal-pic.xml coff-c-pic.xml

noinst_PROGRAMS = syntax_xml_generator
syntax_xml_generator_SOURCES = syntax_xml_generator.cpp
syntax_xml_generator_LDADD = $(LIB_QT)
syntax_xml_generator_LDFLAGS = $(all_libraries)

asm-pic.xml coff-pic.xml: syntax_xml_generator
	./syntax_xml_generator
CLEANFILES = asm-pic.xml coff-pic.xml
