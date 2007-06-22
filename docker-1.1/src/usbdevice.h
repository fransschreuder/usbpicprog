/*-------------------------------------------------------------------------
  usbdevice.h - docker USB device driver

             (c) 2006 Pierre Gaufillet <pierre.gaufillet@magic.fr> 

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
-------------------------------------------------------------------------*/


/* $Id: usbdevice.h,v 1.3 2006/08/24 21:19:21 gaufille Exp $ */

#ifndef USBDEVICE_H
#define USBDEVICE_H

using namespace std;

#include <iostream>
#include <sstream>
#include <vector>
#include <exception>
#include <usb.h>
#include "config.h"
#include "DataBuffer.h"

#define ERASE_FLASH_CMD        0
#define WRITE_FLASH_CMD        1
#define READ_FLASH_CMD         2
#define VALID_APPLICATION_CMD  3
#define RESET_CMD              4
#define SECTION_DESCRIPTOR_CMD 5

/* Sections attributes */
#define FLASH_SECTION_READ     0x01
#define FLASH_SECTION_WRITE    0x02


#define SECTION_DESCRIPTOR_SIZE 7

class USBDevice
{
    public:
        USBDevice(struct usb_device *dev);
        ~USBDevice();
        void erase(void);
        void write(DataBuffer& tab);
        void read(DataBuffer& tab);
        void reset(void);
        void validate_application(void);    
    
    private:
        void erase_block(long address);
        void write_block(long address, DataBuffer& tab);
        void read_block(long address, DataBuffer& tab);
        void get_section_descriptors(void);

        struct usb_dev_handle* dh;

        class Range {
        public:
            Range() {};
            Range(unsigned char f, long b, long e) : flags(f),begin(b),end(e) {};
            ~Range() {};
            long begin;
            long end;
            unsigned char flags;
        };
        
    public:
        typedef vector<Range*> RangeVector;
        
        RangeVector device_mmap;
        
    };

    
#endif // USBDEVICE_H
