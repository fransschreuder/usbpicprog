#
#             (c) 2006 Pierre Gaufillet <pierre.gaufillet@magic.fr> 
#
#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Lesser General Public
#    License as published by the Free Software Foundation; either
#    version 2.1 of the License, or (at your option) any later version.
#
#    This library is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with this library; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#
#    $Id: Makefile.am,v 1.4 2006/11/25 11:25:21 gaufille Exp $
#

#
# BEWARE : SDCCLIB should not be used. sdcc seems compatible only with gplib !
#

OBJEXT=o
PIC_TYPE = PIC18F4550
sdcc_PIC_TYPE = 18f4550
lkr_PIC_TYPE = 18f4550
TOOLSDIR = $(shell dirname `pwd`)/tools
CC = $(TOOLSDIR)/bin/sdcc
GPLIB = $(TOOLSDIR)/bin/gplib
AM_CFLAGS = -mpic16 -p$(sdcc_PIC_TYPE) $(DEBUG) -I../include
AM_LDFLAGS = 
ODYSSEY = $(TOOLSDIR)/bin/odyssey
DEBUG =--optimize-goto --denable-peeps --obanksel=9 --opt-code-size --optimize-cmp --optimize-df --fstack 
#DEBUG = -D_DEBUG

COMPILE=$(CC) $(AM_CFLAGS) $(CFLAGS) -c


noinst_PROGRAMS = libpuf.lib
libpuf_lib_SOURCES = picon.c boot_iface.c usb.c sequencer.c 

libpuf.lib: picon.o sequencer.o usb.o boot_iface.o
	$(GPLIB) -c $@ $+

CLEANFILES=*.o *.rel *.lst *.cod *.map *.asm

picon.o sequencer.o usb.o boot_iface.o: picon.c boot_iface.c usb.c sequencer.c \
	../include/usb.h ../include/picon.h ../include/sequencer.h  \
	../include/application_iface.h ../include/boot_iface.h
	
