/*********************************************************************
 *
 *                UsbPicProg v0.1
 *                Frans Schreuder 23-02-2008
 ********************************************************************/
#ifdef SDCC
#include <pic18f2550.h>
#else
#include <p18cxxx.h>
#endif
#include "system\typedefs.h"
#include "io_cfg.h"             // I/O pin mapping
#include "prog.h"
#include "system\interrupt\interrupt.h"

static ERASESTATE erasestate=ERASEIDLE;
static long lasttick=0;
extern long tick;
void bulk_erase(PICTYPE pictype)
{
	char i;
	switch(erasestate)
	{
		case ERASESTART:
			VDD=0; //high, (inverted)
			for(i=0;i<10;i++)continue; //wait at least 100 ns;
			VPP=0; //high, (inverted)
			for(i=0;i<10;i++)continue; //wait at least 2 us;

			erasestate=ERASE1;
			break;
		case ERASE1:
			switch(pictype)
			{
				case PIC18:
					pic18_send(0x00,0x0E3C); //MOVLW 3Ch
					pic18_send(0x00,0x6eF8); //TBLPTRU
					pic18_send(0x00,0x0E00); //MOVLW 00h
					pic18_send(0x00,0x6eF7); //TBLPTRH
					pic18_send(0x00,0x0E05); //MOVLW 05h
					pic18_send(0x00,0x6eF6); //TBLPTRL
					pic18_send(0x0C,0x3F3F); //Write 3F3Fh to 3C0005h
					pic18_send(0x00,0x0E3C); //MOVLW 3Ch
					pic18_send(0x00,0x6eF8); //TBLPTRU
					pic18_send(0x00,0x0E00); //MOVLW 00h
					pic18_send(0x00,0x6eF7); //TBLPTRH
					pic18_send(0x00,0x0E04); //MOVLW 04h
					pic18_send(0x00,0x6EF6); //TBLPTRL
					pic18_send(0x0C,0x8FF8); //TBLPTRU
					pic18_send(0x00,0x0000); //NOP
					lasttick=tick;
					pic18_send(0x00,0x0000); //hold PGD low until erase completes
					
					break;
				default:
					break;
			}
			
			erasestate=ERASE2;
			break;
		case ERASE2:
			if((tick-lasttick)>P11)
			{
				erasestate=ERASESTOP;
				lasttick=tick;
			}
			break;
		case ERASESTOP:
			VPP=1; //low, (inverted)
			Nop();
			VDD=1; //low, (inverted)
			if((tick-lasttick)>P10)
				erasestate=ERASESUCCESS;
			break;
		default:
			erasestate=ERASEIDLE;
			break;
			
			
	}
}


void pic18_send(char command, unsigned int payload)
{
	char i;
	TRISPGD=0;
	TRISPGC=0;
	PGC=0;
	PGD=0;
	for(i=0;i<3;i++)
	{
		
		PGC=1;
		Nop();
		if(command&(1<<i))PGD=1;
		else PGD=0;
		Nop();
		PGC=0;
		Nop();
		
	}
	Nop();	//wait at least 40ns
	for(i=0;i<15;i++)
	{
		
		PGC=1;
		Nop();
		if(payload&(1<<i))PGD=1;
		else PGD=0;
		Nop();
		PGC=0;
		Nop();
		
	}
	Nop();
	
}

void program_memory(PICTYPE pictype,char address, char length, char* data)
{
}

void program_ids(PICTYPE pictype,char address, char length, char* data)
{
}
void program_data_ee(PICTYPE pictype,char address, char length, char* data)
{
}
char verify_program(PICTYPE pictype,char address, char length, char* data)
{
}
char verify_ids(PICTYPE pictype,char address, char length, char* data)
{
}
char verify_data(PICTYPE pictype,char address, char length, char* data)
{
}
void program_config_bits(PICTYPE pictype,char address, char length, char* data)
{
}