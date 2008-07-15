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

#pragma romdata eedata = 0xf00000

const rom unsigned char eeprom_data1 = 1;
const rom unsigned char eeprom_data2 = 2;
const rom unsigned char eeprom_data3 = 3;
const rom unsigned char eeprom_data4 = 4;
const rom unsigned char eeprom_data5 = 5;
const rom unsigned char eeprom_data6 = 6;
const rom unsigned char eeprom_data7 = 7;
const rom unsigned char eeprom_data8 = 8;
const rom unsigned char eeprom_data9 = 9;
const rom unsigned char eeprom_data10 = 10;
const rom unsigned char eeprom_data11 = 11;
const rom unsigned char eeprom_data12 = 12;
const rom unsigned char eeprom_data13 = 13;
const rom unsigned char eeprom_data14 = 14;
const rom unsigned char eeprom_data15 = 15;
const rom unsigned char eeprom_data16 = 16;
const rom unsigned char eeprom_data17 = 17;

#pragma code
extern long lasttick;
extern long tick;

/**
before calling this function, erasestate has to be ERASESTART
This function has to be called as many times until erasestate==ERASESUCCESS
*/
unsigned int osccal,bandgap; //for P12F629 devices...

char bulk_erase(PICTYPE pictype,PICVARIANT picvariant)
{
	unsigned int i;
	set_vdd_vpp(pictype,1);
	switch(picvariant)
	{
		case P18F2XXX:           //also valid for 18F4XXX
			set_address(pictype, 0x3C0005);
			pic_send(4,0x0C,0x3F3F); //Write 3F3Fh to 3C0005h
			set_address(pictype, 0x3C0004);
			pic_send(4,0x0C,0x8F8F); //Write 8F8Fh to 3C0004h
			pic_send(4,0x00,0x0000); //NOP
			lasttick=tick;
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(P11);
			break;
		case P18FXX2:            //also valid for 18FXX8
			set_address(pictype, 0x3C0004);
			pic_send(4,0x0C,0x0080);
			pic_send(4,0x00,0x0000); //NOP
			lasttick=tick;
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(P11);
			break;
		case P16F87XA:
			pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
			pic_send_n_bits(6,0x1F); //send 11111x to erase device
			DelayMs(Tprog);
			break;
		case P12F6XX:
			return 3;
			break;
		case P12F629:
			for(i=0;i<0x3FF;i++)pic_send_n_bits(6,0x06);//set PC to 3FF
			osccal=pic_read_14_bits(6,0x04);
			pic_send_14_bits(6,0x00,0x0000);//set PC to 0x2000
                        for(i=0;i<7;i++)pic_send_n_bits(6,0x06);//2. Execute Increment Address command 7 times to advance PC to 0x2007.
			bandgap=pic_read_14_bits(6,0x04)&0x3000;
			//a) Read and store OSCCAL and BG bits.
			pic_send_14_bits(6,0x00,0x0000);//b) Execute Load Configuration (000000).
			pic_send_n_bits(6,0x09);//c) Execute Bulk Erase Program Memory (001001).
			DelayMs(Tera);//d) Wait TERA.
			pic_send_n_bits(6,0x0B);//e) Execute Bulk Erase Data Memory (001011).
			DelayMs(Tera);//f) Wait TERA.
			//h) Restore OSCCAL and BG bits.*/
			break;
		case P16F62XA:
			pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
			pic_send_14_bits(6,0x02,0x3FFF); //load data for program memory 0x3FFF << 1
			pic_send_n_bits(6,0x09); //perform bulk erase of the program memory
			DelayMs(Tera); //wait Tera for erase to complete
			/*PGD=0;
			set_vdd_vpp(pictype,0);
			set_vdd_vpp(pictype,1);    */
			pic_send_n_bits(6,0x0B); //perform bulk erase of the data memory
			DelayMs(Tera);
			PGD=0;
			break;
		case P16F62X:
			/*///Remove code protection
			pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
			for(i=0;i<7;i++)pic_send_n_bits(6,0x06);//2. Execute Increment Address command 7 times to advance PC to 0x2007.
			pic_send_n_bits(6,0x01);//3. Execute Bulk Erase Setup 1 command.
			pic_send_n_bits(6,0x07);//4. Execute Bulk Erase Setup 2 command.
			pic_send_n_bits(6,0x08);//5. Execute Begin Erase Programming command.
			DelayMs(Tera+Tprog); //6. Wait Tera + Tprog.
			pic_send_n_bits(6,0x01);//3. Execute Bulk Erase Setup 1 command.
			pic_send_n_bits(6,0x07);//4. Execute Bulk Erase Setup 2 command.
			set_vdd_vpp(pictype,0);
			///erase code memory
			set_vdd_vpp(pictype,1);
			pic_send_14_bits(6,0x02,0x3FFF);//1. Execute a Load Data for Program Memory with the data word set to all �1�s (0x3FFF).
			pic_send_n_bits(6,0x09);//2. Execute a Bulk Erase Program Memory command.
			pic_send_n_bits(6,0x08);//3. Execute a Begin Programming command.
			DelayMs(Tera);//4. Wait Tera for the erase cycle to complete.
			set_vdd_vpp(pictype,0);
			///erase data memory
			set_vdd_vpp(pictype,1);
			pic_send_14_bits(6,0x03,0x3FFF);//1. Execute a Load Data for Data Memory with the	data word set to all �1�s (0x3FFF).
			pic_send_n_bits(6,0x0B);//2. Execute a Bulk Erase Data Memory command.
			pic_send_n_bits(6,0x08);//3. Execute a Begin Programming command.
			DelayMs(Tera);//4. Wait Tera for the erase cycle to complete.
			PGD=0;*/
			/*for(i=0;i<64;i+=2) //fill buffer with 0x3FFF
			{
				temp_data[i]=0x3F;
				temp_data[i+1]=0xFF;
			}
			for(i=0;i<0x7FF;i+=64)
				write_code(pictype, picvariant, i, temp_data,64,0);
			PGD=0;
			set_vdd_vpp(pictype,0);
			write_config_bits(pictype, picvariant, 0x2000, temp_data, 16, 3);
			for(i=0;i<64;i++) //fill buffer with 0xFF
			{
				temp_data[i]=0xFF;
			}
			write_data(pictype, picvariant, 0, temp_data, 64, 1);
			write_data(pictype, picvariant, 0, temp_data, 64, 2);*/
			break;
		default:
			PGD=0;
			set_vdd_vpp(pictype,0);
			return 3;
			break;
	}
	set_vdd_vpp(pictype,0);
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
char write_code(PICTYPE pictype, PICVARIANT picvariant, unsigned long address, unsigned char* data,char blocksize,char lastblock)
{
	unsigned int i;
	char blockcounter;
	if(lastblock&1)set_vdd_vpp(pictype,1);
	switch(picvariant)
	{
		case P18F2XXX:
			pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
			pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
			set_address(pictype, address);
			for(blockcounter=0;blockcounter<(blocksize-2);blockcounter+=2)
			{
				//write 2 bytes and post increment by 2
				//				MSB				LSB
				pic_send(4,0x0D,((unsigned int)*(data+blockcounter))|
						(((unsigned int)*(data+1+blockcounter))<<8));
			}
			//write last 2 bytes of the block and start programming
			pic_send(4,0x0F,((unsigned int)*(data+blockcounter))|(((unsigned int)*(data+1+blockcounter))<<8)); 
			pic_send_n_bits(3, 0);
			lasttick=tick;
			PGC=1;	//hold PGC high for P9 and low for P10
			DelayMs(P9);
			PGC=0;
			DelayMs(P10);
			pic_send_word(0x0000);
			break;
		case P16F87XA:
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=16) //8 words of data = 16 bytes
			{
				for(i=0;i<16;i+=2)
				{
					pic_send_14_bits(6,0x02,(((unsigned int)data[blockcounter+i]))|   //MSB
						(((unsigned int)data[blockcounter+i+1])<<8));//LSB
                                        if(i<14)pic_send_n_bits(6,0x06);	//increment address
				}
				pic_send_n_bits(6,0x08);    //begin programming
				DelayMs(1);
				pic_send_n_bits(5,0x17);    //end programming
				pic_send_n_bits(6,0x06);	//increment address
			}
			break;
		case P12F629:
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				if((address+blockcounter)<0x3FF) //do not program
				{
					pic_send_14_bits(6,0x02,(((unsigned int)data[blockcounter]))|   //MSB
							(((unsigned int)data[blockcounter+1])<<8));//LSB
					pic_send_n_bits(6,0x18);    //begin programming, externally timed
					DelayMs(2);
					pic_send_n_bits(6,0x0A); 	//end programming
					//read data from program memory (to verify) not yet impl...
					pic_send_n_bits(6,0x06);	//increment address
				}
				else     //if the osccall address is called, restore the osccal value instead.
				{
					pic_send_14_bits(6,0x02,osccal);
					pic_send_n_bits(6,0x18);    //begin programming, externally timed
					DelayMs(2);
					pic_send_n_bits(6,0x0A); 	//end programming
				}
			}
			if((lastblock&2)&&((address+blocksize)<0x3FF)) //restore osccal register
			{
				for(i=0;i<(0x3FF-(address+blocksize));i++)
								pic_send_n_bits(6,0x06);	//increment address
				pic_send_14_bits(6,0x02,osccal);
				pic_send_n_bits(6,0x18);    //begin programming, externally timed
				DelayMs(2);
				pic_send_n_bits(6,0x0A); 	//end programming
			}
		case P16F62XA:      //same as P16F62X
		case P16F62X:
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				pic_send_14_bits(6,0x02,(((unsigned int)data[blockcounter]))|   //MSB
						(((unsigned int)data[blockcounter+1])<<8));//LSB
				pic_send_n_bits(6,0x08);    //begin programming
				DelayMs(Tprog);
				//read data from program memory (to verify) not yet impl...
				pic_send_n_bits(6,0x06);	//increment address
			}
			break;
		default:
			set_vdd_vpp(pictype,0);
			return 3;
			break;
	}
	if(lastblock&2)
	{
		set_vdd_vpp(pictype,0);
		return 1;	//ok
	}
	else
	{
		return 2;	//ask for next block
	}
}


