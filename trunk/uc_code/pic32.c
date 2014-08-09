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
#include "interrupt.h"
#include "prog_lolvl.h"
#include "device.h"
#include <delays.h>
#ifdef USE_PIC32
#pragma code
extern unsigned char ConfigLimitPGDPGC;

#define ETAP_ADDRESS    0x08  //Select Address register.
#define ETAP_DATA       0x09  //Select Data register.
#define ETAP_CONTROL    0x0A  //Select EJTAG Control register.
#define ETAP_EJTAGBOOT  0x0C  //Set EjtagBrk, ProbEn and ProbTrap to ‘1’ as the reset value.
#define ETAP_FASTDATA   0x0E  //Selects the Data and Fastdata registers.

#define MTAP_COMMAND    0x07 //TDI and TDO connected to MCHP Command Shift register (See Table 19-2)
#define MTAP_SW_MTAP    0x04 //Switch TAP controller to MCHP TAP controller
#define MTAP_SW_ETAP    0x05 //Switch TAP controller to EJTAG TAP controller
#define MTAP_IDCODE     0x01 //Select Chip Identification Data register


#define MCHP_STATUS        0x00 //NOP and return Status.
#define MCHP_ASSERT_RST    0xD1 //Requests the reset controller to assert device Reset.
#define MCHP_DE_ASSERT_RST 0xD0 //Removes the request for device Reset, which causes the reset controller to deassert device Reset if there is no other source requesting Reset (i.e., MCLR).
#define MCHP_ERASE         0xFC //Cause the Flash controller to perform a Chip Erase.
#define MCHP_FLASH_ENABLE  0xFE //Enables fetches and loads to the Flash (from the processor).
#define MCHP_FLASH_DISABLE 0xFD //Disables fetches and loads to the Flash (from the processor).


#define P32SetMode(x) jtag2w4p(0, x, 6)

void P32SendCommand(unsigned char cmd)
{
	jtag2w4p(0,3,4); //header 1100
	jtag2w4p(cmd,0x10,5);
	jtag2w4p(0, 1, 2); //footer 10
}

unsigned long P32XferData(unsigned long d)
{
	unsigned long res=0;
	jtag2w4p(0,1, 3); //header 100
	res =(unsigned long) jtag2w4p((unsigned char) d,0,8);
	res|=((unsigned long) jtag2w4p((unsigned char) d>>8,0,8)) << 8;
	res|=((unsigned long) jtag2w4p((unsigned char) d>>16,0,8)) <<16;
	res|=((unsigned long) jtag2w4p((unsigned char) d>>24,0x80,8)) <<24;
	jtag2w4p(0, 1, 2); //footer 10
	return res;
}

unsigned long P32XferFastData(unsigned long d)
{
}

void P32XferInstruction (unsigned long instruction)
{
	unsigned long controlVal;
	// Select Control Register
	P32SendCommand(ETAP_CONTROL);
	do {
		controlVal = P32XferData(0x0004C000);
	} while( (controlVal&0x20000)==0 );
	// Select Data Register
	P32SendCommand(ETAP_DATA);
	// Send the instruction
	P32XferData(instruction);
	// Tell CPU to execute instruction
	P32SendCommand(ETAP_CONTROL);
	P32XferData(0x0000C000);
}

void P32CheckDeviceStatus(void)
{
	unsigned char statusVal, i=0;
	P32SetMode (0x1F);// to force the Chip TAP
					  //controller into Run Test/Idle state.
	P32SendCommand (MTAP_SW_MTAP);
	P32SendCommand (MTAP_COMMAND);
	do
	{
		statusVal = (unsigned char)P32XferData (MCHP_STATUS);
	}while((statusVal&0xC)!=0x8&&i++<255);
		//If CFGRDY (statusVal<3>) is not ‘1’ and
		//FCBUSY (statusVal<2>) is not ‘0’, GOTO
		//step 4.
	
}

void bulk_erase_P32( unsigned char doRestore )
{
	unsigned char statusVal, i=0;
	P32SendCommand (MTAP_SW_MTAP);
	P32SendCommand (MTAP_COMMAND);
	P32XferData (MCHP_ERASE);
	//#warning P32XferData (MCHP_DE_ASSERT_RST);//, PIC32MZ devices only.
	do
	{
		DelayMs(10);
		statusVal = (unsigned char)P32XferData (MCHP_STATUS);
	}while((statusVal&0xC)!=0x8&&i++<255);
		//If CFGRDY (statusVal<3>) is not ‘1’ and
		//FCBUSY (statusVal<2>) is not ‘0’, GOTO
		//step 4.
}

