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
#ifdef TEST
#define clock_delay()
#else
#define clock_delay() Nop();Nop()
#endif
//#define I2C_delay() Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop();Nop()
void I2C_delay();


/**
    Sets or clears the VDD and VPP voltages.
*/
void set_vdd_vpp(PICTYPE pictype, PICFAMILY picfamily, char level);
void enter_ISCP( void );

/**
    Sets the address pointer to a certain address location.
*/
void set_address(PICFAMILY picfamily, unsigned long address);
void set_address_P16( unsigned long address );
void set_address_P18( unsigned long address );


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
unsigned int dspic_read_16_bits(unsigned char isLV);


/**
  gives a 24 bit instruction to a dsPIC device
*/
void dspic_send_24_bits(unsigned long payload);


/**
I2C Start
*/
void I2C_start(void);

/**
I2C Stop
*/
void I2C_stop(void);


/** 
returns ack
*/
unsigned char I2C_write(unsigned char d);


unsigned char I2C_read(unsigned char ack);

void enter_ISCP_simple( void );
void enter_ISCP_P16_Vpp( void );
void enter_ISCP_dsPIC30( void );
void enter_ISCP_PIC18J( void );
void enter_ISCP_PIC18K( void );
void enter_ISCP_PIC24( void ) ;
void enter_ISCP_PIC24E( void ) ;
void enter_ISCP_PIC24K( void ) ;
void enter_ISCP_I2C_EE( void );
void exit_ISCP( void );

//	lastblock bits
//#define FIRST_BLOCK	0x01
//#define LAST_BLOCK	0x02
//#define CONFIG_BLOCK	0x04
#define BLOCKTYPE_FIRST	 0x01
#define BLOCKTYPE_LAST	 0x02
#define BLOCKTYPE_CONFIG 0x04

#define TABLE
#ifdef TEST
#include "test.h"	// test harness
#endif
#endif// PROG_LOLVL_H