/**
before calling this function, datastate must be DATASTART
call as many times until datastate==DATASUCCESS
 */
char write_data(PICTYPE pictype, PICVARIANT picvariant, unsigned int address, unsigned char* data, char blocksize, char lastblock)
{
	char blockcounter;
	char receiveddata;
	if(lastblock&1)set_vdd_vpp(pictype,1);
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
				//PGC=0;	//hold PGC low for P10 (100us)
				DelayMs(P10);
				pic_send(4,0x00,0x94A6); //BCF EECON1, WREN
			}
			break;
		case P12F629:		//same as P16F62X
		case P16F87XA:		//same as P16F62X?
		case P16F62XA:          //same as P16F62X
		case P16F62X:
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
			break;
		default:
			set_vdd_vpp(pictype,0);
			return 3;     //unknown pic type
			break;
	}
	if(lastblock&2)
	{
		set_vdd_vpp(pictype,0);
		return 1;	//ok
	}
	else
	{
		return 2;	//ask for next block
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
char write_config_bits(PICTYPE pictype, PICVARIANT picvariant, unsigned long address, unsigned char* data, char blocksize, char lastblock)
{
	char i;
	static char blockcounter;
	unsigned int payload;
	if(lastblock&1)set_vdd_vpp(pictype,1);
	switch(picvariant)
	{
		case P18F2XXX:
        		pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
			pic_send(4,0x00,0x8CA6); //BSF EECON1, CFGS
			//address=0x300000;
			//start for loop
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				set_address(pictype, address+((unsigned int)blockcounter));
				//LSB first
				pic_send(4,0x0F,((unsigned int)*(data+blockcounter))|(((unsigned int)*(data+blockcounter))<<8));
				pic_send_n_bits(3, 0);
				PGC=1;	//hold PGC high for P9
				DelayMs(P9);
				PGC=0;	//hold PGC low for time P10
				DelayMs(P10);
				pic_send_word(0x0000); //last part of the nop
				set_address(pictype, address+((unsigned int)blockcounter)+1);
				pic_send(4,0x0F, ((unsigned int)*(data+1+blockcounter))|(((unsigned int)*(data+1+blockcounter))<<8)); //load MSB and start programming
				pic_send_n_bits(3, 0);
				PGC=1;	//hold PGC high for P9
				DelayMs(P9);
				PGC=0;	//hold PGC low for time P10
				DelayMs(P10);
				pic_send_word(0x0000); //last part of the nop
			}
			break;
		case P16F87XA:
			if(lastblock&1)
			{
				pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
				for(i=0;i<((char)address);i++)pic_send_n_bits(6,0x06);   //increment address until ADDRESS is reached
			}
                        for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				//load data for config memory
				if(((((char)address)+blockcounter)<4)||(((char)address+blockcounter)==7))
				{
					pic_send_14_bits(6,0x00,(((unsigned int)data[blockcounter])<<8)|   //MSB
							(((unsigned int)data[blockcounter+1])));//LSB

				}
				if(((((char)address)+blockcounter)==4)||((((char)address)+blockcounter)==7))
				{
	                                pic_send_n_bits(6,0x08);    //begin programming
					DelayMs(Tprog);
				}
				//read data from program memory (to verify) not yet impl...
				pic_send_n_bits(6,0x06);	//increment address
			}
			break;
		case P12F629:
			if(lastblock&1)
			{
				pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
				for(i=0;i<((char)address);i++)pic_send_n_bits(6,0x06);   //increment address until ADDRESS is reached
			}
                        for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				//load data for config memory
				if(((((char)address)+blockcounter)<4))
				{
					pic_send_14_bits(6,0x00,(((unsigned int)data[blockcounter])<<8)|   //MSB
							(((unsigned int)data[blockcounter+1])));//LSB
					pic_send_n_bits(6,0x08);    //begin programming
					DelayMs(Tprog);
				}
				if(((((char)address)+blockcounter)==7))      //restore bandgap bits
				{
					payload=bandgap|(0x0FFF&((((unsigned int)data[blockcounter])<<8)|   //MSB
							(((unsigned int)data[blockcounter+1]))));
					pic_send_14_bits(6,0x00,payload);
					pic_send_n_bits(6,0x08);    //begin programming
					DelayMs(Tprog);
				}
				//read data from program memory (to verify) not yet impl...
				pic_send_n_bits(6,0x06);	//increment address
			}
			break;
		case P16F62XA: //same as P16F62X
		case P16F62X:
			if(lastblock&1)
			{
				pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
				for(i=0;i<((char)address);i++)pic_send_n_bits(6,0x06);   //increment address until ADDRESS is reached
			}
                        for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				//load data for config memory
				if(((((char)address)+blockcounter)<4)||((((char)address)+blockcounter)==7))
				{
					pic_send_14_bits(6,0x02,(((unsigned int)data[blockcounter])<<8)|   //MSB
							(((unsigned int)data[blockcounter+1])));//LSB
					pic_send_n_bits(6,0x08);    //begin programming
					DelayMs(Tprog);
				}
				//read data from program memory (to verify) not yet impl...
				pic_send_n_bits(6,0x06);	//increment address
			}
			break;
		default:
			set_vdd_vpp(pictype,0);
			return 3;
			break;
	}
	if(lastblock&2)
	{
		set_vdd_vpp(pictype,0);
		return 1;	//ok
	}
	else
	{
		return 2;	//ask for next block
	}
}

