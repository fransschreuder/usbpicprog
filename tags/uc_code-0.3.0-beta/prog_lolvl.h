/**************************************************************************
*   Copyright (C) 2008 by Frans Schreuder                                 *
*   usbpicprog.sourceforge.net                                            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
**************************************************************************/


#ifndef PROG_LOLVL_H
#define PROG_LOLVL_H


#include "prog.h"


/**
    Would one Nop() cycle be enough delay for all PIC's?
    It works for PIC18F2550
*/
#define clock_delay() Nop()



/**
    Sets or clears the VDD and VPP voltages.
*/
void set_vdd_vpp(PICTYPE pictype, PICFAMILY picfamily, char level);

/**
    Sets the address pointer to a certain address location.
*/
void set_address(PICFAMILY picfamily, unsigned long address);

/**
    Reads 8 bits from a pic device with a given cmd_size bits command 
    (only the 2nd byte is read).
*/
char pic_read_byte2(char cmd_size, char command);

/**
    Reads 14 bits from a pic16 device (0 payload 0).
*/
unsigned int pic_read_14_bits(char cmd_size, char command);

/**
    Writes a n-bit command.
*/
void pic_send_n_bits(char cmd_size, char command);

/**
    Writes a 14 bit word with a start and a stop bit (16F devices).
*/
void pic_send_word_14_bits(unsigned int payload);

/**
    Writes 16 bits to the PIC (18F devices).
*/
void pic_send_word(unsigned int payload);

/**
    Writes a cmd_size bit command + 14 bit payload to a pic16 device.
*/
void pic_send_14_bits(char cmd_size, char command, unsigned int payload);

/**
    Writes a cmd_size bit command + 16 bit payload to a pic18 device.
*/
void pic_send(char cmd_size, char command, unsigned int payload);

/**
  reads a 16 bit word from a dsPic device using normal ICSP
*/
unsigned int dspic_read_16_bits(void);


/**
  gives a 24 bit instruction to a dsPIC device
*/
void dspic_send_24_bits(unsigned long payload);

/**
   Writes a 16 bit word to a dsPic device
*/
//void dspic_send(unsigned int payload);

/**
  Reads a 16 bit word from a dsPic device
*/
//unsigned int dspic_read(void);
		


#endif // PROG_LOLVL_H
