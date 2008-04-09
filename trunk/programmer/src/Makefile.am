bin_PROGRAMS = programmer
programmer_SOURCES = main.cpp read_hexfile.cpp 	pictype.cpp upp_gui.cpp

# set the include path found by configure
INCLUDES = $(all_includes)

# the library search path.
programmer_LDFLAGS = $(all_libraries)
noinst_HEADERS = read_hexfile.h 	pictype.h upp_gui.h

AM_CXXFLAGS = `pkg-config gtkmm-2.4 --cflags --libs`
