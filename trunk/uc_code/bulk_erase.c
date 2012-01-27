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
#include "bulk_erase.h"
#ifdef SDCC
#else
#include <p18cxxx.h>
#endif
#include "typedefs.h"
#include "io_cfg.h"             // I/O pin mapping
#include "prog.h"
#include "interrupt.h"
#include "prog_lolvl.h"

#define NORESTORE 0xAA
extern PICFAMILY picfamily;
extern PICTYPE pictype;

/**
 Bulk erases the whole device
 */
char bulk_erase( PICFAMILY picfamily, PICTYPE pictype, unsigned char doRestore )
{
	set_vdd_vpp( pictype, picfamily, 1 );
#ifdef TABLE
	if( currDevice.bulk_erase )
		currDevice.bulk_erase( doRestore );
	else
		switch( pictype ) {
#else
		switch (pictype)
		{
			case I2C_EE_1:
			case I2C_EE_2:
			bulk_erase_none(0);
			break;
			case P24FXXKAXXX:
			bulk_erase_P24KA(doRestore);
			break;
			case dsP30F:
			bulk_erase_dsP30F(doRestore);
			break;
			case P18FXX39:
			bulk_erase_P18FXX39(doRestore);
			break;
			case P18F872X:
			bulk_erase_P18F872X(doRestore);
			break;
			case P18F2XXX: //also valid for 18F4XXX
			bulk_erase_P18F2XXX(doRestore);
			break;
			case P18F4XK22:
			case P18LF4XK22:
			bulk_erase_P18F4XK22(doRestore);
			break;
			case P18FX220:
			case P18FXX31:
			case P18F6X2X:
			case P18FXX2: //also valid for 18FXX8
			bulk_erase_P18FX220(doRestore);
			break;
			case P18F6XKXX:
			bulk_erase_P18F6XKXX(doRestore);
			break;
			case P18F97J60:
			case P18F45J10:
			bulk_erase_P18F97J60(doRestore);
			break;
			case P18LF13K22:
			case P18LF14K22:
			bulk_erase_P18LF13K22( doRestore );
			break;
			case P18F13K22:
			case P18F14K22:
			bulk_erase_P18F13K22( doRestore );
			break;
			case P16F716:
			bulk_erase_P16F716(doRestore);
			break;
			case P16F72:
			case P16F7X:
			case P16F7X7:
			bulk_erase_P16F72(doRestore);
			break;
			case P16F87X:
			bulk_erase_P16F87X(doRestore);
			break;
			case P16F62X:
			bulk_erase_P16F62X(doRestore);
			break;
			case P16F182X:
			case P16F785:
			case P16F88X:
			bulk_erase_P16F182X(doRestore);
			break;
			case P16F91X:
			bulk_erase_P16F91X(doRestore);
			break;
			case P16F87:
			case P16F81X:
			case P16F87XA:
			bulk_erase_P16F87(doRestore);
			break;
			case P12F629:
			bulk_erase_P12F629(doRestore);
			break;
			case P12F61X:
			bulk_erase_P12F61X(doRestore);
			break;
			case P16F84A: //same as P16F62XA
			bulk_erase_P16F84A(doRestore);
			break;
			case P12F6XX: //same as P16F62XA
			case P16F62XA:
			bulk_erase_P12F6XX(doRestore);
			break;
			case P16F59:
			case P16F57:
			case P16F54:
			bulk_erase_P16F59(doRestore);
			break;
			case P10F200: //resets at 0x1FF
			bulk_erase_P10F200(doRestore);
			break;
			case P12F508:
			case P10F202: //resets at 0x3FF
			bulk_erase_P10F202(doRestore);
			break;
#endif
		default:
			PGDlow();
			set_vdd_vpp( pictype, picfamily, 0 );
			return 3;
			break;
		}
	set_vdd_vpp( pictype, picfamily, 0 );
	return 1; //1 means "OK"
}

void bulk_erase_none( unsigned char doRestore )
{
}
#if 0
void bulk_erase_P24KA( unsigned char doRestore )
{
	unsigned int i, j;
	//bulk erase program memory
	//step 1
	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0x040200 ); //GOTO 0x200
	dspic_send_24_bits( 0x000000 ); //NOP
	//step 2
	dspic_send_24_bits( 0x24064A ); //MOV #0x4064, W10
	dspic_send_24_bits( 0x883B0A ); //MOV W10, NVMCON
	//step 3
	dspic_send_24_bits( 0x200000 ); //MOV #<PAGEVAL>, W0
	dspic_send_24_bits( 0x880190 ); //MOV W0, TBLPAG
	dspic_send_24_bits( 0x200000 ); //MOV #000, W0
	dspic_send_24_bits( 0xBB0800 ); //TBLWTL W0, [W0]
	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0x000000 ); //NOP

	//step 4
	dspic_send_24_bits( 0xA8E761 ); //BSET NVMCON, #WR
	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0x000000 ); //NOP
	for( i = 0; i < 200; i++ )
	{
		//step 5
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x040200 ); //GOTO 0x200
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x803B02 ); //MOV NVMCON, W2
		dspic_send_24_bits( 0x883C22 ); //MOV W2, VISI
		dspic_send_24_bits( 0x000000 ); //NOP
		j = dspic_read_16_bits( is3_3V() );
		dspic_send_24_bits( 0x000000 ); //NOP
		if( (j && 0x8000) == 0 )
			break; //programming completed
		DelayMs( 10 );

	}//step 8: repeat step 5-7
}
#endif
void bulk_erase_dsP30F( unsigned char doRestore )
{
	unsigned int i;

	//bulk erase program memory
	//step 1
	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0x040100 ); //GOTO 0x100
	dspic_send_24_bits( 0x000000 ); //NOP
	//step 2
	dspic_send_24_bits( 0x24008A ); //MOV #0x4008, W10
	dspic_send_24_bits( 0x883B0A ); //MOV W10, NVMCON
	//step 3
	dspic_send_24_bits( 0x200F80 ); //MOV #0xF8, W0
	dspic_send_24_bits( 0x880190 ); //MOV W0, TBLPAG
	dspic_send_24_bits( 0x200067 ); //MOV #0x6, W7
	//step 4
	dspic_send_24_bits( 0xEB0300 ); //CLR W6
	dspic_send_24_bits( 0x000000 ); //NOP
	for( i = 0; i < 2; i++ )
	{
		//step 5
		dspic_send_24_bits( 0xBB1B86 ); //TBLWTL W6, [W7++]
		//step 6
		dspic_send_24_bits( 0x200558 ); //MOV #0x55, W8
		dspic_send_24_bits( 0x200AA9 ); //MOV #0xAA, W9
		dspic_send_24_bits( 0x883B38 ); //MOV W8, NVMKEY
		dspic_send_24_bits( 0x883B39 ); //MOV W9, NVMKEY
		//step 7
		dspic_send_24_bits( 0xA8E761 ); //BSET NVMCON, #WR
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
		DelayMs( 201 ); //Externally time 200ms
		dspic_send_24_bits( 0xA9E761 ); //BCLR NVMCON, #WR
		dspic_send_24_bits( 0x000000 ); //NOP
		dspic_send_24_bits( 0x000000 ); //NOP
	}//step 8: repeat step 5-7
	//step 9
	dspic_send_24_bits( 0x2407FA ); //MOV #0x407F, W10
	dspic_send_24_bits( 0x883B0A ); //MOV W10, NVMCON
	//step 10
	dspic_send_24_bits( 0x200558 ); //MOV #0x55, W8
	dspic_send_24_bits( 0x883B38 ); //MOV W8, NVMKEY
	dspic_send_24_bits( 0x200AA9 ); //MOV #0xAA, W9
	dspic_send_24_bits( 0x883B39 ); //MOV W9, NVMKEY
	//step 11
	dspic_send_24_bits( 0xA8E761 ); //BSET NVMCON, #WR
	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0x000000 ); //NOP
	DelayMs( 3 ); //Externally time 2 msec
	dspic_send_24_bits( 0xA9E761 ); //BCLR NVMCON, #WR
	dspic_send_24_bits( 0x000000 ); //NOP
	dspic_send_24_bits( 0x000000 ); //NOP
}
void bulk_erase_P18FXX39( unsigned char doRestore )
{

	set_address( picfamily, 0x3C0004 );
	pic_send( 4, 0x0C, 0x0083 ); //shouldn't it be 0x0C??? prog spec says 0x00
	pic_send( 4, 0x00, 0x0000 ); //NOP
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( P11 );
	pic_send( 4, 0x0C, 0x0088 ); //shouldn't it be 0x0C??? prog spec says 0x00
	pic_send( 4, 0x00, 0x0000 ); //NOP
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( P11 );
	pic_send( 4, 0x0C, 0x0089 ); //shouldn't it be 0x0C??? prog spec says 0x00
	pic_send( 4, 0x00, 0x0000 ); //NOP
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( P11 );
	pic_send( 4, 0x0C, 0x008A ); //shouldn't it be 0x0C??? prog spec says 0x00
	pic_send( 4, 0x00, 0x0000 ); //NOP
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( P11 );
	set_address( picfamily, 0x3C0006 );
	pic_send( 4, 0x00, 0x8EA6 ); //BSF EECON1, EEPGD
	pic_send( 4, 0x00, 0x9CA6 ); //BCF EECON1, CFGS
	set_address( picfamily, 0x200000 );
	pic_send( 4, 0x00, 0x84A6 ); //BSF EECON1, WREN
	pic_send( 4, 0x00, 0x88A6 ); //BSF EECON1, FREE
	pic_send( 4, 0x00, 0x0E55 ); // MOVLW 55h
	pic_send( 4, 0x00, 0x6EA7 ); // MOVWF EECON2
	pic_send( 4, 0x00, 0x0EAA ); // MOVLW AAh
	pic_send( 4, 0x00, 0x6EA7 ); // MOVWF EECON2
	pic_send( 4, 0x00, 0x82A6 ); // BSF EECON1, WR

	pic_send( 4, 0x00, 0x0000 ); //NOP
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( P11 );
	pic_send( 4, 0x00, 0x94A6 ); //BCF EECON1, WREN
}
void bulk_erase_P18F872X( unsigned char doRestore )
{

	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0xFFFF ); //Write 0101h to 3C0005h
	set_address( picfamily, 0x3C0004 );
	pic_send( 4, 0x0C, 0x8787 ); //Write 8080h to 3C0004h
	pic_send( 4, 0x00, 0x0000 ); //NOP
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 400 );
}
void bulk_erase_P18F2XXX( unsigned char doRestore )
{

	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x3F3F ); //Write 3F3Fh to 3C0005h
	set_address( picfamily, 0x3C0004 );
	pic_send( 4, 0x0C, 0x8F8F ); //Write 8F8Fh to 3C0004h
	pic_send( 4, 0x00, 0x0000 ); //NOP
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( P11 );
}
void bulk_erase_P18F4XK22( unsigned char doRestore )
{

	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x0F0F ); //Write 0F0Fh to 3C0005h
	set_address( picfamily, 0x3C0004 );
	pic_send( 4, 0x0C, 0x8F8F ); //Write 8F8Fh to 3C0004h
	pic_send( 4, 0x00, 0x0000 ); //NOP
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( P11K );
}
void bulk_erase_P18FX220( unsigned char doRestore )
{

	set_address( picfamily, 0x3C0004 );
	pic_send( 4, 0x0C, 0x0080 );
	pic_send( 4, 0x00, 0x0000 ); //NOP
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( P11 );
}
void bulk_erase_P18F6XKXX( unsigned char doRestore )
{
	set_address( picfamily, 0x3C0004 ); //Erase configuration words
	pic_send( 4, 0x0C, 0x0202 );
	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x0000 );
	set_address( picfamily, 0x3C0006 );
	pic_send( 4, 0x0C, 0x8080 );
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 6 );

	set_address( picfamily, 0x3C0004 ); //Erase Data EEPROM
	pic_send( 4, 0x0C, 0x0404 );
	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x0000 );
	set_address( picfamily, 0x3C0006 );
	pic_send( 4, 0x0C, 0x8080 );
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 6 );

	set_address( picfamily, 0x3C0004 ); //Erase Boot Block
	pic_send( 4, 0x0C, 0x0505 );
	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x0000 );
	set_address( picfamily, 0x3C0006 );
	pic_send( 4, 0x0C, 0x8080 );
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 6 );

	set_address( picfamily, 0x3C0004 ); //Erase Block 0
	pic_send( 4, 0x0C, 0x0404 );
	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x0101 );
	set_address( picfamily, 0x3C0006 );
	pic_send( 4, 0x0C, 0x8080 );
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 6 );

	set_address( picfamily, 0x3C0004 ); //Erase Block 1
	pic_send( 4, 0x0C, 0x0404 );
	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x0202 );
	set_address( picfamily, 0x3C0006 );
	pic_send( 4, 0x0C, 0x8080 );
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 6 );

	set_address( picfamily, 0x3C0004 ); //Erase Block 2
	pic_send( 4, 0x0C, 0x0404 );
	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x0404 );
	set_address( picfamily, 0x3C0006 );
	pic_send( 4, 0x0C, 0x8080 );
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 6 );

	set_address( picfamily, 0x3C0004 ); //Erase Block 3
	pic_send( 4, 0x0C, 0x0404 );
	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x0808 );
	set_address( picfamily, 0x3C0006 );
	pic_send( 4, 0x0C, 0x8080 );
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 6 );

	set_address( picfamily, 0x3C0004 ); //Erase Block 4
	pic_send( 4, 0x0C, 0x0404 );
	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x1010 );
	set_address( picfamily, 0x3C0006 );
	pic_send( 4, 0x0C, 0x8080 );
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 6 );

	set_address( picfamily, 0x3C0004 ); //Erase Block 5
	pic_send( 4, 0x0C, 0x0404 );
	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x2020 );
	set_address( picfamily, 0x3C0006 );
	pic_send( 4, 0x0C, 0x8080 );
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 6 );

	set_address( picfamily, 0x3C0004 ); //Erase Block 6
	pic_send( 4, 0x0C, 0x0404 );
	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x4040 );
	set_address( picfamily, 0x3C0006 );
	pic_send( 4, 0x0C, 0x8080 );
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 6 );

	set_address( picfamily, 0x3C0004 ); //Erase Block 7
	pic_send( 4, 0x0C, 0x0404 );
	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x8080 );
	set_address( picfamily, 0x3C0006 );
	pic_send( 4, 0x0C, 0x8080 );
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 6 );
}

