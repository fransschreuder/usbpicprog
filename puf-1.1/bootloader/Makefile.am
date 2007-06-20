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
#    $Id: Makefile.am,v 1.5 2006/11/25 11:25:21 gaufille Exp $
#

OBJEXT=o
PIC_TYPE = PIC18F4550
sdcc_PIC_TYPE = 18f4550
lkr_PIC_TYPE = 18f4550
TOOLSDIR = $(shell dirname `pwd`)/tools
CC = $(TOOLSDIR)/bin/sdcc
AM_CFLAGS = -mpic16 -p$(sdcc_PIC_TYPE) $(DEBUG) 
LNK = $(TOOLSDIR)/bin/sdcc
AM_LDFLAGS = 
ODYSSEY = $(TOOLSDIR)/bin/odyssey
DEBUG =--optimize-goto --denable-peeps --obanksel=9 --opt-code-size --optimize-cmp --optimize-df --fstack 
#DEBUG = -D_DEBUG

COMPILE=$(CC) -mpic16 $(DEBUG) -p$(sdcc_PIC_TYPE) -Iinclude -c
LINK=$(LNK) -o $@ $(DEBUG) $(LDFLAGS) --use-crt=crt0.o -Wl-s$(lkr_PIC_TYPE).lkr,-m \
	-mpic16 -p$(sdcc_PIC_TYPE) -llibio$(sdcc_PIC_TYPE).lib -llibc18f.lib


bin_PROGRAMS = bootloader.hex
bootloader_hex_SOURCES = application_iface.c boot_main.c boot_iface.c \
                         usb.c usb_descriptors.c ep0.c ep1.c ep2.c flash.c \
                         config.c interrupt_iface.c usb.h usb_descriptors.h ep0.h \
                         ep1.h ep2.h application_iface.h \
                         usb_std_req.h common_types.h \
                         debug.h flash.h boot_iface.h config.h

EXTRA_DIST=$(lkr_PIC_TYPE).lkr

CLEANFILES=*.o *.rel *.lst *.cod *.map *.asm

prog: bootloader.hex
	(echo "Blanking...";\
	$(ODYSSEY) $(PIC_TYPE) erase) && \
	(echo "Writing...";\
	$(ODYSSEY) $(PIC_TYPE) write $<)

application_iface.o main.o usb_descriptors.o boot_iface.o : usb_descriptors.h application_iface.h common_types.h debug.h boot_iface.h
