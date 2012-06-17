## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src po xml_data examples
ACLOCAL_AMFLAGS =  -I m4



usbpicprogdocdir = ${prefix}/doc/usbpicprog
usbpicprogdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS

usbpicprogdir = ${prefix}/share/usbpicprog
usbpicprog_DATA = \
	index.xml \
	help/usbpicprog.htb \
	firmware.hex

Applicationsdir = ${prefix}/share/applications
Applications_DATA = Usbpicprog.desktop

usbpicprogrulesdir = /lib/udev/rules.d
usbpicprogrules_DATA = 26-microchip.rules

usbpicprogicondir = ${prefix}/share/pixmaps
usbpicprogicon_DATA = icons/usbpicprog.svg

EXTRA_DIST = $(usbpicprogdoc_DATA) $(usbpicprog_DATA) $(Applications_DATA) $(usbpicprogrules_DATA) $(usbpicprogicon_DATA)


# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do \
		if test -f $$specfile; then \
			cp -p $$specfile $(distdir); \
		fi \
	done

