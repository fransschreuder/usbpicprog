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
 * • PIC24EP256MC202
 * as defined in PIC24FXXKA1XX Microchip Doc# DS70633D
 *
 */

#include "interrupt.h"
#include "prog_lolvl.h"
#include "device.h"
#include <delays.h>
int write_status;
extern unsigned char ConfigLimitPGDPGC;
void read_code_PIC24E( unsigned long address, unsigned char* data, char blocksize, char lastblock );

unsigned char p24E_do_write( void ) {
    unsigned char ctr;
    int j;

    dspic_send_24_bits( 0x20055A );     //MOV #0x55, W1
    dspic_send_24_bits( 0x88397A );     //MOV W1, NVMKEY
    dspic_send_24_bits( 0x200AAA );     //MOV #0xAA, W1
    dspic_send_24_bits( 0x88397A );     //MOV W1, NVMKEY
    dspic_send_24_bits( 0xA8E729 );     //BSET NVMCON, #WR
    for( ctr = 0; ctr < 200; ctr++ )
    {
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x040200 ); //GOTO 0x200
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x80394A ); //MOV NVMCON, W2
        dspic_send_24_bits( 0x887C4A ); //MOV W2, VISI
        dspic_send_24_bits( 0x000000 ); //NOP
        j = dspic_read_16_bits( is3_3V() );
        dspic_send_24_bits( 0x000000 ); //NOP
        if( (j & 0x8000) == 0 )
            break; //programming completed
        Delay100TCYx(5);
    }
    return (0);
}
unsigned char p24E_do_write1( void ) {
    unsigned char ctr;
    int j;

    dspic_send_24_bits( 0x20055A );     //MOV #0x55, W1
    dspic_send_24_bits( 0x88397A );     //MOV W1, NVMKEY
    dspic_send_24_bits( 0x200AAA );     //MOV #0xAA, W1
    dspic_send_24_bits( 0x88397A );     //MOV W1, NVMKEY
    dspic_send_24_bits( 0xA8E729 );     //BSET NVMCON, #WR
    dspic_send_24_bits( 0x000000 ); //NOP
    dspic_send_24_bits( 0x000000 ); //NOP
    dspic_send_24_bits( 0x000000 ); //NOP
    DelayMs(100);
    for( ctr = 0; ctr < 200; ctr++ )
    {
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x040200 ); //GOTO 0x200
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x80394A ); //MOV NVMCON, W2
        dspic_send_24_bits( 0x887C4A ); //MOV W2, VISI
        dspic_send_24_bits( 0x000000 ); //NOP
        j = dspic_read_16_bits( is3_3V() );
        dspic_send_24_bits( 0x000000 ); //NOP
        if( (j & 0x8000) == 0 )
            break; //programming completed
        DelayMs( ctr < 10 ? 1: 10 );
    }
    return (0);
}
void bulk_erase_PIC24E( unsigned int nv )
{
    //bulk erase program memory
    //step 2
    dspic_send_24_bits( 0x24000A|(nv<<4) ); //MOV nv, W10
    dspic_send_24_bits( 0x88394A );     //MOV W10, NVMCON
    dspic_send_24_bits( 0x000000 );     //NOP
    dspic_send_24_bits( 0x000000 );     //NOP

    //step 4
    //step 5
    p24E_do_write1();
}
void bulk_erase_P24EP( unsigned char doRestore )
{
    bulk_erase_PIC24E( 0x400F );
}

