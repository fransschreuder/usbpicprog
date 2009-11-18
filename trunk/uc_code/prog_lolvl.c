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
 
#include "prog_lolvl.h"
#ifdef SDCC
#include <pic18f2550.h>
#else
#include <p18cxxx.h>
#endif
#include "typedefs.h"
#include "interrupt.h"
#include "prog.h"
#include "upp.h" 
#include "io_cfg.h"             // I/O pin mapping

extern long tick;
extern long lasttick;

void set_vdd_vpp(PICTYPE pictype, PICFAMILY picfamily,char level)
{
	unsigned int i;
	if(level==1)
	{
		TRISPGD =0;    //PGD output
		TRISPGC =0;    //PGC output
		PGD =0;        // initial value for programming mode
		PGC =0;        // initial value for programming mode
		clock_delay();    // dummy tempo
		switch(pictype)
		{		
			case dsP30F:
				break;
			case P12F629:
			case P12F6XX:
				VPP=0;
				break;
			default:
				VDD=0; //high, (inverted)
				break;
		}
		lasttick=tick;
		while((tick-lasttick)<100)continue;
		switch(pictype)
		{
			case P12F629:
			case P12F6XX:
				VDD=0;
				break;
			case dsP30F:
				VDD=0;
				clock_delay();
				VPP=0;
				lasttick=tick;
				while((tick-lasttick)<26)continue;
				dspic_send_24_bits(0);
				dspic_send_24_bits(0);
				dspic_send_24_bits(0);
				dspic_send_24_bits(0);
				VPP=1;
				VPP_RST=1;
				VPP_RST=0;
				for(i=0;i<1;i++)continue;
				VPP=0;
				break;
			default:
				VPP=0; //high, (inverted)
				break;
		}
		lasttick=tick;
		while((tick-lasttick)<100)continue;
	}
	else
	{
		VPP=1; //low, (inverted)
		VPP_RST=1; //hard reset, low (inverted)
		lasttick=tick;
		while((tick-lasttick)<40)continue;
		VPP_RST=0; //hard reset, high (inverted)
		VDD=1; //low, (inverted)
		TRISPGD =1;    //PGD input
		TRISPGC =1;    //PGC input
		lasttick=tick;
		while((tick-lasttick)<20)continue;
	}
}

void set_address(PICFAMILY picfamily, unsigned long address)
{
	unsigned long i;
	switch(picfamily)
	{
		case PIC18:
			pic_send(4,0x00,(unsigned int)(0x0E00|((address>>16)&0xFF))); //MOVLW Addr [23:16]
			pic_send(4,0x00,0x6EF8); //MOVWF TBLPTRU
			pic_send(4,0x00,(unsigned int)(0x0E00|((address>>8)&0xFF))); //MOVLW Addr [15:8]
			pic_send(4,0x00,0x6EF7); //MOVWF TBLPTRU
			pic_send(4,0x00,(unsigned int)(0x0E00|((address)&0xFF))); //MOVLW Addr [7:0]
			pic_send(4,0x00,0x6EF6); //MOVWF TBLPTRU
			break;
		case PIC10:
		case PIC16:
			for(i=0;i<address;i++)
				pic_send_n_bits(6,0x06);	//increment address
		default:
			break;
	}
}

/**
Writes a n-bit command
**/
void pic_send_n_bits(char cmd_size, char command)
{
	char i;
	TRISPGD=0;
	TRISPGC=0;
	PGC=0;
	PGD=0;
	for(i=0;i<cmd_size;i++)
	{
		if(command&1)PGD=1;
		else PGD=0;
		PGC=1;		
		command>>=1;
		clock_delay();
		PGC=0;
		clock_delay();

	}
	for(i=0;i<10;i++)continue;	//wait at least 1 us <<-- this could be tweaked to get the thing faster
}

void pic_send_word(unsigned int payload)
{
	char i;
	for(i=0;i<16;i++)
	{
		if(payload&1)PGD=1;
		else PGD=0;
		PGC=1;
		payload>>=1;
		clock_delay();
		PGC=0;
		clock_delay();
	}
	clock_delay();
}