/**
This function has to be called only once per block
read_program will read program memory, id's and configuration bits
**/
void read_code(PICTYPE pictype, PICVARIANT picvariant, unsigned long address, unsigned char* data, char blocksize, char lastblock)
{
	unsigned int i;
	char blockcounter=0;
	unsigned int payload;
	if(lastblock&1)set_vdd_vpp(pictype,1);
	switch(picvariant)
	{
		case P18F2XXX:
                        set_address(pictype, address);
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
				*(data+blockcounter)=pic_read_byte2(4,0x09);
			break;
		case P12F629:	//same as P16F62X
		case P16F87XA:	//same as P16F62X
		case P16F62XA:  //same as P16F62X
		case P16F62X:
			if(address>=0x2000) //read configuration memory
			{
	                        pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
	                        if(lastblock&1)
				{
					for(i=0;i<(((unsigned int)address)-0x2000);i++)pic_send_n_bits(6,0x06);	//increment address
				}
				for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
				{
					payload=pic_read_14_bits(6,0x04); //read code memory
					data[blockcounter]=(char)(payload>>8);
					data[blockcounter+1]=(char)payload;
					pic_send_n_bits(6,0x06);	//increment address
				}
			}
			else
			{
				if(lastblock&1)
				{
					pic_read_14_bits(6,0x04); //read code memory
					for(i=0;i<(unsigned int)address;i++)pic_send_n_bits(6,0x06);	//increment address
				}
				for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
				{
					payload=pic_read_14_bits(6,0x04); //read code memory
					data[blockcounter]=(char)(payload>>8);
					data[blockcounter+1]=(char)payload;
					pic_send_n_bits(6,0x06);	//increment address
				}


			}
			break;
		default:
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)         //fill with zeros
				*(data+blockcounter)=0;
			break;
	}
	if(lastblock&2)set_vdd_vpp(pictype,0);

}


/**
This function reads a block of data from the data eeprom of size blocksize into *data
call this function only once.
**/
void read_data(PICTYPE pictype, PICVARIANT picvariant, unsigned int address, unsigned char* data, char blocksize, char lastblock)
{
	
	unsigned int i;
	char blockcounter=0;
	//if(lastblock&1)
	if(lastblock&1)set_vdd_vpp(pictype,1);
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
		case P12F629:	//same as P16F62X			
		case P16F87XA:	//same as P16F62X
		case P16F62XA:  //same as P16F62X
		case P16F62X:
			if(lastblock&1)
			{
				pic_read_14_bits(6,0x05);
				for(i=0;i<(unsigned int)address;i++)pic_send_n_bits(6,0x06);	//increment address
			}
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
			{
				data[blockcounter]=(char)pic_read_14_bits(6,0x05); //read data memory
				pic_send_n_bits(6,0x06);	//increment address
			}
			break;
		default:
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)   //fill with zeros
				*(data+blockcounter)=0;
			break;
	}
	//if(lastblock&2)
	if(lastblock&2)set_vdd_vpp(pictype,0);
}












