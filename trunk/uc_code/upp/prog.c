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

ERASESTATE erasestate=ERASEIDLE;
PROGSTATE progstate=PROGIDLE;
DATASTATE datastate=DATAIDLE;
CONFIGSTATE configstate=CONFIGIDLE;

long lasttick=0;
extern long tick;

/**
before calling this function, erasestate has to be ERASESTART
This function has to be called as many times until erasestate==ERASESUCCESS
*/

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
		case ERASEIDLE:
			break;
		case ERASESUCCESS:
			break;
		default:
			erasestate=ERASEIDLE;
			break;
			
			
	}
}



/**
before calling this function, progstate must be PROGSTART
This function has to be called as many times until progstate==PROGNEXTBLOCK
or when lastblock=1:
call as many times until progstate==PROGSUCCESS
*/
void program_memory(PICTYPE pictype,unsigned long address, char* data,char blocksize,char lastblock)
{
	char i;
	char blockcounter;
	switch(progstate)
	{
		case PROGSTART:
			VDD=0; //high, (inverted)
			for(i=0;i<10;i++)continue; //wait at least 100 ns;
			VPP=0; //high, (inverted)
			for(i=0;i<10;i++)continue; //wait at least 2 us;

			progstate=PROG1;
			break;
		case PROG1:
			switch(pictype)
			{
				case PIC18:
					pic18_send(0x00,0x8EA6); //BSF EECON1, EEPGD
					pic18_send(0x00,0x9CA6); //BCF EECON1, CFGS
					break;
				default:
					break;
			}
			progstate=PROG2;
		case PROG2:
			switch(pictype)
			{
				case PIC18:
					pic18_send(0x00,(unsigned int)(0x0E00|((address>>16)&0x3F))); //MOVLW Addr [21:16]
					pic18_send(0x00,0x6EF8); //MOVWF TBLPTRU
					pic18_send(0x00,(unsigned int)(0x0E00|((address>>8)&0xFF))); //MOVLW Addr [15:8]
					pic18_send(0x00,0x6EF7); //MOVWF TBLPTRU
					pic18_send(0x00,(unsigned int)(0x0E00|((address)&0xFF))); //MOVLW Addr [7:0]
					pic18_send(0x00,0x6EF6); //MOVWF TBLPTRU
					for(blockcounter=0;blockcounter<(blocksize-2);blockcounter+=2)
					{

						//write 2 bytes and post increment by 2
						//				MSB				LSB
						pic18_send(0x0D,((unsigned int)*(data+blockcounter))<<8|((unsigned int)*(data+1+blockcounter))); 
					}
					//write last 2 bytes of the block and start programming
					pic18_send(0x0F,((unsigned int)*(data+blockcounter))<<8|((unsigned int)*(data+1+blockcounter))); 
					pic18_send(0x00, 0x0000); //nop, hold PGC high for time P9 and low for P10
					break;
				default:
					break;
			}
			lasttick=tick;
			PGC=1;	//hold PGC high for P9
			progstate=PROG2;
			break;
		case PROG3:
			if((tick-lasttick)>P9)
			{
				progstate=PROG4;
				PGC=0;	//hold PGC low for time P10
				lasttick=tick;
			}
			break;
		case PROG4:
			if((tick-lasttick)>P10)
			{
				if(lastblock==0)progstate=PROGNEXTBLOCK;
				else 
				{	
					progstate=PROGSTOP;
					lasttick=tick;
				}
			}
		case PROGNEXTBLOCK:		
			/**
			the higher level program has to check for this state and load the next block
			then make progstate PROG2 to continue the next block
			**/
			break;
		case PROGSTOP:
			VPP=1; //low, (inverted)
			Nop();
			VDD=1; //low, (inverted)
			if((tick-lasttick)>P10)
				progstate=PROGSUCCESS;
			break;
		case PROGIDLE:
			break;
		case PROGSUCCESS:
			break;
		default:
			progstate=PROGIDLE;
			break;
	}
}


/**
before calling this function, datastate must be DATASTART
call as many times until progstate==PROGSUCCESS
 */
