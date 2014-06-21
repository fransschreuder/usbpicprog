dnl ***************************************************************************
dnl *   Copyright (C) 2008 by Frans Schreuder                                 *
dnl *   usbpicprog.sourceforge.net                                            *
dnl *                                                                         *
dnl *   This program is free software; you can redistribute it and/or modify  *
dnl *   it under the terms of the GNU General Public License as published by  *
dnl *   the Free Software Foundation; either version 2 of the License, or     *
dnl *   (at your option) any later version.                                   *
dnl *                                                                         *
dnl *   This program is distributed in the hope that it will be useful,       *
dnl *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
dnl *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
dnl *   GNU General Public License for more details.                          *
dnl *                                                                         *
dnl *   You should have received a copy of the GNU General Public License     *
dnl *   along with this program; if not, write to the                         *
dnl *   Free Software Foundation, Inc.,                                       *
dnl *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
dnl ***************************************************************************




AC_INIT(usbpicprog, 1.0.0)
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE
AC_CONFIG_MACRO_DIR([m4])



dnl ***************************************************************************
dnl Configure options
dnl ***************************************************************************

WX_CONFIG_OPTIONS



dnl ***************************************************************************
dnl Basic checks
dnl ***************************************************************************

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_CPP
AC_PROG_CXX
dnl AM_PROG_LIBTOOL



dnl ***************************************************************************
dnl Internationalization
dnl ***************************************************************************

GETTEXT_PACKAGE=usbpicprog
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])



dnl ***************************************************************************
dnl Checks for our library dependencies
dnl ***************************************************************************

#
# check for wxWidgets
#
WX_CONFIG_CHECK([2.9.0], [has_wxWin=1])
if test "$has_wxWin" != 1; then
    AC_MSG_ERROR([
       wxWidgets must be installed on your system
       but wx-config script couldn't be found.

       Please check that wx-config is in path, the directory
       where wxWidgets libraries are installed (returned by
      'wx-config --libs' command) is in LD_LIBRARY_PATH or
       equivalent variable and wxWidgets version is 2.9.0 or above.
    ])
fi

#
# check for libusb
#
#AC_CHECK_HEADER([libusb-1.0/libusb.h],,
#                [AC_MSG_ERROR([cannot find headers for libusb-1.0])])

PKG_CHECK_MODULES([LIBUSB], [libusb-1.0 >= 1.0.0], have_libusb_1_0=yes, have_libusb_1_0=no)


AC_SUBST(USBPICPROG_CFLAGS)
AC_SUBST(USBPICPROG_LIBS)



dnl ***************************************************************************
dnl Output generation
dnl ***************************************************************************

AC_OUTPUT([
    Makefile
    src/Makefile
    xml_data/Makefile
    examples/Makefile
    examples/blinking_led_pic18f4553/Makefile
    examples/blinking_led_12F_16F/Makefile
    examples/dice_for_12F6XX/Makefile
    examples/dice_for_12F6XX/gerber/Makefile
    examples/dice_for_12F6XX/kicad/Makefile
    examples/dice_for_12F6XX/code/Makefile
    po/Makefile.in
])
