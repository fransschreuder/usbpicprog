/**************************************************************************
 *   Copyright (C) 2012 by Mike Afheldt                                    *
 *   usbpicprog.org                                                        *
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

/*
 * covers
 *
 * • PIC24FJ32GA102	• PIC24FJ64GA102
 * • PIC24FJ32GA104	• PIC24FJ64GA104
 * • PIC24FJ32GB002	• PIC24FJ64GB002
 * • PIC24FJ32GB004	• PIC24FJ64GB004
 * as defined in PIC24FJ64GA1/GB0 Microchip Doc# DS39934B
 *
 * • PIC24FJ256GA106	• PIC24FJ256GB106
 * • PIC24FJ256GA108	• PIC24FJ256GB108
 * • PIC24FJ256GA110	• PIC24FJ256GB110
 * • PIC24FJ192GA106	• PIC24FJ192GB106
 * • PIC24FJ192GA108	• PIC24FJ192GB108
 * • PIC24FJ192GA110	• PIC24FJ192GB110
 * • PIC24FJ128GA106	• PIC24FJ128GB106
 * • PIC24FJ128GA108	• PIC24FJ128GB108
 * • PIC24FJ128GA110	• PIC24FJ128GB110
 * • PIC24FJ64GB106	• PIC24FJ64GB110
 * • PIC24FJ64GB106	• PIC24FJ64GB108
 * • PIC24FJ64GB110
 * as defined in PIC24FJ64GA1/GB0 Microchip Doc# DS39934B
 *
 * • PIC24FJ16GA002	• PIC24FJ96GA006
 * • PIC24FJ16GA004	• PIC24FJ96GA008
 * • PIC24FJ32GA002	• PIC24FJ96GA010
 * • PIC24FJ32GA004	• PIC24FJ128GA006
 * • PIC24FJ48GA002	• PIC24FJ128GA008
 * • PIC24FJ48GA004
 * • PIC24FJ128GA010
 * • PIC24FJ64GA002
 * • PIC24FJ64GA004
 * • PIC24FJ64GA006
 * • PIC24FJ64GA008
 * • PIC24FJ64GA010
 * as defined in PIC24FJXXXGA0XX Microchip Doc# DS39934B
 *
 *
 * as defined in dsPIC33F/PIC24H Microchip Doc# DS70152E
 *
 * • PIC24F04KA200	• PIC24F04KA201
 * as defined in PIC24FXXKA2XX Microchip Doc# DS39991A
 *
 * • PIC24F08KA101	• PIC24F32KA301
 * • PIC24F16KA101	• PIC24F16KA302
 * • PIC24F08KA102	• PIC24F32KA302
 * • PIC24F16KA102	• PIC24F16KA304
 * • PIC24F16KA301	• PIC24F32KA304
 * as defined in PIC24FXXKA1XX Microchip Doc# DS39919B
 *
 */
#include "interrupt.h"
#include "prog_lolvl.h"
#include "device.h"

extern unsigned char ConfigLimitPGDPGC;

unsigned char p16b_do_write( void )
{
	unsigned char ctr;
	int j;

	dspic_send_24_bits( 0xA8E761 ); 	//BSET NVMCON, #WR
	dspic_send_24_bits( 0x000000 ); 	//NOP
	dspic_send_24_bits( 0x000000 ); 	//NOP
	dspic_send_24_bits( 0x000000 ); 	//NOP		// PIC24H suggests 4 NOPS ?
	dspic_send_24_bits( 0x000000 ); 	//NOP
	for( ctr = 0; ctr < 200; ctr++ )
	{
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x040200 ); //GOTO 0x200
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x803B02 ); //MOV NVMCON, W2
		dspic_send_24_bits( 0x883C22 ); //MOV W2, VISI
		dspic_send_24_bits( 0x000000 ); //NOP
		j = dspic_read_16_bits( is3_3V() );
		dspic_send_24_bits( 0x000000 ); //NOP
		if( (j & 0x8000) == 0 )
			break; //programming completed
		DelayMs( ctr < 10 ? 1: 10 );
	}
	//Step 9: Reset device internal PC.
	dspic_send_24_bits( 0x040200 ); 	//GOTO 0x200
	dspic_send_24_bits( 0x000000 ); 	//NOP
	return (0);
}
void bulk_erase_PIC24( unsigned int nv )
{
	//bulk erase program memory
	//step 2
	dspic_send_24_bits( 0x24000A|(nv<<4) ); //MOV nv, W10
	dspic_send_24_bits( 0x883B0A ); 	//MOV W10, NVMCON
	//step 3
	dspic_send_24_bits( 0x200000 ); 	//MOV #<PAGEVAL>, W0
	dspic_send_24_bits( 0x880190 ); 	//MOV W0, TBLPAG
	dspic_send_24_bits( 0x200000 ); 	//MOV #000, W0
	dspic_send_24_bits( 0xBB0800 ); 	//TBLWTL W0, [W0]
	dspic_send_24_bits( 0x000000 ); 	//NOP
	dspic_send_24_bits( 0x000000 ); 	//NOP

	//step 4
	//step 5
	p16b_do_write();
}
void bulk_erase_P24KA( unsigned char doRestore )
{
	bulk_erase_PIC24( 0x4064 );
}
void bulk_erase_P24FJ( unsigned char doRestore )
{
	bulk_erase_PIC24( 0x404F );
}

