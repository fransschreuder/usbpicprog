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
#include "write_code.h"
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
 address points to the first byte of the block
 data contains the data MSB0, LSB0, MSB1, LSB1, etc...
 blocksize is the block syze in BYTES
 */
char write_code( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{

	if( lastblock & BLOCKTYPE_FIRST )
		enter_ISCP();
	if( currDevice.write_code )
		currDevice.write_code( address, data, blocksize, lastblock );
	else
	{
		exit_ISCP();
		return 3;
	}
	if( lastblock & BLOCKTYPE_LAST )
	{
		exit_ISCP();
		return 1; //ok
	}
	else
	{
		return 2; //ask for next block
	}
}

void write_code_EE_1( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
//FIXME: This currently dowsn't work as the app sends too many bytes for the write cache (2-16bytes depending on model)

	unsigned char i, tries;
	char blockcounter;

	tries = 0;
restart:
	I2C_start();
	I2C_write( 0xA0 | ((int)address>>7)&0x0E );			//Device Address + 0=write

	I2C_write( (unsigned char) ((address & 0x00FF)) ); 	//LSB

	for( blockcounter = 0; blockcounter < blocksize; blockcounter++ )
	{
		i = (unsigned int) I2C_write( data[blockcounter] );
		if( i == 1 )   // received a NACK
		{
			for( i = 0; i < 4; i++ ) {
				setLeds( 7);
				DelayMs(125);
				setLeds(0);
				DelayMs( 125 );
			}
			I2C_stop();
			DelayMs(30);
			if( tries < 2 )
				goto restart;
			return; // what else to do??
		}
	}
	I2C_stop();
	DelayMs( 10 );
}

void write_code_EE_2( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	unsigned char i, tries;
	char blockcounter;

	tries = 0;
restart:
	I2C_start();
	I2C_write( 0xA0 | (address >= 0x10000? 8: 0) ); 	//Device Address + 0=write

	I2C_write( (unsigned char) ((address & 0xFF00) >> 8) ); //MSB
	I2C_write( (unsigned char) ((address & 0x00FF)) );	//LSB

	for( blockcounter = 0; blockcounter < blocksize; blockcounter++ )
	{
		i = (unsigned int) I2C_write( data[blockcounter] );
		if( i == 1 )   // received a NACK
		{
			for( i = 0; i < 4; i++ ) {
				setLeds( 7);
				DelayMs(125);
				setLeds(0);
				DelayMs( 125 );
			}
			I2C_stop();
			DelayMs(30);
			if( tries < 2 )
				goto restart;
			return; // what else to do??
		}
	}
	I2C_stop();
	DelayMs( 30 );
}
void write_code_dsP30F( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	unsigned int i;
	char blockcounter;

	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0x000000 ); //NOP
	//Step 1: Exit the Reset vector.
	dspic_send_24_bits( 0x040100 ); //GOTO 0x100
	dspic_send_24_bits( 0x040100 ); //GOTO 0x100
	dspic_send_24_bits( 0x000000 ); //NOP
	//Step 2: Set the NVMCON to program 32 instruction words.
	dspic_send_24_bits( 0x24001A ); //MOV #0x4001, W10
	dspic_send_24_bits( 0x883B0A ); //MOV W10, NVMCON
	for( blockcounter = 0; blockcounter < blocksize; blockcounter += 12 )
	{
		//Step 3: Initialize the write pointer (W7) for TBLWT instruction.
		dspic_send_24_bits( 0x200000 | (((((blockcounter + address) * 2) / 3) & 0xFF0000) >> 12) ); //MOV #<DestinationAddress23:16>, W0
		dspic_send_24_bits( 0x880190 ); //MOV W0, TBLPAG
		dspic_send_24_bits( 0x200007 | (((((blockcounter + address) * 2) / 3) & 0x00FFFF) << 4) ); //MOV #<DestinationAddress15:0>, W7
		//Step 4: Initialize the read pointer (W6) and load W0:W5 with the next 4 instruction words to program.
		for( i = 0; i < 6; i++ )
		{
			dspic_send_24_bits( 0x200000 | (((unsigned long) data[blockcounter + (i * 2)]) << 4)
					| (((unsigned long) data[blockcounter + (i * 2) + 1]) << 12)
					| ((unsigned long) i) );
			/**
			 MOV #<LSW0>, W0
			 MOV #<MSB1:MSB0>, W1
			 MOV #<LSW1>, W2
			 MOV #<LSW2>, W3
			 MOV #<MSB3:MSB2>, W4
			 MOV #<LSW3>, W5
			 */
		}
		//Step 5: Set the read pointer (W6) and load the (next set of) write latches.
		dspic_send_24_bits( 0xEB0300 ); //CLR W6
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBB0BB6 ); //TBLWTL [W6++], [W7]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBBDBB6 ); //TBLWTH.B [W6++], [W7++]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBBEBB6 ); //TBLWTH.B [W6++], [++W7]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBB1BB6 ); //TBLWTL [W6++], [W7++]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBB0BB6 ); //TBLWTL [W6++], [W7]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBBDBB6 ); //TBLWTH.B [W6++], [W7++]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBBEBB6 ); //TBLWTH.B [W6++], [++W7]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBB1BB6 ); //TBLWTL [W6++], [W7++]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
	}//Step 6: Repeat steps 3-5 eight times to load the write latches for 32 instructions.
	//if((address%96)==64)
	//{
	//Step 7: Unlock the NVMCON for writing.
	dspic_send_24_bits( 0x200558 ); //MOV #0x55, W8
	dspic_send_24_bits( 0x883B38 ); //MOV W8, NVMKEY
	dspic_send_24_bits( 0x200AA9 ); //MOV #0xAA, W9
	dspic_send_24_bits( 0x883B39 ); //MOV W9, NVMKEY
	//Step 8: Initiate the write cycle.
	dspic_send_24_bits( 0xA8E761 ); //BSET NVMCON, #WR
	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0x000000 ); //NOP
	DelayMs( 3 ); //Externally time 2 msec
	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0xA9E761 ); //BCLR NVMCON, #WR
	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0x000000 ); //NOP
	//Step 9: Reset device internal PC.
	dspic_send_24_bits( 0x040100 ); //GOTO 0x100
	dspic_send_24_bits( 0x000000 ); //NOP
	//}
}
void write_code_P18F( unsigned long address, unsigned char* data, char blocksize, char lastblock,char writeSize, char performReads, char setWREN, char clearCFGS, char KseriesRegs, char enableMultiPanel)
{
	unsigned int i;
	char blockcounter, localWriteSize;

	if( lastblock & BLOCKTYPE_FIRST )
	{
		if(KseriesRegs)
		{
			pic_send( 4, 0x00, 0x8E7F ); //BSF EECON1, EEPGD
			pic_send( 4, 0x00, 0x9C7F ); //BSF EECON1, CFGS
			pic_send( 4, 0x00, 0x847F ); //BSF EECON1, WREN
		}
		else
		{
			pic_send( 4, 0x00, 0x8EA6 ); //BSF EECON1, EEPGD
			if(clearCFGS)
			{
				pic_send( 4, 0x00, 0x8CA6 ); //BSF EECON1, CFGS
				if(setWREN)pic_send( 4, 0x00, 0x84A6 ); //BSF EECON1, WREN
				if(enableMultiPanel)
				{
					set_address_P18( 0x3C0006 );
					if(enableMultiPanel==1)
						pic_send( 4, 0x0c, 0x0040);
					else
						pic_send( 4, 0x0c, 0x0000); //send 2 to explicitly disable multi panel writes
				}
				pic_send( 4, 0x00, 0x8EA6 ); //BSF EECON1, EEPGD
				pic_send( 4, 0x00, 0x9CA6 ); //BCF EECON1, CFGS
			}
			else
			{
				pic_send( 4, 0x00, 0x9CA6 ); //BCF EECON1, CFGS
				if(setWREN)pic_send( 4, 0x00, 0x84A6 ); //BSF EECON1, WREN
			}
		}
		
	}
	if((writeSize<64)||(writeSize==64&&(address&0x20==0))||(writeSize==128&&(address&0x60==0)))
		set_address_P18( address );
	localWriteSize = writeSize;
	if(writeSize>32)localWriteSize = 32;
	for( blockcounter = 0; blockcounter < (blocksize); blockcounter += localWriteSize ) //blocks of writeSize bytes
	{
		for( i = 0; i < localWriteSize-2; i += 2 )
		{
			//write 2 bytes and post increment by 2
			//				MSB				LSB
			pic_send( 4, 0x0D, ((unsigned int) *(data + blockcounter + i)) | (((unsigned int) *(data + 1
					+ blockcounter + i)) << 8) );
		}
		//write last 2 bytes of the block and start programming
		if(((writeSize==64&&((address&0x20)==0))||
		   (writeSize==128&&((address&0x60)!=0x60))))
		{
			pic_send( 4, 0x0D, ((unsigned int) *(data + blockcounter + i)) | (((unsigned int) *(data + 1
					+ blockcounter + i)) << 8) );
			if( lastblock & BLOCKTYPE_LAST ) //if the last block is the first half of 64 bytes, it needs to be finished with a dummy block to finish.
			{
				for( blockcounter = 0; blockcounter < writeSize-2; blockcounter += 2 )
					pic_send( 4, 0x0D, 0xFFFF );
				pic_send( 4, 0x0F, 0xFFFF );
				pic_send_n_bits( 3, 0 );
				PGChigh(); //hold PGC high for P9 and low for P10
				DelayMs( 10 );
				PGClow();
				DelayMs( 1 );
				pic_send_word( 0x0000 );
			}
		}
		else
		{
			pic_send( 4, 0x0F, ((unsigned int) *(data + blockcounter + i)) | (((unsigned int) *(data + 1
					+ blockcounter + i)) << 8) );
			pic_send_n_bits( 3, 0 );
			PGChigh(); //hold PGC high for P9 and low for P10
			DelayMs( P9 );
			PGClow();
			DelayMs( P10 );
			pic_send_word( 0x0000 );
			if(performReads)
			{
				pic_read_byte2( 4, 0x09 ); //perform 2 reads to increase the address by 2
				pic_read_byte2( 4, 0x09 );
			}
		}
		
		
	}
}
void write_code_P18F872X( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P18F( address, data, blocksize, lastblock, 64, 0, 1, 1, 0, 0);
}
void write_code_P18F6XKXX( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P18F( address, data, blocksize, lastblock, 64, 0, 1, 1, 1, 0);
}
void write_code_P18F67KXX( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P18F( address, data, blocksize, lastblock, 128, 0, 1, 1, 1, 0);
}
void write_code_P18F2XXX( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P18F(address, data, blocksize, lastblock, 32, 0, 0, 0, 0, 0);
}
void write_code_P18F4XK22( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P18F(address, data, blocksize, lastblock, 32, 0, 1, 0, 0, 0);
}
void write_code_P18F14K22( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P18F(address, data, blocksize, lastblock, 16, 1, 1, 0, 0, 0);
}
void write_code_P18F13K22( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P18F(address, data, blocksize, lastblock, 8, 1, 1, 0, 0, 0);
}
void write_code_P18FXX20( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P18F(address, data, blocksize, lastblock, 8, 1, 1, 1, 0, 1);
}
void write_code_P18FX220( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P18F(address, data, blocksize, lastblock, 8, 1, 0, 0, 0, 0);
}
void write_code_P18FXX31( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P18F(address, data, blocksize, lastblock, 8, 1, 0, 1, 0, 2);
}
void write_code_P18F45J10( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{

	write_code_P18F(address, data, blocksize, lastblock, 64, 0, 1, 0, 0, 0);
}

void write_code_P16(unsigned long address, unsigned char* data, char blocksize, char lastblock, unsigned char overflowFirst, unsigned char writeSize, unsigned char beginProg, unsigned char endProg, unsigned char PDelay, unsigned char EndDelay)
{
	unsigned int i;
	char blockcounter;
	if( (lastblock & BLOCKTYPE_FIRST) )
	{
		if(overflowFirst)
			pic_send_n_bits( 6, 0x06 );//increment address to go from 1FF / 3FF to 0
		set_address_P16( address ); //set the initial address
	}
	//2 word programming
	for( blockcounter = 0; blockcounter < blocksize; blockcounter += (writeSize) ) //writeSize/2 words of data = writeSize bytes
	{
		for( i = 0; i < (writeSize); i += 2 )
		{
			pic_send_14_bits( 6, 0x02, (((unsigned int) data[blockcounter + i])) | //MSB
					(((unsigned int) data[blockcounter + i + 1]) << 8) );//LSB
			if( i < (writeSize) )
				pic_send_n_bits( 6, 0x06 ); //increment address
		}
		pic_send_n_bits( 6, beginProg ); //begin programming only, externally timed
		DelayMs( PDelay );
		if(endProg) pic_send_n_bits( 6, endProg ); //end programming
		DelayMs( EndDelay );
		pic_send_n_bits( 6, 0x06 ); //increment address
	}
}


void write_code_P16F18XX( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	if( lastblock & BLOCKTYPE_FIRST )
	{
		pic_send_n_bits( 6, 0x16 ); //reset address
	}
	write_code_P16(address, data, blocksize, lastblock, 0, 2, 0x08, 0, Tprog, 10);
}

void write_code_P16F84A( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P16(address, data, blocksize, lastblock, 0, 2, 0x08, 0, Tprog, 10);
}
void write_code_P12F61X( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P16(address, data, blocksize, lastblock, 0, 2, 0x18, 0x0A, Tprog, 10);
}
void write_code_P16C6XX( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{

	char blockcounter;
	char i;
	unsigned int payload;
	if( (lastblock & BLOCKTYPE_FIRST) && (address > 0) )
	{
		set_address_P16( address ); //set the initial address
	}
	for( blockcounter = 0; blockcounter < blocksize; blockcounter += 2 )
	{
		for(i=0;i<25;i++)
		{
			payload = (((unsigned int) data[blockcounter])) | //MSB
				(((unsigned int) data[blockcounter + 1]) << 8);
			pic_send_14_bits( 6, 0x02,  payload);//LSB
		
			pic_send_n_bits( 6, 0x08 ); //begin programming
			DelayUs( 100 );
			pic_send_n_bits( 6, 0x0E ); //end programming
			if(pic_read_14_bits( 6, 0x04 )==payload&&i<22)i=22; //correct? do 3 more programming cycles.
		}
		pic_send_n_bits( 6, 0x06 ); //increment address
	}
}
void write_code_P16F72( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P16(address, data, blocksize, lastblock, 0, 4, 0x08, 0x0E, 2, 0);
}
void write_code_P16F785( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P16(address, data, blocksize, lastblock, 0, 8, 0x18, 0x0A, Tprog, 0);
}
void write_code_P16F716( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P16(address, data, blocksize, lastblock, 0, 8, 0x18, 0x0E, Tprog, 0);
}
void write_code_P12F6XX( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P16(address, data, blocksize, lastblock, 0, 8, 0x08, 0, Tprog, 0);
}
void write_code_P16F87( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P16(address, data, blocksize, lastblock, 0, 8, 0x18, 0x17, 2, 0);
}
void write_code_P16F87XA( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P16(address, data, blocksize, lastblock, 0, 16, 0x08, 0, 16, 0);
}
void write_code_P16F54( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P16(address, data, blocksize, lastblock, 1, 2, 0x08, 0x0E, 3, 1);
}