void write_code_PIC24E( unsigned long address, unsigned char* data, char blocksize, char lastblock, int nv )
{
    // *** this fails if blocksize is not a multiple of 6 (ie an even number of instructions ***
    unsigned int i;
    unsigned char blockcounter;

    //Step 6: Set the NVMCON to program 2 instruction words.
    dspic_send_24_bits( 0x24000A|(nv<<4) ); //MOV #0x4001, W10
    dspic_send_24_bits( 0x88394A );     //MOV W10, NVMCON
    dspic_send_24_bits( 0x000000 );     //NOP
    dspic_send_24_bits( 0x000000 );     //NOP
    //Step 2: setup TBLPAG
    dspic_send_24_bits( 0x200FA0 );     //MOV #FA, W0
    dspic_send_24_bits( 0x8802A0 );     //MOV W0, TBLPAG

    //Step 6: Repeat steps 3-5 .
    for( blockcounter = 0; blockcounter < blocksize; blockcounter += 6 )
    {
        //Step 3: Load W0:W2 with the next 2 instruction words to program
        for( i = 0; i < 3; i++ )
        {
            dspic_send_24_bits( 0x200000 | (((unsigned long) data[blockcounter + (i * 2)]) << 4)
                    | (((unsigned long) data[blockcounter + (i * 2) + 1]) << 12)
                    | ((unsigned long) i) );
            /**
             MOV #<LSW0>, W0
             MOV #<MSB1:MSB0>, W1
             MOV #<LSW1>, W2
             */
        }
        //Step 4: Set the read pointer (W6) and load the (next set of) write latches.
        dspic_send_24_bits( 0xEB0300 ); //CLR W6
//        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0xEB0380 ); //CLR W7
//        dspic_send_24_bits( 0x000000 ); //NOP
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
        //Step 5: Set the NVMADRU/NVMADR register-pair to point to the correct address
        dspic_send_24_bits( 0x200006 | (((((blockcounter + address) * 2) / 3) & 0x00FFFF) << 4) );  //MOV #<DestinationAddress15:0>, W3
        dspic_send_24_bits( 0x200007 | (((((blockcounter + address) * 2) / 3) & 0xFF0000) >> 12) ); //MOV #<DestinationAddress23:16>, W4
        dspic_send_24_bits( 0x883956 ); //MOV W3, NVMADR
        dspic_send_24_bits( 0x883967 ); //MOV W4, NVMADRU
        //Step 7: Do the write
        p24E_do_write();
    }

}
void write_code_P24EP( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_PIC24E( address, data, blocksize, lastblock, 0x4001 );
}

#if 0
void write_config_bits_PIC24E( unsigned long address, unsigned char* data, char blocksize, unsigned int nv )
{
	static char blockcounter;
	unsigned int payload;

    //Step 6: Set the NVMCON to program 2 instruction words.
    dspic_send_24_bits( 0x24000A|(nv<<4) ); //MOV #0x4001, W10
    dspic_send_24_bits( 0x000000 );     //NOP
    dspic_send_24_bits( 0x88394A );     //MOV W10, NVMCON
    dspic_send_24_bits( 0x000000 );     //NOP
    dspic_send_24_bits( 0x000000 );     //NOP
    //Step 2: setup TBLPAG
    dspic_send_24_bits( 0x200FA0 );     //MOV #FA, W0
    dspic_send_24_bits( 0x8802A0 );     //MOV W0, TBLPAG

    //Step 6: Repeat steps 3-5 .
    for( blockcounter = 0; blockcounter < blocksize; blockcounter += 4 )
    {
        //Step 3: Load W0:W1 with the next 2 configuration bytes
        dspic_send_24_bits( 0x2FF000 |data[blockcounter] << 4 );    //MOV data,W0
        dspic_send_24_bits( 0x2FF001 |data[blockcounter+2] << 4 );  //MOV data,W1
        //Step 4: Set the read pointer (W3) and load the (next set of) write latches.
        dspic_send_24_bits( 0xEB0180 ); //CLR W3
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0xBB1980 ); //TBLWTL W0, [W3++]
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0xBB0981 ); //TBLWTL W1, [W3]
        dspic_send_24_bits( 0x000000 ); //NOP
        dspic_send_24_bits( 0x000000 ); //NOP
        //Step 5: Set the NVMADRU/NVMADR register-pair to point to the correct address
        dspic_send_24_bits( 0x200003 | (((((blockcounter + address) * 2) / 3) & 0x00FFFF) << 4) );  //MOV #<DestinationAddress15:0>, W3
        dspic_send_24_bits( 0x200004 | (((((blockcounter + address) * 2) / 3) & 0xFF0000) >> 12) ); //MOV #<DestinationAddress23:16>, W4
        dspic_send_24_bits( 0x883953 ); //MOV W3, NVMADR
        dspic_send_24_bits( 0x883964 ); //MOV W4, NVMADRU
        //Step 7: Do the write
        p24E_do_write();
    }
}
#else
/*
 * writes configs 2 at a time
 * if odd number (e.g. 1) read address+1, then write
 */
