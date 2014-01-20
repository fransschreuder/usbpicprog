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

#include "upp.h"
#include "read_data.h"
#ifdef SDCC
#include <pic18f2550.h>
#else
#include <p18cxxx.h>
#endif
#include "typedefs.h"
#include "io_cfg.h"             // I/O pin mapping
#include "prog.h"
#include "interrupt.h"
#include "prog_lolvl.h"

/**
 This function reads a block of data from the data eeprom of size blocksize into *data
 call this function only once.
 **/
unsigned char read_data( unsigned long address, unsigned char* data,
		char blocksize, char lastblock )
{
	char blockcounter = 0;

	if( lastblock & BLOCKTYPE_FIRST )
		enter_ISCP();
	if( currDevice.read_data )
		currDevice.read_data( address, data, blocksize, lastblock );
	else
		for( blockcounter = 0; blockcounter < blocksize; blockcounter++ ) //fill with zeros
			*(data + blockcounter) = 0;
	if( lastblock & BLOCKTYPE_LAST )
		exit_ISCP();
	return 0;
}

void read_data_dsPIC30( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	unsigned int i, payload;
	char blockcounter = 0;

	//Step 1: Exit the Reset vector.
	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0x040100 ); //GOTO 0x100
	dspic_send_24_bits( 0x000000 ); //NOP
	//Step 2: Initialize TBLPAG and the read pointer (W6) for TBLRD instruction.
	dspic_send_24_bits( 0x2007F0 ); //MOV #0x7F, W0
	dspic_send_24_bits( 0x880190 ); //MOV W0, TBLPAG
	dspic_send_24_bits( 0x2F0006 | ((unsigned long) ((address & 0x0FFF) << 4)) ); //MOV #<SourceAddress15:0>, W6
	for( blockcounter = 0; blockcounter < blocksize; blockcounter += 8 )
	{
		//Step 3: Initialize the write pointer (W7) and store the next four locations of code memory to W0:W5.
		dspic_send_24_bits( 0xEB0380 ); //CLR W7
		dspic_send_24_bits( 0x000000 ); //NOP
		for( i = 0; i < 4; i++ )
		{
			dspic_send_24_bits( 0xBA1BB6 ); //TBLRDL [W6++], [W7++]
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
		}
		//Step 4: Output W0:W5 using the VISI register and REGOUT command.
		for( i = 0; i < 4; i++ )
		{
			dspic_send_24_bits( 0x883C20 | (unsigned long) i ); //MOV W0, VISI
			dspic_send_24_bits( 0x000000 ); //NOP
			payload = dspic_read_16_bits( is3_3V() ); //VISI
			data[blockcounter + (i * 2)] = (unsigned char) payload;
			data[blockcounter + ((i * 2) + 1)] = (unsigned char) ((payload & 0xFF00) >> 8);
			dspic_send_24_bits( 0x000000 ); //NOP
		}
		//Step 5: Reset device internal PC.
		dspic_send_24_bits( 0x040100 ); //GOTO 0x100
		dspic_send_24_bits( 0x000000 ); //NOP
	}
}

void read_data_PIC18( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	char blockcounter = 0;

	pic_send( 4, 0x00, 0x9EA6 ); //BCF EECON1, EEPGD // @ TODO This is necessary only on BLOCKTYPE_FIRST
	pic_send( 4, 0x00, 0x9CA6 ); //BCF EECON1, CFGS
	for( blockcounter = 0; blockcounter < blocksize; blockcounter++ )
	{
		pic_send( 4, 0x00, (0x0E00 | (address + (unsigned int) blockcounter) & 0xFF) ); //MOVLW Addr [7:0]
		pic_send( 4, 0x00, 0x6EA9 ); //MOVWF EEADR
		pic_send( 4, 0x00, (0x0E00 | ((address + (unsigned int) blockcounter) >> 8) & 0xFF) ); //MOVLW Addr [15:8]
		pic_send( 4, 0x00, 0x6EAA ); //MOVWF EEADRH
		pic_send( 4, 0x00, 0x80A6 ); //BSF EECON1, RD
		pic_send( 4, 0x00, 0x50A8 ); //MOVF EEDATA, W, 0
		pic_send( 4, 0x00, 0x6EF5 ); //MOVWF TABLAT
		pic_send( 4, 0x00, 0x0000 ); //Nop
		*(data + blockcounter) = pic_read_byte2( 4, 0x02 );
	}
}

void read_data_PIC18K( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	char blockcounter = 0;

	pic_send( 4, 0x00, 0x9E7F ); //BCF EECON1, EEPGD // @ TODO This is necessary only on BLOCKTYPE_FIRST
	pic_send( 4, 0x00, 0x9C7F ); //BCF EECON1, CFGS
	for( blockcounter = 0; blockcounter < blocksize; blockcounter++ )
	{
		pic_send( 4, 0x00, (0x0E00 | (address + (unsigned int) blockcounter) & 0xFF) ); //MOVLW Addr [7:0]
		pic_send( 4, 0x00, 0x6E62 ); //MOVWF EEADR
		pic_send( 4, 0x00, (0x0E00 | ((address + (unsigned int) blockcounter) >> 8) & 0xFF) ); //MOVLW Addr [15:8]
		pic_send( 4, 0x00, 0x6E63 ); //MOVWF EEADRH
		pic_send( 4, 0x00, 0x807F ); //BSF EECON1, RD
		pic_send( 4, 0x00, 0x5061 ); //MOVF EEDATA, W, 0
		pic_send( 4, 0x00, 0x6EF5 ); //MOVWF TABLAT
		pic_send( 4, 0x00, 0x0000 ); //Nop
		*(data + blockcounter) = pic_read_byte2( 4, 0x02 );
	}
}

void read_data_PIC16( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	unsigned int i;
	char blockcounter = 0;

	if( (lastblock & BLOCKTYPE_FIRST) && (address > 0) )
	{
		data[0] = pic_read_14_bits( 6, 0x05 ); //read first byte
		for( i = 0; i < (unsigned int) address; i++ )
			pic_send_n_bits( 6, 0x06 ); //increment address
	}
	for( blockcounter = 0; blockcounter < blocksize; blockcounter++ )
	{
		data[blockcounter] = (char) pic_read_14_bits( 6, 0x05 ); //read data memory
		pic_send_n_bits( 6, 0x06 ); //increment address
	}
}