void program_data(PICTYPE pictype,unsigned int address, char* data, char blocksize)
{
	char i;
	char blockcounter;
	char receiveddata;
	switch(datastate)
	{
		case DATASTART:
			VDD=0; //high, (inverted)
			for(i=0;i<10;i++)continue; //wait at least 100 ns;
			VPP=0; //high, (inverted)
			for(i=0;i<10;i++)continue; //wait at least 2 us;

			datastate=DATA;
			break;
		case DATA:
			switch(pictype)
			{
				case PIC18:
					pic18_send(0x00,0x8EA6); //BSF EECON1, EEPGD
					pic18_send(0x00,0x9CA6); //BCF EECON1, CFGS
					for(blockcounter=0;blockcounter<blocksize;blockcounter++)
					{
						pic18_send(0x00,(unsigned int)(0x0E00|((address+blockcounter)&0xFF))); //MOVLW EEAddr
						pic18_send(0x00,0x6EA9); //MOVWF EEADR
						pic18_send(0x00,(unsigned int)(0x0E00| (((address+ (unsigned int)blockcounter)>>8)&0xFF))); //MOVLW EEAddrH
						pic18_send(0x00,0x6EAA); //MOVWF EEADRH
						pic18_send(0x00,0x0E00|(unsigned int)*(data+blockcounter)); //MOVLW data
						pic18_send(0x00,0x6eA8); //MOVWF EEDATA
						pic18_send(0x00,0x84A6); //BSF EECON1, WREN
						pic18_send(0x00,0x82A6); //BSF EECON1, WR
						i=0;
						do
						{
							pic18_send(0x00,0x50A6); //movf EECON1, W, 0
							pic18_send(0x00,0x6EF5); //movwf TABLAT
							pic18_send(0x00,0x0000); //nop
							receiveddata=pic18_read(0x02); //Shift TABLAT register out
						}while((receiveddata&0x02)&&(i++<255)); //poll for WR bit to clear
						PGC=0;	//hold PGC low for P10 (100us)
						for(i=0;i<200;i++)continue;
						pic18_send(0x00,0x94A6); //BCF EECON1, WREN
					}
					break;
				default:
					break;
			}
			datastate=DATASTOP;
			lasttick=tick;
		case DATASTOP:
			VPP=1; //low, (inverted)
			Nop();
			VDD=1; //low, (inverted)
			if((tick-lasttick)>P10)
				datastate=DATASUCCESS;
			break;
		case DATAIDLE:
			break;
		case DATASUCCESS:
			break;
		default:
			progstate=DATAIDLE;
			break;
	}	
}
/**
use program_memory in stead with address=0x200000 and blocksize=8
void program_ids(PICTYPE pictype,char address, char* data, char blocksize)
{
}
**/

/**
program_config_bits writes 2 configuration bytes each time (*data has to be 2 bytes big)
the address will be 0x300000 + the id location
before calling this function, make configstate CONFIGSTART
keep calling this function until configstate==CONFIGSUCCESS
**/
void program_config_bits(PICTYPE pictype,unsigned long address, char* data)
{
	char i;
	static char blockcounter;
	switch(configstate)
	{
		case CONFIGSTART:
			blockcounter=0;
			VDD=0; //high, (inverted)
			for(i=0;i<10;i++)continue; //wait at least 100 ns;
			VPP=0; //high, (inverted)
			for(i=0;i<10;i++)continue; //wait at least 2 us;

			configstate=CONFIG1;
			break;
		case CONFIG1:
			switch(pictype)
			{
				case PIC18:
					pic18_send(0x00,0x8EA6); //BSF EECON1, EEPGD
					pic18_send(0x00,0x9CA6); //BCF EECON1, CFGS
					break;
				default:
					break;
			}
			progstate=CONFIG2;
		case CONFIG2:
			switch(pictype)
			{
				case PIC18:
					pic18_send(0x00,(unsigned int)(0x0E00|((address>>16)&0x3F))); //MOVLW Addr [21:16]
					pic18_send(0x00,0x6EF8); //MOVWF TBLPTRU
					pic18_send(0x00,(unsigned int)(0x0E00|((address>>8)&0xFF))); //MOVLW Addr [15:8]
					pic18_send(0x00,0x6EF7); //MOVWF TBLPTRU
					pic18_send(0x00,(unsigned int)(0x0E00|((address)&0xFF))); //MOVLW Addr [7:0]
					pic18_send(0x00,0x6EF6); //MOVWF TBLPTRU
					//LSB first
					pic18_send(0x0F,(unsigned int)*(data)); 
					pic18_send(0x00,0x0000); //nop
					break;
				default:
					break;
			}
			lasttick=tick;
			PGC=1;	//hold PGC high for P9
			configstate=CONFIG2;
			break;
		case CONFIG3:
			if((tick-lasttick)>P9)
			{
				configstate=CONFIG4;
				PGC=0;	//hold PGC low for time P10
				for(i=0;i<200;i++)continue; //delay P10=100us
			}
			break;
		case CONFIG4:
			pic18_send(0x00, 0x0E00|((unsigned int)(address&0xFF)+1)); //movlw 0x01 + address
			pic18_send(0x00, 0x6EF6); //movwf TBLPTRL
			pic18_send(0x0F, ((unsigned int)*(data+1))<<8); //load MSB and start programming
			pic18_send(0x00, 0x0000); //nop
			lasttick=tick;
			PGC=1;	//hold PGC high for P9
			configstate=CONFIG5;
		case CONFIG5:
			if((tick-lasttick)>P9)
			{
				configstate=CONFIGSTOP;
				PGC=0;	//hold PGC low for time P10
				for(i=0;i<200;i++)continue; //delay P10=100us
				lasttick=tick;
			}
			break;
		case CONFIGSTOP:
			VPP=1; //low, (inverted)
			Nop();
			VDD=1; //low, (inverted)
			if((tick-lasttick)>P10)
				progstate=CONFIGSUCCESS;
			break;
		case CONFIGIDLE:
			break;
		case CONFIGSUCCESS:
			break;
		default:
			progstate=CONFIGIDLE;
			break;
	}
}