void P32EnterSerialExecutionMode(void)
{
	P32SendCommand (MTAP_SW_MTAP);
	P32SendCommand (MTAP_COMMAND);
	//statusVal = XferData (MCHP_STATUS);
	//If CPS (statusVal<7>) is not ‘1’, the device must
	//be erased first.
	P32XferData (MCHP_ASSERT_RST);
	P32SendCommand (MTAP_SW_ETAP);
	P32SendCommand (ETAP_EJTAGBOOT);
	P32SendCommand (MTAP_SW_MTAP);
	P32SendCommand (MTAP_COMMAND);
	P32XferData (MCHP_DE_ASSERT_RST);
	P32XferData (MCHP_FLASH_ENABLE);//– This step is not required for PIC32MZ EC family devices.
	P32SendCommand (MTAP_SW_ETAP);
}

void write_code_P32(unsigned long address, unsigned char* data, char blocksize, char lastblock)
{
	char blockcounter;
	char i;
	unsigned int payload;
	if( (lastblock & BLOCKTYPE_FIRST) )
	{
		P32XferInstruction(0x3c10a000+address);
	}
	
	for( blockcounter = 0; blockcounter < blocksize; blockcounter += 4 )
	{
		payload = (((unsigned int) data[blockcounter])) | //MSB
			(((unsigned int) data[blockcounter + 1]) << 8);
		P32XferInstruction(0x3c080000|(unsigned long)payload);
		payload = (((unsigned int) data[blockcounter+2])) | //MSB
			(((unsigned int) data[blockcounter + 3]) << 8);
		P32XferInstruction(0x35080000|(unsigned long)payload);
		P32XferInstruction(0xae080000|((address+blockcounter)&0x7F));
		
	}
	if((address&0x60)==0x60)
	{
		//Step 1:
		//All PIC32 devices: Initialize constants. Registers
		//a1, a2, and a3 are set for WREN = 1 or
		//NVMOP<3:0> = 0011, WR = 1 and WREN = 1,
		//respectively. Registers s1 and s2 are set for the
		//unlock data values and S0 is initialized to ‘0’.
		P32XferInstruction(0x34054003); 
		P32XferInstruction(0x34068000); 
		P32XferInstruction(0x34074000); 
		P32XferInstruction(0x3c11aa99); 
		P32XferInstruction(0x36316655); 
		P32XferInstruction(0x3c125566); 
		P32XferInstruction(0x365299aa); 
		P32XferInstruction(0x3c100000); 
		//Step 2:
		//PIC32MX devices only: Set register a0 to the
		//base address of the NVM register (0xBF80_F400)
		P32XferInstruction(0x3c04bf80);
		P32XferInstruction(0x3484f400);
		//Step 2: PIC32MZ EC devices only: Set register a0 to the
		//base address of the NVM register (0xBF80_0600).
		//Register s3 is set for the value used to disable
		//write protection in NVMBPB.
#if 0		
		P32XferInstruction(0x3c04bf80); 
		P32XferInstruction(0x34840600); 
		P32XferInstruction(0x34138080); 
		//Step 3: 
		//PIC32MZ EC devices only: Unlock and disable
		//Boot Flash write protection.
		P32XferInstruction(0xac910010);
		P32XferInstruction(0xac920010);
		P32XferInstruction(0xac930090);
		P32XferInstruction(0x00000000);
#endif
		//Step 4:
		//All PIC32 devices: Set the NVMADDR register
		//with the address of the Flash row to be
		//programmed.
		P32XferInstruction(0x3c080000|(address>>16));
		P32XferInstruction(0x35080000|(address&0xFFFF));
		P32XferInstruction(0xac880020  );
		//Step 5:
		//PIC32MX devices only: Set the NVMSRCADDR
		//register with the physical source SRAM address
		//(offset is 64).
		P32XferInstruction(0x36100000|(address&0xFFFF));
		P32XferInstruction(0xac900040  );
#if 0		
		//Step 5
		//PIC32MZ EC devices only: Set the
		//NVMSRCADDR register with the physical source
		//SRAM address (offset is 112).
		P32XferInstruction(0x36100000|(address&0xFFFF));
		P32XferInstruction(0xac900070  );
#endif
		//Step 6:
		//All PIC32 devices: Set up the NVMCON register
		//for write operation
		P32XferInstruction(0xac850000);
		//Step 7:
		//PIC32MX devices only: Poll the LVDSTAT
		//register.
		P32XferInstruction(0x8C880000);
		P32XferInstruction(0x31080800);
		P32XferInstruction(0x1500fffd);
		P32XferInstruction(0x00000000);
		//Step 8:
		//All PIC32 devices: Unlock the NVMCON register
		//and start the write operation
		P32XferInstruction(0xac910010);
		P32XferInstruction(0xac920010);
		P32XferInstruction(0xac860008);
		//Step 9:
		//All PIC32 devices: Loop until the WR bit
		//(NVMCON<15>) is clear.
		P32XferInstruction(0x8c880000);
		P32XferInstruction(0x01064024);
		P32XferInstruction(0x1500fffd);
		P32XferInstruction(0x00000000);
		//Step 10:
		//All PIC32 devices: Wait at least 500 ns after
		//seeing a ‘0’ in the WR bit (NVMCON<15>) before
		//writing to any of the NVM registers. This requires
		//inserting a NOP in the execution. The following
		//example assumes that the core is executing at
		//8 MHz; therefore, four NOP instructions equate to
		//500 ns.
		P32XferInstruction(0x00000000);
		P32XferInstruction(0x00000000);
		P32XferInstruction(0x00000000);
		P32XferInstruction(0x00000000);
		//Step 11:
		//All PIC32 devices: Clear the WREN bit
		//(NVMCONM<14>).
		P32XferInstruction(0xac870004);
		//Step 12: All PIC32 devices: Check the WRERR bit
		//(NVMCON<13>) to ensure that the program
		//sequence has completed successfully. If an error
		//occurs, jump to the error processing routine.
		//P32XferInstruction(0x8c880000);
		//P32XferInstruction(0x30082000);
		//P32XferInstruction(0x1500<ERR_PROC>);
		//P32XferInstruction(0x00000000);
	}
}

