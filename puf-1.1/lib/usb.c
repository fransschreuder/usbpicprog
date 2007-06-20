/*-------------------------------------------------------------------------
  usb.c - Main USB functions

             (c) 2005-2006 Pierre Gaufillet <pierre.gaufillet@magic.fr> 

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

/* $Id: usb.c,v 1.1 2006/11/18 19:52:06 gaufille Exp $ */

#include <pic18fregs.h>
#include "usb.h"

/* Buffer descriptors Table */
volatile BufferDescriptorTable __at (0x400) ep_bdt[32];

#pragma code dummy_function 0x500
void dummy_function(void)
{
    ep_bdt[0].Cnt; // makes the compiler happy !
}