void write_code_PIC24( unsigned long address, unsigned char* data, char blocksize, char lastblock, int nv, unsigned char write_size )
{
	unsigned int i;
	unsigned char blockcounter;

	//Step 2: Set the NVMCON to program 32/64 instruction words.
	dspic_send_24_bits( 0x24000A|(nv<<4) ); //MOV #0x4004/#0x4001, W10
	dspic_send_24_bits( 0x883B0A ); 	//MOV W10, NVMCON

	//Step 6: Repeat steps 3-5 eight times to load the write latches for 32 instructions.
	//Step 7: do write	(construct is ugly, but works independent of how large blockcounter increment is (as long as it's a divisor of write_size)
	for( blockcounter = 0; blockcounter < blocksize;
	     blockcounter += 12,((address + blockcounter)%write_size) == 0? p16b_do_write(): 0 )
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
	}

	if( ((address + blockcounter)%write_size) != 0 && lastblock & BLOCKTYPE_LAST )  // do the write if BLOCKTYPE_LAST and we haven't already done so
		p16b_do_write();
}
void write_code_P24KA( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	// write in 32 instruction (32x3 byte) blocks
	write_code_PIC24( address, data, blocksize, lastblock, 0x4004, 32 * 3 );
}
void write_code_P24FJ( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	// write in 64 instruction (64*3 byte) blocks
	write_code_PIC24( address, data, blocksize, lastblock, 0x4001, 64 * 3 );
}

void write_config_bits_PIC24( unsigned long address, unsigned char* data, char blocksize, unsigned int nv )
{
	static char blockcounter;
	unsigned int payload;

	//Step 2: Initialize the write pointer (W7) for the TBLWT instruction.
	dspic_send_24_bits( 0x200007|((address&0xFFFF)<<4)); //MOV <addr15:0>, W7

	//Step 3: Set the NVMCON to program 1 Configuration register.
	dspic_send_24_bits( 0x24000A|(nv<<4));	//MOV #0x4003/#0x4001/#0x4000, W10
	dspic_send_24_bits( 0x883B0A ); 	//MOV W10, NVMCON

	//Step 4: Initialize the TBLPAG register.
	dspic_send_24_bits(0x200000|((address&0xFF0000)>>12)); //MOV <addr23:16>, W0
	dspic_send_24_bits( 0x880190 ); 	//MOV W0, TBLPAG

	for( blockcounter = 0; blockcounter < blocksize; blockcounter += 2 )
	{
		//Step 5: Load the Configuration register data to W6.
		payload = (((unsigned int) data[blockcounter]) | (((unsigned int) data[blockcounter + 1]) << 8));
		dspic_send_24_bits( 0x200006 | ((unsigned long) payload << 4) ); //MOV #<CONFIG_VALUE>, W6

		//Step 6: Write the Configuration register data to the write latch and increment the write pointer.
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0xBB1B86 ); //TBLWTL W6, [W7++]
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP

		//Step 7: Initiate the write cycle.
		//step 8
		//Step 9: Reset device internal PC.
		p16b_do_write();
	}//Step 10: Repeat steps 5-9 until all Configuration registers (2/4) are written.
}
void write_config_bits_P24KA( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_config_bits_PIC24( address, data, blocksize, 0x4004 );
}

void write_config_bits_P24FJ( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_config_bits_PIC24( address, data, blocksize, 0x4003 );
}

void write_config_bits_P24H( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_config_bits_PIC24( address, data, blocksize, 0x4000 );
}
void write_data_P24KA1( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_config_bits_PIC24( address+0x7FFE00, data, blocksize, 0x4004 );
}

