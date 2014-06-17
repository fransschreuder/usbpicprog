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
#include "read_code.h"
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
extern unsigned char ConfigLimitPGDPGC;
/**
 read_program will read program memory, id's and configuration bits
 **/
char read_code( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	char blockcounter = 0;

	if( lastblock & BLOCKTYPE_FIRST )
		enter_ISCP();
	if( currDevice.read_code )
		currDevice.read_code( address, data, blocksize, lastblock );
	else
		for( blockcounter = 0; blockcounter < blocksize; blockcounter++ ) //fill with zeros
			*(data + blockcounter) = 0;

	if( lastblock & BLOCKTYPE_LAST )
		exit_ISCP();
}

void read_code_I2C_EE_1( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	char blockcounter = 0;
	I2C_start();
	I2C_write( 0xA0 | ((int)address>>7)&0x0E ); 			//Device Address + 0=write
	I2C_write( (unsigned char) ((address & 0x00FF)) );	//LSB
	I2C_start();
	I2C_write( 0xA1 | ((int)address>>7)&0x0E ); 			//Device Address + 1=read
	for( blockcounter = 0; blockcounter < blocksize; blockcounter++ )
	{
		data[blockcounter] = I2C_read( 0 );
	}
	I2C_stop();
}

void read_code_I2C_EE_2( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	char blockcounter = 0;

	I2C_start();
	I2C_write( 0xA0 | (address >= 0x10000? 8: 0)); 		//Device Address + 0=write
	I2C_write( (unsigned char) ((address & 0xFF00) >> 8) ); //MSB
	I2C_write( (unsigned char) ((address & 0x00FF)) );	//LSB
	I2C_start();
	I2C_write( 0xA1 | (address >= 0x10000? 8: 0));		//Device Address + 1=read
	for( blockcounter = 0; blockcounter < blocksize-1; blockcounter++ )
	{
		data[blockcounter] = I2C_read( 0 );
	}
	data[blockcounter] = I2C_read( 1 );
	I2C_stop();
}
void read_code_dsPIC30( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	unsigned int i;
	char blockcounter = 0;
	unsigned int payload;
	if( lastblock & BLOCKTYPE_CONFIG )
	{
		if( lastblock & BLOCKTYPE_FIRST )
		{
			//Step 1: Exit the Reset vector.
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x040100 ); //GOTO 0x100
			dspic_send_24_bits( 0x000000 ); //NOP
			//Step 2: Initialize TBLPAG, and the read pointer (W6) and the write pointer (W7) for TBLRD instruction.
			dspic_send_24_bits( 0x200000 | ((address & 0xFF0000) >> 12) ); //MOV #0xF8, W0
			dspic_send_24_bits( 0x880190 ); //MOV W0, TBLPAG
			//dspic_send_24_bits(0x200006|((address&0x00FFFF)<<4));	//MOV #<SourceAddress15:0>, W6
			//ignore the lower 16 bits off the address, they are always 0.
			dspic_send_24_bits( 0xEB0300 ); //CLR W6
			dspic_send_24_bits( 0xEB0380 ); //CLR W7
			dspic_send_24_bits( 0x000000 ); //NOP
		}
		for( blockcounter = 0; blockcounter < blocksize; blockcounter += 2 )
		{
			//Step 3: Read the Configuration register and write it to the VISI register (located at 0x784).
			dspic_send_24_bits( 0xBA0BB6 ); //TBLRDL [W6++], [W7]
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x883C20 ); //MOV W0, VISI
			dspic_send_24_bits( 0x000000 ); //NOP
			//Step 4: Output the VISI register using the REGOUT command.
			payload = dspic_read_16_bits( is3_3V() ); //read <VISI>
			data[blockcounter] = (unsigned char) payload;
			data[blockcounter + 1] = (unsigned char) ((payload & 0xFF00) >> 8);
			dspic_send_24_bits( 0x000000 ); //NOP
			//Step 5: Reset device internal PC.
			dspic_send_24_bits( 0x040100 ); //GOTO 0x100
			dspic_send_24_bits( 0x000000 ); //NOP
		}
		//Step 6: Repeat steps 3-5 six times to read all of configuration memory.
	}
	else
	{
		//Step 1: Exit the Reset vector.
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x040100 ); //GOTO 0x100
		dspic_send_24_bits( 0x000000 ); //NOP
		//Step 2: Initialize TBLPAG and the read pointer (W6) for TBLRD instruction.
		dspic_send_24_bits( 0x200000 | (((((address) * 2) / 3) & 0xFF0000) >> 12) ); //MOV #<SourceAddress23:16>, W0
		dspic_send_24_bits( 0x880190 ); //MOV W0, TBLPAG
		dspic_send_24_bits( 0x200006 | (((((address) * 2) / 3) & 0x00FFFF) << 4) ); //MOV #<SourceAddress15:0>, W6
		//Step 3: Initialize the write pointer (W7) and store the next four locations of code memory to W0:W5.
		dspic_send_24_bits( 0xEB0380 ); //CLR W7
		//dspic_send_24_bits(0x000000);	//NOP
		dspic_send_24_bits( 0xBA1B96 ); //TBLRDL [W6], [W7++]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBADBB6 ); //TBLRDH.B [W6++], [W7++]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBADBD6 ); //TBLRDH.B [++W6], [W7++]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBA1BB6 ); //TBLRDL [W6++], [W7++]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBA1B96 ); //TBLRDL [W6], [W7++]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBADBB6 ); //TBLRDH.B [W6++], [W7++]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBADBD6 ); //TBLRDH.B [++W6], [W7++]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBA0BB6 ); //TBLRDL [W6++], [W7]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		//Step 4: Output W0:W5 using the VISI register and REGOUT command.
		for( i = 0; i < 6; i++ )
		{
			dspic_send_24_bits( 0x883C20 | (unsigned long) i ); //MOV W0, VISI
			dspic_send_24_bits( 0x000000 ); //NOP
			payload = dspic_read_16_bits( is3_3V() ); //Clock out contents of VISI register
			data[blockcounter + i * 2] = (unsigned char) payload & 0xFF;
			data[blockcounter + i * 2 + 1] = (unsigned char) ((payload & 0xFF00) >> 8);
			dspic_send_24_bits( 0x000000 ); //NOP
		}
		//Step 5: Reset the device internal PC.
		dspic_send_24_bits( 0x040100 ); //GOTO 0x100
		dspic_send_24_bits( 0x000000 ); //NOP
	}
}
void read_code_PIC18( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	char blockcounter = 0;

	set_address_P18( address );
	for( blockcounter = 0; blockcounter < blocksize; blockcounter++ )
	{
		*(data + blockcounter) = pic_read_byte2( 4, 0x09 );
	}
}
void read_code_P16F18XX( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	unsigned int i;
	char blockcounter = 0;
	unsigned int payload;

	if( lastblock & BLOCKTYPE_FIRST )
	{
		pic_send_n_bits(6,0x16); //reset address;
		if( lastblock & BLOCKTYPE_CONFIG )
		{
			pic_send_14_bits( 6, 0x00, 0x0000 );  //Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000 (or 0x8000).
			address &= 0xFF;
		}
		else
			pic_read_14_bits( 6, 0x04 ); //read code memory	//FIXME: what does this do? - seems to be necessary to make things work

		for( i = 0; i < (unsigned int) address; i++ )
			pic_send_n_bits( 6, 0x06 ); //increment address
	}
	for( blockcounter = 0; blockcounter < blocksize; blockcounter += 2 )
	{
		payload = pic_read_14_bits( 6, 0x04 ); //read code memory
		data[blockcounter + 1] = (char) (payload >> 8);
		data[blockcounter] = (char) payload;
		pic_send_n_bits( 6, 0x06 ); //increment address
	}
}
void read_code_PIC16( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	unsigned int i;
	char blockcounter = 0;
	unsigned int payload;

	if( lastblock & BLOCKTYPE_FIRST )
	{
		exit_ISCP();
		enter_ISCP();
		if( lastblock & BLOCKTYPE_CONFIG )
		{

			pic_send_14_bits( 6, 0x00, 0x0000 );  //Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000 (or 0x8000).
			address &= 0xFF;
		}
		else
			pic_read_14_bits( 6, 0x04 ); //read code memory	//FIXME: what does this do? - seems to be necessary to make things work

		for( i = 0; i < (unsigned int) address; i++ )
			pic_send_n_bits( 6, 0x06 ); //increment address
	}
	for( blockcounter = 0; blockcounter < blocksize; blockcounter += 2 )
	{
		payload = pic_read_14_bits( 6, 0x04 ); //read code memory
		data[blockcounter + 1] = (char) (payload >> 8);
		data[blockcounter] = (char) payload;
		pic_send_n_bits( 6, 0x06 ); //increment address
	}
}