/**
This function has to be called only once per block
read_program will read program memory, id's and configuration bits
**/
void read_program(PICTYPE pictype,unsigned long address, char* data, char blocksize)
{
	char i;
	char blockcounter=0;
	VDD=0; //high, (inverted)
	for(i=0;i<10;i++)continue; //wait at least 100 ns;
	VPP=0; //high, (inverted)
	for(i=0;i<10;i++)continue; //wait at least 2 us;
	switch(pictype)
	{
		case PIC18:
			pic18_send(0x00,(unsigned int)(0x0E00|((address>>16)&0x3F))); //MOVLW Addr [21:16]
			pic18_send(0x00,0x6EF8); //MOVWF TBLPTRU
			pic18_send(0x00,(unsigned int)(0x0E00|((address>>8)&0xFF))); //MOVLW Addr [15:8]
			pic18_send(0x00,0x6EF7); //MOVWF TBLPTRU
			pic18_send(0x00,(unsigned int)(0x0E00|((address)&0xFF))); //MOVLW Addr [7:0]
			pic18_send(0x00,0x6EF6); //MOVWF TBLPTRU
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
				*(data+blockcounter)=pic18_read(0x09);
			break;
		default:
			break;
	}
	VPP=1; //low, (inverted)
	Nop();
	VDD=1; //low, (inverted)
	Nop();
	
}


/**
This function reads a block of data from the data eeprom of size blocksize into *data
call this function only once.
**/
void read_data(PICTYPE pictype,unsigned int address, char* data, char blocksize)
{
	
	char i;
	char blockcounter=0;
	VDD=0; //high, (inverted)
	for(i=0;i<10;i++)continue; //wait at least 100 ns;
	VPP=0; //high, (inverted)
	for(i=0;i<10;i++)continue; //wait at least 2 us;
	switch(pictype)
	{
		case PIC18:
			pic18_send(0x00,0x9EA6); //BCF EECON1, EEPGD
			pic18_send(0x00,0x9CA6); //BCF EECON1, CFGS
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
			{
				pic18_send(0x00,(unsigned int)(0x0E00|(((address+blockcounter)>>8)&0xFF))); //MOVLW Addr [15:8]
				pic18_send(0x00,0x6EA9); //MOVWF EEADR
				pic18_send(0x00,(unsigned int)(0x0E00|((address+blockcounter)&0xFF))); //MOVLW Addr [7:0]
				pic18_send(0x00,0x6EAA); //MOVWF TBLPTRU
				pic18_send(0x00,0x80A6); //BSF EECON1, RD
				pic18_send(0x00,0x50A6); //MOVF EEDATA, W, 0
				pic18_send(0x00,0x6EF5); //MOVWF TABLAT
				pic18_send(0x00,0x0000); //Nop
				*(data+blockcounter)=pic18_read(0x02);
			}
			break;
		default:
			break;
	}
	VPP=1; //low, (inverted)
	Nop();
	VDD=1; //low, (inverted)
	Nop();
}


/**
reads 8 bits from a pic18 device with a given 4 bits command
**/
char pic18_read(char command)
{
	char i;
	char result;
	TRISPGD=0;
	TRISPGC=0;
	PGC=0;
	PGD=0;
	for(i=0;i<4;i++)
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
	for(i=0;i<8;i++)
	{
		
		PGC=1;
		Nop();
		PGD=0;
		Nop();
		PGC=0;
		Nop();
	}
	TRISPGD=1; //PGD = input
	Nop();
	result=0;
	for(i=0;i<8;i++)
	{
		
		PGC=1;
		Nop();
		result|=((char)PGD_READ)<<i;
		Nop();
		PGC=0;
		Nop();
	}
	TRISPGD=0; //PGD = output
	Nop();
	return result;
}


/**
Writes a 4-bit command + 16 bit payload to a pic18 device
**/
void pic18_send(char command, unsigned int payload)
{
	char i;
	TRISPGD=0;
	TRISPGC=0;
	PGC=0;
	PGD=0;
	for(i=0;i<4;i++)
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
	for(i=0;i<16;i++)
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