void read_code_PIC24( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	char blockcounter = 0;
	unsigned int payload;

	if( lastblock & BLOCKTYPE_CONFIG )
		address = (address*3)/2;		// fix up address

    //Step 2: Initialize the read pointer (W6) and TBLPAG for TBLRD instruction.
    dspic_send_24_bits( 0x200000 | ((((address * 2) / 3) & 0xFF0000) >> 12) ); //MOV #<DestinationAddress23:16>, W0
    dspic_send_24_bits( 0x880190 ); 	//MOV W0, TBLPAG
    dspic_send_24_bits( 0x200006 | ((((address * 2) / 3) & 0x00FFFF) << 4) ); //MOV #<DestinationAddress15:0>, W6

	//Step 2: Initialize the write pointer (W7) for the TBLWT instruction.
	dspic_send_24_bits( 0x207847 ); 	//MOV #VISI, W7
	dspic_send_24_bits( 0x000000 ); 	//NOP

	if( !(lastblock & BLOCKTYPE_CONFIG) ) // temporary until rest of system changes
	{
		for( blockcounter = 0; blockcounter < blocksize; blockcounter += 6 )
		{
			//Step 4: Output W0:W5 using the VISI register and REGOUT command.
			dspic_send_24_bits( 0xBA0B96 ); //TBLRDL [W6], [W7]
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
			payload = dspic_read_16_bits( is3_3V() ); //read <VISI>
			data[blockcounter + 0] = (unsigned char) payload;
			data[blockcounter + 1] = (unsigned char) (payload >> 8);
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0xBADBB6 ); //TBLRDH.B [W6++], [W7++]
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0xBAD3D6 ); //TBLRDH.B [++W6], [W7--]
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
			payload = dspic_read_16_bits( is3_3V() ); //read <VISI>
			data[blockcounter + 2] = (unsigned char) payload;
			data[blockcounter + 3] = (unsigned char) (payload >> 8);
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0xBA0BB6 ); //TBLRDL [W6++], [W7]
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
			payload = dspic_read_16_bits( is3_3V() ); //read <VISI>
			data[blockcounter + 4] = (unsigned char) payload;
			data[blockcounter + 5] = (unsigned char) (payload >> 8);
			dspic_send_24_bits( 0x000000 ); //NOP
		}
		dspic_send_24_bits( 0x040200 ); 	//GOTO 0x200
		dspic_send_24_bits( 0x000000 ); 	//NOP
	}
	else // read config/EEPROM words
	{

		for( blockcounter = 0; blockcounter < blocksize; blockcounter += 2 )
		{
			//Step 3: Read the Configuration register and write it to the VISI register (located at 0x784).
			dspic_send_24_bits( 0xBA0BB6 ); //TBLRDL [W6++], [W7]
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
			payload = dspic_read_16_bits( is3_3V() ); //read <VISI>
			data[blockcounter] = (unsigned char) payload;
			data[blockcounter + 1] = (unsigned char) (payload >> 8);
			dspic_send_24_bits( 0x000000 ); //NOP
		}
		dspic_send_24_bits( 0x040200 ); 	//GOTO 0x200
		dspic_send_24_bits( 0x000000 ); 	//NOP
	}
}
void read_data_P24KA1( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	read_code_PIC24( address+0x7FFE00, data, blocksize, lastblock | BLOCKTYPE_CONFIG );
}
#pragma romdata DEVICES
DEVICE_TABLE devices_pic24[] =
{

//    		Pictype,	picfamily,5V,	enter_ISCP,	bulk_erase,	read_code,	read_data,	write_code,	write_data,	write_config_bits )
DEVICE_ENTRY( P24FJXXXGA0XX,PIC24,	3V,	PIC24,		P24FJ,		PIC24,		none,		P24FJ,		none,		P24FJ )
DEVICE_ENTRY( P24FJXXXGA1,	PIC24,	3V,	PIC24,		P24FJ,		PIC24,		none,		P24FJ,		none,		P24FJ )
DEVICE_ENTRY( P24FXXKAXXX,	PIC24,	3V,	PIC24,		P24KA,		PIC24,		P24KA1,		P24KA,		P24KA1,		P24KA )
//FIXME: why is this commented out
//DEVICE_ENTRY( P24FVXXKA1XX,	PIC24,	5V,	PIC24,		P24KA,		PIC24,		P24KA1,		P24KA,		P24KA,		P24KA )
DEVICE_ENTRY( P24FJG,		PIC24,	3V,	PIC24,		P24FJ,		PIC24,		none,		P24FJ,		none,		P24FJ )
DEVICE_ENTRY( dsP33F,		dsPIC33,3V,	PIC24,		P24FJ,		PIC24,		none,		P24FJ,		none,		P24H )
DEVICE_ENTRY( P24H,		    PIC24,	3V,	PIC24,		P24FJ,		PIC24,		none,		P24FJ,		none,		P24H )
};
#pragma romdata
#undef LIST
