/*********************************************************************
 *
 *                UsbPicProg v0.1
 *                Frans Schreuder 12-03-2008
 ********************************************************************/
 
#include "prog_lolvl.h"
#ifdef SDCC
#include <pic18f2550.h>
#else
#include <p18cxxx.h>
#endif
#include "system\typedefs.h"
#include "system\interrupt\interrupt.h"
#include "prog.h"
#include "upp.h" 
#include "io_cfg.h"             // I/O pin mapping

extern long tick;
extern long lasttick;

void set_vdd_vpp(PICFAMILY picfamily,char level)
{
    unsigned int i;
    if(level==1)
    {
		if(picfamily==PIC18)VDD=0; //high, (inverted)
		else VPP=0;
		lasttick=tick;
		setLeds(1);
		while((tick-lasttick)<100)continue;
		if(picfamily==PIC18)VPP=0; //high, (inverted)
		else VDD=0;
		setLeds(2);
		lasttick=tick;
		while((tick-lasttick)<100)continue;
		setLeds(3);
    }
    else
    {
		VPP=1; //low, (inverted)
		setLeds(4);
		lasttick=tick;
		while((tick-lasttick)<20)continue;
		setLeds(5);
		VDD=1; //low, (inverted)
		lasttick=tick;
		while((tick-lasttick)<20)continue;
		setLeds(0);
    }
}

void set_address(PICFAMILY picfamily, unsigned long address)
{
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
		case PIC16:
			pic_send(6,0x02,((unsigned int)address)<<1);
		default:
			break;
	}
}

/*
void clock_delay()
{
	//char i;
	//for(i=0;i<2;i++)continue;
	//Nop();
	//Nop(); //would that be enough for all pic's?
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
	Nop();
}
*/


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

		PGC=1;
		clock_delay();
		if(command&1)PGD=1;
		else PGD=0;
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

		PGC=1;
		clock_delay();
		if(payload&1)PGD=1;
		else PGD=0;
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
	PGC=1;
	clock_delay();
	PGD=0;
	clock_delay();
	PGC=0;
	clock_delay();
	for(i=0;i<14;i++)
	{

		PGC=1;
		clock_delay();
		if(payload&1)PGD=1;
		else PGD=0;
		payload>>=1;
		clock_delay();
		PGC=0;
		clock_delay();

	}
	PGC=1;
	clock_delay();
	PGD=0;
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

		PGC=1;
		clock_delay();
		PGD=0;
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
