# This file is part of fsusb_demo
#
# fsusb_demo is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# fsusb_demo is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with fsusb_picdem; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301, USA

# For Windows, please put usb.h and MingW version of libusb.a into
# the MingW include and lib directory respectively.
# For Linux only
#LIBUSB = /usr/lib

OPTS=-Wall -D_GNU_SOURCE
OBJS=fsusb_demo.o

CFLAGS=$(OPTS) -I$(LIBUSB)/include
LDFLAGS=-L$(LIBUSB)/lib -lusb

# Needed for static linking under OS X:
# LDFLAGS=-lusb -lIOKit -framework CoreFoundation

all: fsusb_demo

fsusb_demo: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

clean:
	-rm fsusb_demo *.o