void read_code_PIC10( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	unsigned int i;
	char blockcounter = 0;
	unsigned int payload;

	if( lastblock & BLOCKTYPE_CONFIG )		// address >= configAddress )
	{
		payload = pic_read_14_bits( 6, 0x04 ); //read config memory
		data[1] = (char) (payload >> 8);
		data[0] = (char) payload;
	}
	else
	{
		if( lastblock & BLOCKTYPE_FIRST )
		{
			//pic_read_14_bits(6,0x04); //read code memory
			pic_send_n_bits( 6, 0x06 ); //increment address
			//			for(i=0;i<10;i++);
			//pic_read_14_bits(6,0x04); //read code memory
			//			for(i=0;i<10;i++);
			for( i = 0; i < (unsigned int) address; i++ )
			{
				pic_send_n_bits( 6, 0x06 ); //increment address
				//				for(i=0;i<10;i++);
			}
		}
		for( blockcounter = 0; blockcounter < blocksize; blockcounter += 2 )
		{
			payload = pic_read_14_bits( 6, 0x04 ); //read code memory
			//			for(i=0;i<10;i++);
			data[blockcounter + 1] = (char) (payload >> 8);
			data[blockcounter] = (char) payload;
			pic_send_n_bits( 6, 0x06 ); //increment address
			//			for(i=0;i<10;i++);
		}
	}
}
