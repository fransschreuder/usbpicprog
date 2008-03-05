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
//PERASESTATE perasestate=PERASEIDLE;
PROGSTATE progstate=PROGIDLE;
DATASTATE datastate=DATAIDLE;
CONFIGSTATE configstate=CONFIGIDLE;

long lasttick=0;
extern long tick;

/**
before calling this function, erasestate has to be ERASESTART
This function has to be called as many times until erasestate==ERASESUCCESS
*/

void bulk_erase(PICTYPE pictype,PICVARIANT picvariant)
{
	char i;
	switch(erasestate)
	{
		case ERASESTART:
                        set_vdd_vpp(1);
			erasestate=ERASE1;
			break;
		case ERASE1:
			switch(pictype)
			{
				case PIC18:
//					pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
//					pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
//					pic_send(4,0x00,0x88A6); //BSF EECON1, WREN
                                        switch(picvariant)
                                        {
                                               case 18F2XXX:           //also valid for 18F4XXX
                                                    set_address(pictype, 0x3C0005);
                                                    pic_send(4,0x0C,0x3F3F); //Write 3F3Fh to 3C0005h
					            set_address(pictype, 0x3C0004);
					            pic_send(4,0x0C,0x8F8F); //Write 8F8Fh to 3C0004h
					            break;
                                               case 18FXX2:            //also valid for 18FXX8
                                                    set_address(pictype, 0x3C0004);
                                                    pic_send(4,0x0C,0x0080);
                                                    break;
                                               default:
                                                    break;

                                        }
					pic_send(4,0x00,0x0000); //NOP
					lasttick=tick;
					pic_send(4,0x00,0x0000); //hold PGD low until erase completes

					break;   
                                case PIC16:
                                        pic_send_n_bits(6,0x1F); //send 11111x to erase device
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
			set_vdd_vpp(0);
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
before calling this function, erasestate has to be ERASESTART
This function has to be called as many times until erasestate==ERASESUCCESS
 */


/*static unsigned long paddress;
void program_erase(PICTYPE pictype)
{
	char i;
	
	
	switch(perasestate)
	{
		case PERASESTART:
			set_vdd_vpp(1);
			perasestate=PERASE1;
			paddress=0;
			break;
		case PERASE1:
			switch(pictype)
			{
				case PIC18:
	
					pic_send(4,0,0x8EA6);//	BSF EECON1, EEPGD
					pic_send(4,0,0x9CA6);//	BCF EECON1, CFGS
					pic_send(4,0,0x84A6);//	BSF EECON1, WREN
					//Step 2: Point to first row in code memory.
					set_address(pictype, paddress);
					pic_send(4,0,0x88A6);//	BSF EECON1, FREE
					pic_send(4,0,0x82A6);//	BSF EECON1, WR
					pic_send(4,0,0x0000);//	NOP hold PGC high for time P9 and low for time P10.
			
					PGC=1;
					break;   
                                case PIC16:
                                        pic_send_n_bits(6,0x1F); //send 11111x to erase device
				default:
					break;
			}
			
			perasestate=PERASE2;
			break;
		case PERASE2:
			if((tick-lasttick)>P9)
			{
				PGC=0;
				perasestate=PERASE3;
				lasttick=tick;
			}
			break;
		case PERASE3:
			if((tick-lasttick)>P10)
			{
				paddress+=64;
				if(paddress>0x7FFF)
				{
					lasttick=tick;
					perasestate=PERASESTOP;
				}
				else
				{
					perasestate=PERASE1;
				}
			}
			break;
		case PERASESTOP:
			set_vdd_vpp(0);
			if((tick-lasttick)>P10)
				perasestate=PERASESUCCESS;
			break;
		case PERASEIDLE:
			break;
		case PERASESUCCESS:
			break;
		default:
			perasestate=PERASEIDLE;
			break;
			
			
	}
}       */

/**
before calling this function, progstate must be PROGSTART
This function has to be called as many times until progstate==PROGNEXTBLOCK, then load next block and make progstate PROG2 to continue programming
or when lastblock=1:
call as many times until progstate==PROGSUCCESS
address points to the first byte of the block
data contains the data MSB0, LSB0, MSB1, LSB1, etc...
blocksize is the block syze in BYTES
lastblock is 1 if this block is the last block to program, otherwise lastblock is 0
 */
void program_memory(PICTYPE pictype, PICVARIANT picvariant, unsigned long address, char* data,char blocksize,char lastblock)
{
	char i;
	char blockcounter;
	switch(progstate)
	{
		case PROGSTART:
			set_vdd_vpp(1);

			progstate=PROG1;
			break;
		case PROG1:
			switch(pictype)
			{
				case PIC18:
					pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
					pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
					break;
				default:
					break;
			}
			progstate=PROG2;
		case PROG2:
			switch(pictype)
			{
				case PIC18:
					set_address(pictype, address);
					for(blockcounter=0;blockcounter<(blocksize-2);blockcounter+=2)
					{

						//write 2 bytes and post increment by 2
						//				MSB				LSB
						pic_send(4,0x0D,((unsigned int)*(data+blockcounter))<<8|
								((unsigned int)*(data+1+blockcounter)));
					//	pic_send(4,0x0D,0xAA55);
					}
					//write last 2 bytes of the block and start programming
					pic_send(4,0x0F,((unsigned int)*(data+blockcounter))<<8|((unsigned int)*(data+1+blockcounter))); 
					//pic_send(4, 0x0F,0x55AA);
					/*
					PGC => 1 0 1 0 1 0 1 (then stay high for P9)
					       b0  b1  b2
					*/
                                        pic_send_n_bits(3, 0);
					break;
				default:
					break;
			}
			lasttick=tick;
			PGC=1;	//hold PGC high for P9
			progstate=PROG3;
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
			set_vdd_vpp(0);
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
void program_data(PICTYPE pictype, PICVARIANT picvariant, unsigned int address, char* data, char blocksize)
{
	char i;
	char blockcounter;
	char receiveddata;
	switch(datastate)
	{
		case DATASTART:
			set_vdd_vpp(1);

			datastate=DATA;
			break;
		case DATA:
			switch(pictype)
			{
				case PIC18:
					pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
					pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
					for(blockcounter=0;blockcounter<blocksize;blockcounter++)
					{
						pic_send(4,0x00,(unsigned int)(0x0E00|((address+blockcounter)&0xFF))); //MOVLW EEAddr
						pic_send(4,0x00,0x6EA9); //MOVWF EEADR
						pic_send(4,0x00,(unsigned int)(0x0E00| (((address+ (unsigned int)blockcounter)>>8)&0xFF))); //MOVLW EEAddrH
						pic_send(4,0x00,0x6EAA); //MOVWF EEADRH
						pic_send(4,0x00,0x0E00|(unsigned int)*(data+blockcounter)); //MOVLW data
						pic_send(4,0x00,0x6eA8); //MOVWF EEDATA
						pic_send(4,0x00,0x84A6); //BSF EECON1, WREN
						pic_send(4,0x00,0x82A6); //BSF EECON1, WR
						i=0;
						do
						{
							pic_send(4,0x00,0x50A6); //movf EECON1, W, 0
							pic_send(4,0x00,0x6EF5); //movwf TABLAT
							pic_send(4,0x00,0x0000); //nop
							receiveddata=pic_read(4,0x02); //Shift TABLAT register out
						}while((receiveddata&0x02)&&(i++<255)); //poll for WR bit to clear
						PGC=0;	//hold PGC low for P10 (100us)
						for(i=0;i<200;i++)continue;
						pic_send(4,0x00,0x94A6); //BCF EECON1, WREN
					}
					break;
				default:
					break;
			}
			datastate=DATASTOP;
			lasttick=tick;
		case DATASTOP:
			set_vdd_vpp(0);
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
void program_config_bits(PICTYPE pictype, PICVARIANT picvariant, unsigned long address, char* data)
{
	char i;
	static char blockcounter;
	switch(configstate)
	{
		case CONFIGSTART:
			blockcounter=0;
			set_vdd_vpp(1);

			configstate=CONFIG1;
			break;
		case CONFIG1:
			switch(pictype)
			{
				case PIC18:
					pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
					pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
					break;
				default:
					break;
			}
			progstate=CONFIG2;
		case CONFIG2:
			switch(pictype)
			{
				case PIC18:
					set_address(pictype, address);
					//LSB first
					pic_send(4,0x0F,(unsigned int)*(data)); 
					pic_send(4,0x00,0x0000); //nop
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
			pic_send(4,0x00, 0x0E00|((unsigned int)(address&0xFF)+1)); //movlw 0x01 + address
			pic_send(4,0x00, 0x6EF6); //movwf TBLPTRL
			pic_send(4,0x0F, ((unsigned int)*(data+1))<<8); //load MSB and start programming
			pic_send(4,0x00, 0x0000); //nop
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
			set_vdd_vpp(0);
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
void read_program(PICTYPE pictype, PICVARIANT picvariant, unsigned long address, char* data, char blocksize, char lastblock)
{
	char i;
	char blockcounter=0;
	if(lastblock&1)set_vdd_vpp(1);
	switch(pictype)
	{
		case PIC18:
                        set_address(pictype, address);
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
				*(data+blockcounter)=pic_read(4,0x09);
			break;
		default:
			break;
	}
	if(lastblock&2)set_vdd_vpp(0);

}


/**
This function reads a block of data from the data eeprom of size blocksize into *data
call this function only once.
**/
void read_data(PICTYPE pictype, PICVARIANT picvariant, unsigned int address, char* data, char blocksize)
{
	
	char i;
	char blockcounter=0;
	set_vdd_vpp(1);
	switch(pictype)
	{
		case PIC18:
			pic_send(4,0x00,0x9EA6); //BCF EECON1, EEPGD
			pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
			{
				pic_send(4,0x00,(unsigned int)(0x0E00|(((address+blockcounter)>>8)&0xFF))); //MOVLW Addr [15:8]
				pic_send(4,0x00,0x6EA9); //MOVWF EEADR
				pic_send(4,0x00,(unsigned int)(0x0E00|((address+blockcounter)&0xFF))); //MOVLW Addr [7:0]
				pic_send(4,0x00,0x6EAA); //MOVWF TBLPTRU
				pic_send(4,0x00,0x80A6); //BSF EECON1, RD
				pic_send(4,0x00,0x50A6); //MOVF EEDATA, W, 0
				pic_send(4,0x00,0x6EF5); //MOVWF TABLAT
				pic_send(4,0x00,0x0000); //Nop
				*(data+blockcounter)=pic_read(4,0x02);
			}
			break;
		default:
			break;
	}
	set_vdd_vpp(0);
}


/**
reads 8 bits from a pic device with a given cmd_size bits command
**/
char pic_read(char cmd_size, char command)
{
	char i;
	char result;
	pic_send_n_bits(cmd_size,command);
	for(i=0;i<80;i++)continue;	//wait at least 1us
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
	for(i=0;i<40;i++)continue;
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
	clock_delay();
	return result;
}

void set_vdd_vpp(char level)
{
    unsigned int i;
    if(level==1)
    {
        VDD=0; //high, (inverted)
        lasttick=tick;
	setLeds(1);
	while((tick-lasttick)<20)continue;
        VPP=0; //high, (inverted)
	setLeds(2);
	lasttick=tick;
	while((tick-lasttick)<20)continue;
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

void set_address(PICTYPE pictype, unsigned long address)
{
	switch(pictype)
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


/*void clock_delay()
{
	//char i;
	//for(i=0;i<2;i++)continue;
	Nop(); //would that be enough for all pic's?
}*/


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
	for(i=0;i<40;i++)continue;	//wait at least 1 us
}


/**
Writes a n-bit command + 16 bit payload to a pic device
**/
void pic_send(char cmd_size, char command, unsigned int payload)
{
	char i;
	pic_send_n_bits(cmd_size,command);
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
