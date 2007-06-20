/*-------------------------------------------------------------------------
  boot_iface.h - Boot API

             (c) 2006 Pierre Gaufillet <pierre.gaufillet@magic.fr> 

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
-------------------------------------------------------------------------*/

/* $Id: boot_iface.h,v 1.2 2006/11/18 19:52:06 gaufille Exp $ */

#ifndef BOOT_H_
#define BOOT_H_

#include "common_types.h"

void boot_goto_table(void);

void dispatch_usb_event(void);
void reset_usb(void);
void usb_sleep(void);
void fill_in_buffer(uchar EPnum,
                    uchar **source,
                    uint buffer_size, 
                    uint *nb_byte);
                    
void ep0_init(void);
void ep0_in(void);
void ep0_setup(void);

void ep1_init(void);
void ep1_out(void);

void ep2_init(void);
void ep2_in(void);

extern uchar __at(0x005f) usb_device_state;
extern uchar __at(0x005e) usb_active_cfg;
extern uchar __at(0x005d) usb_active_alt_setting;



#endif /*APPLICATION_H_*/
