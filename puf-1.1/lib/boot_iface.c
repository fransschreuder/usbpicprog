/*---------------------------------------------------------------------------
  boot_iface.c - Boot API

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
---------------------------------------------------------------------------*/

/* $Id: boot_iface.c,v 1.1 2006/11/18 19:52:06 gaufille Exp $ */

#include "common_types.h"

#pragma udata access usb_device_state
uchar __at(0x005f) usb_device_state;
#pragma udata access usb_active_cfg
uchar __at(0x005e) usb_active_cfg; 
#pragma udata access usb_active_alt_setting
uchar __at(0x005d) usb_active_alt_setting; 

#pragma code boot_goto_table 0x1c

// This dummy function is in fact a table of goto exported to
// the application

void boot_goto_table(void) __naked
{
    __asm
 
global _boot_version
global _dispatch_usb_event
global _reset_usb
global _usb_sleep
global _fill_in_buffer

global _ep0_init
global _ep0_in
global _ep0_setup

global _ep1_init
global _ep1_out

global _ep2_init
global _ep2_in

global __mullong
global __mulint
global __gptrget1
global __gptrget2
global __gptrget3
global __gptrget4
global __gptrput1
global __gptrput2
global __gptrput3
global _delay10ktcy
global _delay1ktcy
global _delay10tcy

_boot_version:       res 2
_dispatch_usb_event: res 4
_reset_usb:          res 4
_usb_sleep:          res 4
_fill_in_buffer:     res 4

_ep0_init:           res 4
_ep0_in:             res 4
_ep0_setup:          res 4

_ep1_init:           res 4
_ep1_out:            res 4

_ep2_init:           res 4
_ep2_in:             res 4

__mullong:           res 4
__mulint:            res 4
__gptrget1:          res 4
__gptrget2:          res 4
__gptrget3:          res 4
__gptrget4:          res 4
__gptrput1:          res 4
__gptrput2:          res 4
__gptrput3:          res 4
_delay10ktcy:        res 4
_delay1ktcy:         res 4
_delay10tcy:         res 4

    __endasm;

}
    