void bulk_erase_P18F97J60( unsigned char doRestore )
{

	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x0101 ); //Write 0101h to 3C0005h
	set_address( picfamily, 0x3C0004 );
	pic_send( 4, 0x0C, 0x8080 ); //Write 8080h to 3C0004h
	pic_send( 4, 0x00, 0x0000 ); //NOP
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 500 );
}
void bulk_erase_P18F13K22( unsigned char doRestore )
{
	set_address( picfamily, 0x3C0005 );
	pic_send( 4, 0x0C, 0x0F0F ); //Write 0F0Fh to 3C0005h
	set_address( picfamily, 0x3C0004 );
	pic_send( 4, 0x0C, 0x8F8F ); //Write 8080h to 3C0004h
	pic_send( 4, 0x00, 0x0000 ); //NOP
	pic_send( 4, 0x00, 0x0000 ); //hold PGD low until erase completes
	DelayMs( 6 );
}
void bulk_erase_P16F716( unsigned char doRestore )
{

	pic_send_14_bits( 6, 0x00, 0x0000 );//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
	pic_send_n_bits( 6, 0x09 ); //send 1001xx to erase device
	DelayMs( 30 );
}
void bulk_erase_P16F72( unsigned char doRestore )
{

	pic_send_n_bits( 6, 0x09 ); //send 1001xx to erase device
	DelayMs( 30 );
}
void bulk_erase_P16F87X( unsigned char doRestore )
{
	unsigned char temp[2];
	int i;

	read_code( picfamily, pictype, 0x2007, temp, 2, 3 ); //read the config word to see wether it is code protected or not
	set_vdd_vpp( pictype, picfamily, 0 );
	DelayMs( 10 );
	set_vdd_vpp( pictype, picfamily, 1 );
	DelayMs( 10 );
	if( ((temp[1] & 0x31) < 0x31) || ((temp[0] & 0x30) < 0x30) )
	{
		pic_send_14_bits( 6, 0x00, 0x3FFF );//Execute a Load Configuration command (00) with (0x3FFF)
		for( i = 0; i < 7; i++ )
			pic_send_n_bits( 6, 0x06 );//2. Execute Increment Address command (000110) to set address to configuration word location (0x2007)
		pic_send_n_bits( 6, 0x01 ); //3. Execute a Bulk Erase Setup1 command (000001)
		pic_send_n_bits( 6, 0x07 ); //4. Execute a Bulk Erase Setup2 command (000111)
		pic_send_n_bits( 6, 0x08 ); //5. Execute a Begin Erase/Programming command (001000)
		DelayMs( 8 ); //6. Wait 8 ms
		pic_send_n_bits( 6, 0x01 ); //7. Execute a Bulk Erase Setup1 command (000001)
		pic_send_n_bits( 6, 0x07 ); //8. Execute a Bulk Erase Setup2 command (000111)
	}
	else //device is not code protected
	{
		//Execute a Load Data for Program Memory command
		pic_send_14_bits( 6, 0x02, 0x3FFF );//(000010) with a ’1’ in all locations (0x3FFF)
		pic_send_n_bits( 6, 0x01 ); //2. Execute a Bulk Erase Setup1 command (000001)
		pic_send_n_bits( 6, 0x07 ); //3. Execute a Bulk Erase Setup2 command (000111)
		pic_send_n_bits( 6, 0x08 ); //4. Execute a Begin Erase/Programming command (001000)
		DelayMs( 8 ); //5. Wait 8 ms
		pic_send_n_bits( 6, 0x01 ); //6. Execute a Bulk Erase Setup1 command (000001)
		pic_send_n_bits( 6, 0x07 ); //7. Execute a Bulk Erase Setup2 command (000111)
		//Procedure to bulk erase data memory:
		pic_send_14_bits( 6, 0x03, 0x3FFF );//1. Execute a Load Data for Data Memory command (000011) with (0x3FFF)

		pic_send_n_bits( 6, 0x01 ); //2. Execute a Bulk Erase Setup1 command (000001)
		pic_send_n_bits( 6, 0x07 ); //3. Execute a Bulk Erase Setup2 command (000111)
		pic_send_n_bits( 6, 0x08 ); //4. Execute a Begin Erase/Programming command (001000)
		DelayMs( 8 ); //5. Wait 8 ms
		pic_send_n_bits( 6, 0x01 ); //6. Execute a Bulk Erase Setup1 command (000001)
		pic_send_n_bits( 6, 0x07 ); //7. Execute a Bulk Erase Setup2 command (000111)
	}
}
void bulk_erase_P16F62X( unsigned char doRestore )
{
	int i;

	pic_send_14_bits( 6, 0x00, 0x3FFF );//Execute a Load Configuration command (dataword 0x3FFF) to set PC to 0x2000.
	for( i = 0; i < 7; i++ )
		pic_send_n_bits( 6, 0x06 );//set PC to 0x2007
	/*pic_send_14_bits(6,0x02,0x3FFF);*/
	pic_send_n_bits( 6, 0x01 );//,0x0000);//Execute Bulk Erase Setup 1 command.
	pic_send_n_bits( 6, 0x07 );//,0x0000);//Execute Bulk Erase Setup 2 command.
	pic_send_n_bits( 6, 0x08 );//, 0x0000);//Execute Begin Erase Programming command.
	DelayMs( 20 ); //Wait Tera + Tprog.
	pic_send_n_bits( 6, 0x01 );//,0x0000);//Execute Bulk Erase Setup 1 command.
	pic_send_n_bits( 6, 0x07 );//,0x0000);//Execute Bulk Erase Setup 2 command.
	DelayMs( 20 );
	set_vdd_vpp( pictype, picfamily, 0 );//reset
	DelayMs( 10 );
	set_vdd_vpp( pictype, picfamily, 1 );
	DelayMs( 10 );

	pic_send_14_bits( 6, 0x00, 0x3FFF );//Execute a Load Configuration command (dataword 0x0000) to set
	/*pic_send_14_bits(6,0x02,0x3FFF);*/
	pic_send_n_bits( 6, 0x09 ); //bulk erase program memory, userid and config memory
	pic_send_n_bits( 6, 0x08 );//Execute Begin Erase Programming command.
	DelayMs( 20 ); //Wait Tera + Tprog.
	set_vdd_vpp( pictype, picfamily, 0 );//reset
	DelayMs( 10 );
	set_vdd_vpp( pictype, picfamily, 1 );
	DelayMs( 10 );
	pic_send_14_bits( 6, 0x02, 0x3FFF );
	pic_send_n_bits( 6, 0x0B ); //bulk erase data memory
	pic_send_n_bits( 6, 0x08 );//Execute Begin Erase Programming command.
	DelayMs( 6 ); //wait terase
}
void bulk_erase_P16F182X( unsigned char doRestore )
{

	pic_send_14_bits( 6, 0x00, 0x0000 );//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
	pic_send_n_bits( 6, 0x09 ); //bulk erase program memory, userid and config memory
	DelayMs( 6 ); //wait terase
	set_vdd_vpp( pictype, picfamily, 0 );//reset
	DelayMs( 10 );
	set_vdd_vpp( pictype, picfamily, 1 );
	DelayMs( 10 );
	pic_send_n_bits( 6, 0x0B ); //bulk erase data memory
	DelayMs( 6 ); //wait terase
}
void bulk_erase_P16F91X( unsigned char doRestore )
{
	unsigned int i;

	pic_send_14_bits( 6, 0x00, 0x0000 );//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
	for( i = 0; i < 8; i++ )
		pic_send_n_bits( 6, 0x06 );//set PC to 0x3008
	pic_send_n_bits( 6, 0x09 ); //bulk erase program memory, userid and config memory
	DelayMs( 6 ); //wait terase
	set_vdd_vpp( pictype, picfamily, 0 );//reset
	DelayMs( 10 );
	set_vdd_vpp( pictype, picfamily, 1 );
	DelayMs( 10 );
	pic_send_n_bits( 6, 0x0B ); //bulk erase data memory
	DelayMs( 6 ); //wait terase
}
void bulk_erase_P16F87( unsigned char doRestore )
{

	pic_send_14_bits( 6, 0x00, 0x0000 );//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
	pic_send_n_bits( 6, 0x1F ); //send 11111x to erase device
	DelayMs( Tprog );
}
void bulk_erase_P12F629( unsigned char doRestore )
{
	unsigned int osccal, bandgap; //for P12F629 devices...
	unsigned int i;
	unsigned char temp[2];

	for( i = 0; i < 0x3FF; i++ )
		pic_send_n_bits( 6, 0x06 );//set PC to 3FF
	osccal = pic_read_14_bits( 6, 0x04 );
	pic_send_14_bits( 6, 0x00, 0x0000 );//set PC to 0x2000
	for( i = 0; i < 7; i++ )
		pic_send_n_bits( 6, 0x06 );//2. Execute Increment Address command 7 times to advance PC to 0x2007.
	bandgap = pic_read_14_bits( 6, 0x04 ) & 0x3000;
	//a) Read and store OSCCAL and BG bits.
	pic_send_14_bits( 6, 0x00, 0x0000 );//b) Execute Load Configuration (000000).
	pic_send_n_bits( 6, 0x09 );//c) Execute Bulk Erase Program Memory (001001).
	DelayMs( Tera );//d) Wait TERA.
	pic_send_n_bits( 6, 0x0B );//e) Execute Bulk Erase Data Memory (001011).
	DelayMs( Tera );//f) Wait TERA.
	//h) Restore OSCCAL and BG bits.*/
	set_vdd_vpp( pictype, picfamily, 0 );
	DelayMs( 10 );
	if( doRestore == NORESTORE )
		return; //do not restore bandgap and osccal registers
	temp[0] = (unsigned char) (osccal & 0xFF);
	temp[1] = (unsigned char) ((osccal >> 8) & 0xFF);
	write_code( picfamily, pictype, 0x3FF, temp, 2, 3 );
	temp[0] = 0xFF;
	temp[1] = (unsigned char) ((bandgap >> 8) | 0xF);
	write_config_bits( picfamily, pictype, 0x2007, temp, 2, 3 );
}
void bulk_erase_P12F61X( unsigned char doRestore )
{
	pic_send_14_bits( 6, 0x00, 0x0000 );//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
	pic_send_14_bits( 6, 0x02, 0x3FFF ); //load data for program memory 0x3FFF << 1
	pic_send_n_bits( 6, 0x09 ); //perform bulk erase of the user memory
	DelayMs( 20 ); //wait Tera for erase to complete
	/*PGDlow();
	 set_vdd_vpp(pictype, picfamily,0);
	 set_vdd_vpp(pictype, picfamily,1);    */
}
void bulk_erase_P16F84A( unsigned char doRestore )
{
	pic_send_14_bits( 6, 0x02, 0x3FFF );
	pic_send_n_bits( 6, 0x09 ); //perform bulk erase of the program memory
	pic_send_n_bits( 6, 0x08 ); //begin programming cycle
	DelayMs( 20 ); //wait Tera for erase to complete
	pic_send_14_bits( 6, 0x00, 0x0000 );//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
	pic_send_14_bits( 6, 0x02, 0x3FFF ); //load data for program memory 0x3FFF << 1
	pic_send_n_bits( 6, 0x08 ); //begin programming cycle
	pic_send_n_bits( 6, 0x09 ); //perform bulk erase of the user memory
	DelayMs( 20 ); //wait Tera for erase to complete
	/*PGDlow();
	 set_vdd_vpp(pictype, picfamily,0);
	 set_vdd_vpp(pictype, picfamily,1);    */
	pic_send_n_bits( 6, 0x0B ); //perform bulk erase of the data memory
	pic_send_n_bits( 6, 0x08 ); //begin programming cycle
	DelayMs( 20 );
	PGDlow();
}
void bulk_erase_P12F6XX( unsigned char doRestore )
{
	pic_send_14_bits( 6, 0x00, 0x0000 );//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
	pic_send_14_bits( 6, 0x02, 0x3FFF ); //load data for program memory 0x3FFF << 1
	pic_send_n_bits( 6, 0x09 ); //perform bulk erase of the user memory
	DelayMs( 20 ); //wait Tera for erase to complete
	/*PGDlow();
	 set_vdd_vpp(pictype, picfamily,0);
	 set_vdd_vpp(pictype, picfamily,1);    */
	pic_send_n_bits( 6, 0x0B ); //perform bulk erase of the data memory
	DelayMs( 20 );
	PGDlow();
}
void bulk_erase_P16F59( unsigned char doRestore )
{

	pic_send_n_bits( 6, 0x06 ); //increment address
	pic_send_n_bits( 6, 0x09 ); //begin programming
	DelayMs( 20 );

}
void bulk_erase_P10F200( unsigned char doRestore )
{
	unsigned int osccal; //for P12F629 devices...
	unsigned char temp[2];

	if( doRestore == NORESTORE )
		set_address( picfamily, 0x100 ); //erase backup osccal as well
	pic_send_n_bits( 6, 0x06 );//increment address
	pic_send_n_bits( 6, 0x09 );//c) Execute Bulk Erase Program Memory (001001).
	DelayMs( 10 );
	if( doRestore == NORESTORE )
		return; //do not restore bandgap and osccal registers
	DelayMs( 10 );
	set_address( picfamily, 0x104 );
	osccal = pic_read_14_bits( 6, 0x04 );
	set_vdd_vpp( pictype, picfamily, 0 );
	temp[0] = (unsigned char) (osccal & 0xFF);
	temp[1] = (unsigned char) ((osccal >> 8) & 0xFF);
	write_code( picfamily, pictype, 0xFF, temp, 2, 3 );
	DelayMs( 10 );
}
void bulk_erase_P10F202( unsigned char doRestore )
{
	unsigned int osccal; //for P12F629 devices...
	unsigned char temp[2];

	if( doRestore == NORESTORE )
		set_address( picfamily, 0x200 ); //erase backup osccal as well
	pic_send_n_bits( 6, 0x06 );//increment address
	pic_send_n_bits( 6, 0x09 );//c) Execute Bulk Erase Program Memory (001001).
	DelayMs( 10 );
	if( doRestore == NORESTORE )
		return; //do not restore bandgap and osccal registers
	set_address( picfamily, 0x204 );
	osccal = pic_read_14_bits( 6, 0x04 );
	set_vdd_vpp( pictype, picfamily, 0 );
	DelayMs( 10 );
	temp[0] = (unsigned char) (osccal & 0xFF);
	temp[1] = (unsigned char) ((osccal >> 8) & 0xFF);
	write_code( picfamily, pictype, 0x1FF, temp, 2, 3 );
	DelayMs( 10 );
}
