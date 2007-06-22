#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(docker, 1.1, vasco-project@lists.gforge.enseeiht.fr)
AM_INIT_AUTOMAKE([dist-bzip2])
AC_CONFIG_SRCDIR([src/main.cxx])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX


# Checks for libraries.
PKG_CHECK_MODULES(libusb, libusb)

# Checks for header files.
AC_STDC_HEADERS
AC_LANG_PUSH(C++)
AC_CHECK_HEADERS(string map vector exception,,
	AC_MSG_ERROR(missing the required C++ header file $ac_header)
)
AC_LANG_POP(C++)

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

# Generate outputs
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT

