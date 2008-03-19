/*********************************************************************
 *
 *                UsbPicProg v0.1
 *                Frans Schreuder 23-02-2008
 ********************************************************************/
#include "upp.h" 
#ifdef SDCC
#include <pic18f2550.h>
#else
#include <p18cxxx.h>
#endif
#include "system\typedefs.h"
#include "io_cfg.h"             // I/O pin mapping
#include "prog.h"
#include "system\interrupt\interrupt.h"
#include "prog_lolvl.h"

extern long lasttick;
extern long tick;

/**
before calling this function, erasestate has to be ERASESTART
This function has to be called as many times until erasestate==ERASESUCCESS
*/

char bulk_erase(PICTYPE pictype,PICVARIANT picvariant)
{
	unsigned int i;	
	switch(picvariant)
	{
		case P18F2XXX:           //also valid for 18F4XXX
			set_vdd_vpp(pictype,1);
			set_address(pictype, 0x3C0005);
			pic_send(4,0x0C,0x3F3F); //Write 3F3Fh to 3C0005h
			set_address(pictype, 0x3C0004);
			pic_send(4,0x0C,0x8F8F); //Write 8F8Fh to 3C0004h
			pic_send(4,0x00,0x0000); //NOP
			lasttick=tick;
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(P11);
			set_vdd_vpp(pictype,0);
			break;
		case P18FXX2:            //also valid for 18FXX8
			set_vdd_vpp(pictype,1);
			set_address(pictype, 0x3C0004);
			pic_send(4,0x0C,0x0080);
			pic_send(4,0x00,0x0000); //NOP
			lasttick=tick;
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(P11);
			set_vdd_vpp(pictype,0);
			break;
		case P16F87XA:
			set_vdd_vpp(pictype,1);
			pic_send_n_bits(6,0x1F); //send 11111x to erase device
			set_vdd_vpp(pictype,0);
			break;
		case P16F62XA:
			set_vdd_vpp(pictype,1);
			//why does this stupid PIC not have the set_address command???
			for(i=0;i<0x2000;i++)
				pic_send_n_bits(6,0x06); //increase the adress until 0x2000
			pic_send_14_bits(6,0x02,0x3FFF); //load data for program memory 0x3FFF << 1
			pic_send_n_bits(6,0x09); //perform bulk erase of the program memory
			DelayMs(Tera); //wait Tera for erase to complete
			PGD=0;
			set_vdd_vpp(pictype,0);
			set_vdd_vpp(pictype,1);
			pic_send_n_bits(6,0x0B); //perform bulk erase of the data memory
			DelayMs(Tera);
			PGD=0;
			set_vdd_vpp(pictype,0);
			break;
		case P16F62X:
			set_vdd_vpp(pictype,1);
			//why does this stupid PIC not have the set_address command???
			for(i=0;i<0x2000;i++)
				pic_send_n_bits(6,0x06); //increase the adress until 0x2000
			pic_send_14_bits(6,0x02,0x3FFF); //load data for program memory 0x3FFF << 1
			pic_send_n_bits(6,0x09); //perform bulk erase of the program memory
			pic_send_n_bits(6,0x08); //perform begin erase / program cycle
			DelayMs(Tera); //wait Tera for erase to complete
			PGD=0;
			set_vdd_vpp(pictype,0);
			set_vdd_vpp(pictype,1);
			pic_send_14_bits(6,0x02,0x3FFF); //load data for program memory 0x3FFF << 1
			pic_send_n_bits(6,0x0B); //perform bulk erase of the data memory
			pic_send_n_bits(6,0x08); //perform begin erase / program cycle
			DelayMs(Tera);
			PGD=0;
			set_vdd_vpp(pictype,0);
			break;
		default:
			return 3;
			break;
	}
	return 1; //1 means "OK" 		
}






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
char write_code(PICTYPE pictype, PICVARIANT picvariant, unsigned long address, char* data,char blocksize,char lastblock)
{
	char i;
	char blockcounter;
	switch(picvariant)
	{
		case P18F2XXX:
			if(lastblock&1)set_vdd_vpp(pictype,1);
			pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
			pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
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
			pic_send_n_bits(3, 0);
			lasttick=tick;
			PGC=1;	//hold PGC high for P9 and low for P10
			DelayMs(P9);
			PGC=0;
			DelayMs(P10);
			pic_send_word(0x0000);
			if(lastblock&2)set_vdd_vpp(pictype,0);
			break;
		case P16F62XA:
			if(lastblock&2)set_vdd_vpp(pictype,0);	
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				pic_send_14_bits(6,0x02,(((unsigned int)data[blockcounter])<<8)|   //MSB
						(((unsigned int)data[blockcounter+1])));//LSB
				pic_send_n_bits(6,0x08);
				DelayMs(Tprog);
				//read data from program memory (to verify) not yet impl...
				pic_send_n_bits(6,0x06);
			}
			if(lastblock&2)set_vdd_vpp(pictype,0);		
			break;
		default:
			return 3;
			break;
	}
	if(lastblock&2)
	{
		return 2;	//ask for next block
	}
	else
	{
		return 1;	//ok
	}
}


/**
before calling this function, datastate must be DATASTART
call as many times until datastate==DATASUCCESS
 */
char write_data(PICTYPE pictype, PICVARIANT picvariant, unsigned int address, char* data, char blocksize, char lastblock)
{
	char blockcounter;
	char receiveddata;
	switch(picvariant)
	{
		case P18F2XXX:
			if(lastblock&1)set_vdd_vpp(pictype,1);
			pic_send(4,0x00,0x9EA6); //BCF EECON1, EEPGD
			pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
			{
				
				pic_send(4,0x00,(0x0E00|( address+(unsigned int)blockcounter)    &0xFF)); //MOVLW Addr [7:0]
				pic_send(4,0x00,0x6EA9); //MOVWF EEADR
				pic_send(4,0x00,(0x0E00|((address+(unsigned int)blockcounter)>>8)&0xFF)); //MOVLW Addr [15:8]
				pic_send(4,0x00,0x6EAA); //MOVWF EEADRH
				pic_send(4,0x00,0x0E00|(unsigned int)data[blockcounter]); //MOVLW data
				pic_send(4,0x00,0x6eA8); //MOVWF EEDATA
				pic_send(4,0x00,0x84A6); //BSF EECON1, WREN
				pic_send(4,0x00,0x82A6); //BSF EECON1, WR
				//pic_send(4,0x00,0x0000); //NOP, when not polling for the WR bit, the PIC still needs at least 4 clocks
				lasttick=tick;
				do
				{
					pic_send(4,0x00,0x50A6); //movf EECON1, W, 0
					pic_send(4,0x00,0x6EF5); //movwf TABLAT
					pic_send(4,0x00,0x0000); //nop
					receiveddata=pic_read_byte2(4,0x02); //Shift TABLAT register out
				}while(((receiveddata&0x02)==0x02)&&((tick-lasttick)<P11A)); //poll for WR bit to clear
				
				//DelayMs(P11A);
				//lasttick=tick;
				//PGC=0;	//hold PGC low for P10 (100us)
				DelayMs(P10);
				pic_send(4,0x00,0x94A6); //BCF EECON1, WREN
			}
			if(lastblock&2)set_vdd_vpp(pictype,0);
			break;
		case P16F62XA:
			if(lastblock&1)set_vdd_vpp(pictype,1);
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
			{
				//load data
				pic_send_14_bits(6,0x03,((unsigned int)data[blockcounter]));//LSB only
				//begin programming command
				pic_send_n_bits(6,0x08);
				//wait Tprog
				DelayMs(Tdprog);
				//read data from data memory (to verify) not yet impl...
				//increment address
				pic_send_n_bits(6,0x06);
				
			}
			if(lastblock&2)set_vdd_vpp(pictype,0);
			break;	
		default:
			return 3;
			break;
	}
	if(lastblock&2)
	{
		return 2;	//ask for next block
	}
	else
	{
		return 1;	//ok
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
char write_config_bits(PICTYPE pictype, PICVARIANT picvariant, unsigned long address, char* data, char blocksize, char lastblock)
{
	char i;
	static char blockcounter;
	switch(picvariant)
	{
		case P18F2XXX:
			if(lastblock&1)set_vdd_vpp(pictype,1);
			pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
			pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
			address=0x300000;
			//start for loop
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				set_address(pictype, address+((unsigned int)blockcounter));
				//LSB first
				pic_send(4,0x0F,((unsigned int)*(data))&0x00FF);
				pic_send_n_bits(3, 0);
				PGC=1;	//hold PGC high for P9
				DelayMs(P9);
				PGC=0;	//hold PGC low for time P10
				DelayMs(P10);
				pic_send_word(0x0000); //last part of the nop
				set_address(pictype, address+((unsigned int)blockcounter)+1);
				pic_send(4,0x0F, (((unsigned int)*(data+1))<<8)&0xFF00); //load MSB and start programming
				pic_send_n_bits(3, 0);
				PGC=1;	//hold PGC high for P9
				DelayMs(P9);
				PGC=0;	//hold PGC low for time P10
				DelayMs(P10);
				pic_send_word(0x0000); //last part of the nop
			}
			if(lastblock&2)set_vdd_vpp(pictype,0);
			setLeds(4);


			break;
		default:
			return 3;
			break;
	}
	if(lastblock&2)
	{
		return 2;	//ask for next block
	}
	else
	{
		return 1;	//ok
	}
}

/**
This function has to be called only once per block
read_program will read program memory, id's and configuration bits
**/
void read_code(PICTYPE pictype, PICVARIANT picvariant, unsigned long address, char* data, char blocksize, char lastblock)
{
	char i;
	char blockcounter=0;
	if(lastblock&1)set_vdd_vpp(pictype,1);
	switch(picvariant)
	{
		case P18F2XXX:
                        set_address(pictype, address);
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
				*(data+blockcounter)=pic_read_byte2(4,0x09);
			break;
		default:
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
				*(data+blockcounter)=0;
			break;
			break;
	}
	if(lastblock&2)set_vdd_vpp(pictype,0);

}


/**
This function reads a block of data from the data eeprom of size blocksize into *data
call this function only once.
**/
void read_data(PICTYPE pictype, PICVARIANT picvariant, unsigned int address, char* data, char blocksize, char lastblock)
{
	
	char i;
	char blockcounter=0;
	//if(lastblock&1)
	set_vdd_vpp(pictype,1);
	switch(picvariant)
	{
		case P18F2XXX:
			pic_send(4,0x00,0x9EA6); //BCF EECON1, EEPGD
			pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
			{
				pic_send(4,0x00,(0x0E00|( address+(unsigned int)blockcounter)    &0xFF)); //MOVLW Addr [7:0]
				pic_send(4,0x00,0x6EA9); //MOVWF EEADR
				pic_send(4,0x00,(0x0E00|((address+(unsigned int)blockcounter)>>8)&0xFF)); //MOVLW Addr [15:8]
				pic_send(4,0x00,0x6EAA); //MOVWF EEADRH
				pic_send(4,0x00,0x80A6); //BSF EECON1, RD
				pic_send(4,0x00,0x50A8); //MOVF EEDATA, W, 0
				pic_send(4,0x00,0x6EF5); //MOVWF TABLAT
				pic_send(4,0x00,0x0000); //Nop
				*(data+blockcounter)=pic_read_byte2(4,0x02);
			}
			break;
		default:
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
				*(data+blockcounter)=0;
			break;
	}
	//if(lastblock&2)
	set_vdd_vpp(pictype,0);
}