void pic_send_word_14_bits(unsigned int payload)
{
	char i;
	PGD=0;
	clock_delay();
	PGC=1;
	clock_delay();
	PGC=0;
	clock_delay();
	for(i=0;i<14;i++)
	{
		if(payload&1)PGD=1;
		else PGD=0;
		PGC=1;		
		payload>>=1;
		clock_delay();
		PGC=0;
		clock_delay();

	}
	PGD=0;
	clock_delay();
	PGC=1;
	clock_delay();
	PGC=0;
	clock_delay();
	clock_delay();
}

/**
Writes a n-bit command + 16 bit payload to a pic18 device
**/
void pic_send(char cmd_size, char command, unsigned int payload)
{
	pic_send_n_bits(cmd_size,command);
	pic_send_word(payload);
	PGD = 0;      //  <=== Must be low at the end, at least when VPP and VDD go low.
	
}

/**
Writes a n-bit command + 14 bit payload to a pic16 device
**/
void pic_send_14_bits(char cmd_size,char command, unsigned int payload)
{
	pic_send_n_bits(cmd_size,command);
	pic_send_word_14_bits(payload);
	PGD = 0;      //  <=== Must be low at the end, at least when VPP and VDD go low.
}

unsigned int pic_read_14_bits(char cmd_size, char command)
{
	char i;
	unsigned int result;
	pic_send_n_bits(cmd_size,command);
//	for(i=0;i<80;i++)continue;	//wait at least 1us
	TRISPGD=1; //PGD = input
	for(i=0;i<10;i++)continue;
	result=0;
	PGC=1;
	clock_delay();
	PGC=0;
	clock_delay();
	for(i=0;i<14;i++)
	{

		PGC=1;
		clock_delay();
		result|=((unsigned int)PGD_READ)<<i;
		clock_delay();
		PGC=0;
		clock_delay();
	}
	PGC=1;
	clock_delay();
	PGC=0;
	clock_delay();
	TRISPGD=0; //PGD = output
	PGD=0;
	clock_delay();
	return result;
}


/**
reads 8 bits from a pic device with a given cmd_size bits command
**/
char pic_read_byte2(char cmd_size, char command)
{
	char i;
	char result;
	pic_send_n_bits(cmd_size,command);
//	for(i=0;i<80;i++)continue;	//wait at least 1us
	for(i=0;i<8;i++)
	{
		PGD=0;
		clock_delay();
		PGC=1;
		clock_delay();
		PGC=0;
		clock_delay();
	}
	TRISPGD=1; //PGD = input
	for(i=0;i<10;i++)continue;
	result=0;
	for(i=0;i<8;i++)
	{

		PGC=1;
		clock_delay();
		result|=((char)PGD_READ)<<i;
		clock_delay();
		PGC=0;
		clock_delay();
	}
	TRISPGD=0; //PGD = output
	PGD=0;
	clock_delay();
	return result;
}

/// read a 16 bit "word" from a dsPIC
unsigned int dspic_read_16_bits(void)
{
	char i;
	unsigned int result;
	PGD=0;
	PGD=1;	//send 1
	PGC=1;	//clock pulse
	PGC=0;
	PGD=0;	//send 3 zeroes
	for(i=0;i<3;i++)
	{
		PGC=1;
		PGC=0;
	}
	//pic_send_n_bits(4,1);
	result=0;
	for(i=0;i<8;i++)
	{
		PGC=1;
		PGC=0;
	}
	//pic_send_n_bits(8,0);
	TRISPGD=1; //PGD = input
	clock_delay();
	for(i=0;i<16;i++)
	{
		PGC=1;
		clock_delay();
		result|=((unsigned int)PGD_READ)<<i;
		PGC=0;
	}
	TRISPGD=0; //PGD = output
	PGD=0;
	return result;
}


void dspic_send_24_bits(unsigned long payload)
{
	unsigned char i;
	PGD=0;
	for(i=0;i<4;i++)
	{
		PGC=1;
		PGC=0;
	}
	for(i=0;i<24;i++)
	{
		
		if(payload&1)PGD=1;
		else PGD=0;
		payload>>=1;
		clock_delay();
		PGC=1;
		clock_delay();
		PGC=0;
	}
}