void write_config_bits_P24EP( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
    unsigned char data1[6];
    unsigned char u;
    static unsigned char byte1, first;

    data1[1] = 0xFF;
    data1[2] = 0xFF;
    data1[3] = 0xFF;
    data1[5] = 0xFF;
    if( lastblock & BLOCKTYPE_FIRST )
        first = 1;
    for( u = 0; u < blocksize; u += 2 ) {
        if( first ) {
            first = 0;
            byte1 = data[u];
        }
        else {
            data1[0] = byte1;
            data1[4] = data[u];
            write_code_PIC24E( (address+u-2)*3/2, data1, 6, 0, 0x4001);
            first = 1;
        }
    }
    if( (lastblock & BLOCKTYPE_LAST) && !first ) {
        read_code_PIC24E( address+u, &data1[4], 2, BLOCKTYPE_CONFIG|BLOCKTYPE_FIRST|BLOCKTYPE_LAST );
        write_code_PIC24E( (address+u-2)*3/2, data1, 6, 0, 0x4001);
        first = 1;
    }
}
#endif
void read_code_PIC24E( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	char blockcounter = 0;
	unsigned int payload;

	if( lastblock & BLOCKTYPE_CONFIG )
		address = (address*3)/2;		// fix up address

	//Step 2: Initialize the read pointer (W6) and TBLPAG for TBLRD instruction.
	dspic_send_24_bits( 0x200000 | ((((address * 2) / 3) & 0xFF0000) >> 12) ); //MOV #<DestinationAddress23:16>, W0
	dspic_send_24_bits( 0x8802A0 ); 	//MOV W0, TBLPAG
	dspic_send_24_bits( 0x200006 | ((((address * 2) / 3) & 0x00FFFF) << 4) ); //MOV #<DestinationAddress15:0>, W6

	//Step 2: Initialize the write pointer (W7) for the TBLWT instruction.
	dspic_send_24_bits( 0x20F887 ); 	//MOV #VISI, W7
	dspic_send_24_bits( 0x000000 ); 	//NOP


	if( !(lastblock & BLOCKTYPE_CONFIG) ) // temporary until rest of system changes
	{
		for( blockcounter = 0; blockcounter < blocksize; blockcounter += 6 )
		{
			//Step 4: Output W0:W5 using the VISI register and REGOUT command.
			dspic_send_24_bits( 0xBA0B96 ); //TBLRDL [W6], [W7]
			dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
			payload = dspic_read_16_bits( is3_3V() ); //read <VISI>
			data[blockcounter + 0] = (unsigned char) payload;
			data[blockcounter + 1] = (unsigned char) (payload >> 8);
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0xBADBB6 ); //TBLRDH.B [W6++], [W7++]
            dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0xBAD3D6 ); //TBLRDH.B [++W6], [W7--]
            dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
			payload = dspic_read_16_bits( is3_3V() ); //read <VISI>
			data[blockcounter + 2] = (unsigned char) payload;
			data[blockcounter + 3] = (unsigned char) (payload >> 8);
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0xBA0BB6 ); //TBLRDL [W6++], [W7]
			dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
			payload = dspic_read_16_bits( is3_3V() ); //read <VISI>
			data[blockcounter + 4] = (unsigned char) payload;
			data[blockcounter + 5] = (unsigned char) (payload >> 8);
			dspic_send_24_bits( 0x000000 ); //NOP
		}
		dspic_send_24_bits( 0x040200 ); 	//GOTO 0x200
        dspic_send_24_bits( 0x000000 );     //NOP
        dspic_send_24_bits( 0x000000 );     //NOP
        dspic_send_24_bits( 0x000000 );     //NOP
	}
	else // read config/EEPROM words
	{

		for( blockcounter = 0; blockcounter < blocksize; blockcounter += 2 )
		{
            //Step 3: Read the Configuration register and write it to the VISI register (located at 0xF88).
			dspic_send_24_bits( 0xBA0BB6 ); //TBLRDL [W6++], [W7]
			dspic_send_24_bits( 0x000000 ); //NOP
			dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
            dspic_send_24_bits( 0x000000 ); //NOP
			payload = dspic_read_16_bits( is3_3V() ); //read <VISI>
			data[blockcounter] = (unsigned char) payload;
			data[blockcounter + 1] = (unsigned char) (payload >> 8);
			dspic_send_24_bits( 0x000000 ); //NOP
		}
		dspic_send_24_bits( 0x040200 ); 	//GOTO 0x200
		dspic_send_24_bits( 0x000000 ); 	//NOP
		dspic_send_24_bits( 0x000000 );     //NOP
		dspic_send_24_bits( 0x000000 );     //NOP
	}
}

#pragma romdata DEVICES
DEVICE_TABLE devices_pic24e[] =
{
//    		Pictype,	picfamily,5V,	enter_ISCP,	bulk_erase,	read_code,	read_data,	write_code,	write_data,	write_config_bits )
DEVICE_ENTRY( P24EPXXX,    PIC24,	3V,	PIC24E,		P24EP,		PIC24E,	    none,		P24EP,		none,		P24EP )
};
#pragma romdata
#undef LIST
