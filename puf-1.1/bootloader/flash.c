/*-------------------------------------------------------------------------
  flash.c - Program memory flashing process

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

/* $Id: flash.c,v 1.5 2006/04/17 20:36:15 gaufille Exp $ */

#include "config.h"
#include "common_types.h"
#include <pic18fregs.h>
#include "debug.h"
#include "flash.h"

uint ep2_num_bytes_to_send;
uchar *ep2_source_data;

void write_block(__data uchar *src)
{
    static uchar i;
    debug("write_block\n");

    __asm
    
    tblrd*-

    __endasm;
    
    for(i = 0; i < 32; i++)
    {
        TABLAT = *src;
        debug2("%uhx ", TABLAT);
        src++;
        __asm
        
        tblwt+*

        __endasm;
    } 
    debug("\n");
    // Beware : when WR is set, TBLPTR have to be inside the 32 byte range to be written
    // so it should be decremented...
    
    __asm
    
    bsf     _EECON1, 7 // EEPGD
    bcf     _EECON1, 6 // CFGS
    bsf     _EECON1, 2 // WREN
    bcf     _INTCON, 7 // GIE
    
    // required erase sequence
    movlw   0x55
    movwf   _EECON2
    movlw   0xaa
    movwf   _EECON2
    bsf     _EECON1, 1 // WR

    // stall here for 2ms

    __endasm;
    debug("write finished\n");
}

void erase_block()
{
    // Erase a 64 bytes block
    __asm
    // address set by the caller 

    // Point to flash program memory
    bsf     _EECON1, 7 // EEPGD
    bcf     _EECON1, 6 // CFGS
    bsf     _EECON1, 2 // WREN
    bsf     _EECON1, 4 // FREE
    bcf     _INTCON, 7 // GIE
    
    // required erase sequence
    movlw   0x55
    movwf   _EECON2
    movlw   0xaa
    movwf   _EECON2
    bsf     _EECON1, 1 // WR
    
    // stall here for 2ms
    
    __endasm;
}