void read_code_P32( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	char blockcounter;
	unsigned long payload;
	if(lastblock&BLOCKTYPE_FIRST)
	{
		//Step 1: Initialize some constants.
		P32XferInstruction(0x3c13ff20); //lui $s3, 0xFF20
	}
	for( blockcounter = 0; blockcounter < blocksize; blockcounter += 4 )
	{
		//Step 2: Read memory Location.
		P32XferInstruction(0x3c080000 | (address>>16));// lui $t0,<FLASH_WORD_ADDR(31:16)>
		P32XferInstruction(0x35080000 | (address&0xFF)); // ori $t0,<FLASH_WORD_ADDR(15:0)>
		//Step 3: Write to Fastdata location.
		P32XferInstruction(0x8d090000);// lw $t1, 0($t0)
		P32XferInstruction(0xae690000);// sw $t1, 0($s3)
		//Step 4: Read data from Fastdata register 0xFF200000.
		
		//Step 5: Repeat Steps 2-4 until all configuration locations
		//are read.
	}
}

void write_config_bits_P32( unsigned long address, unsigned char* data, char blocksize, char lastblock )
{
	write_code_P32(address, data, blocksize, lastblock);
}

void get_device_id_P32(unsigned char* data)
{
	unsigned long payload;
	P32SendCommand (MTAP_SW_MTAP);
	P32SendCommand (MTAP_IDCODE);
	payload = P32XferData(0);
	data[0] = (unsigned char)payload;
	data[1] = (unsigned char)(payload>>8);
	data[2] = (unsigned char)(payload>>16);
	data[3] = (unsigned char)(payload>>24);
}

#pragma romdata DEVICES
DEVICE_TABLE devices_pic32[] =
{
//    		Pictype,	picfamily,5V,	enter_ISCP,	bulk_erase,	read_code,	read_data,	write_code,	write_data,	write_config_bits )
DEVICE_ENTRY( P32MX110,PIC32,	  3V,   PIC32,		P32,		P32,		none,		P32,		none,		P32 )
};

#pragma romdata
#undef LIST


#endif
