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
#ifdef SDCC
#include <pic18f2550.h>
#else
#include <p18cxxx.h>
#endif
#include "typedefs.h"
#include "io_cfg.h"             // I/O pin mapping
#include "prog.h"
#include "interrupt.h"
#include "prog_lolvl.h"


#define NORESTORE 0xAA
/**
Bulk erases the whole device
*/
char bulk_erase(PICFAMILY picfamily,PICTYPE pictype,unsigned char doRestore)
{
	unsigned int osccal,bandgap; //for P12F629 devices...
	unsigned int i,j;
	unsigned char temp[2];
	set_vdd_vpp(pictype,picfamily,1);
	switch(pictype)
	{
		case I2C_EE_1:
		case I2C_EE_2:
			break;
		case P24FXXKAXXX:
			//bulk erase program memory
			//step 1
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x040200);	//GOTO 0x200
			dspic_send_24_bits(0x000000);	//NOP
			//step 2
			dspic_send_24_bits(0x24064A);	//MOV #0x4064, W10
			dspic_send_24_bits(0x883B0A);	//MOV W10, NVMCON
			//step 3
			dspic_send_24_bits(0x200000);	//MOV #<PAGEVAL>, W0
			dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
			dspic_send_24_bits(0x200000);	//MOV #000, W0
			dspic_send_24_bits(0xBB0800);	//TBLWTL W0, [W0]
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			
			//step 4
			dspic_send_24_bits(0xA8E761);	//BSET NVMCON, #WR
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			for(i=0;i<200;i++)
			{				
				//step 5
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x040200);	//GOTO 0x200
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x803B02);	//MOV NVMCON, W2
				dspic_send_24_bits(0x883C22);	//MOV W2, VISI				
				dspic_send_24_bits(0x000000);	//NOP
				j=dspic_read_16_bits(0);
				dspic_send_24_bits(0x000000);	//NOP
				if((j&&0x8000)==0)break;	//programming completed
				DelayMs(10);

			}//step 8: repeat step 5-7
			break;
		case dsP30F:
			//bulk erase program memory
			//step 1
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x040100);	//GOTO 0x100
			dspic_send_24_bits(0x000000);	//NOP
			//step 2
			dspic_send_24_bits(0x24008A);	//MOV #0x4008, W10
			dspic_send_24_bits(0x883B0A);	//MOV W10, NVMCON
			//step 3
			dspic_send_24_bits(0x200F80);	//MOV #0xF8, W0
			dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
			dspic_send_24_bits(0x200067);	//MOV #0x6, W7
			//step 4
			dspic_send_24_bits(0xEB0300);	//CLR W6
			dspic_send_24_bits(0x000000);	//NOP
			for(i=0;i<2;i++)
			{				
				//step 5
				dspic_send_24_bits(0xBB1B86);	//TBLWTL W6, [W7++]
				//step 6
				dspic_send_24_bits(0x200558);	//MOV #0x55, W8
				dspic_send_24_bits(0x200AA9);	//MOV #0xAA, W9
				dspic_send_24_bits(0x883B38);	//MOV W8, NVMKEY
				dspic_send_24_bits(0x883B39);	//MOV W9, NVMKEY
				//step 7
				dspic_send_24_bits(0xA8E761);	//BSET NVMCON, #WR
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				DelayMs(201);			//Externally time 200ms
				dspic_send_24_bits(0xA9E761);	//BCLR NVMCON, #WR
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
			}//step 8: repeat step 5-7
			//step 9
			dspic_send_24_bits(0x2407FA);	//MOV #0x407F, W10
			dspic_send_24_bits(0x883B0A);	//MOV W10, NVMCON
			//step 10
			dspic_send_24_bits(0x200558);	//MOV #0x55, W8
			dspic_send_24_bits(0x883B38);	//MOV W8, NVMKEY
			dspic_send_24_bits(0x200AA9);	//MOV #0xAA, W9
			dspic_send_24_bits(0x883B39);	//MOV W9, NVMKEY
			//step 11
			dspic_send_24_bits(0xA8E761);	//BSET NVMCON, #WR
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			DelayMs(3);			//Externally time 2 msec
			dspic_send_24_bits(0xA9E761);	//BCLR NVMCON, #WR
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP

					
			break;
		case P18FXX39:
			set_address(picfamily, 0x3C0004);
			pic_send(4,0x0C,0x0083); //shouldn't it be 0x0C??? prog spec says 0x00
			pic_send(4,0x00,0x0000); //NOP
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(P11);
			pic_send(4,0x0C,0x0088); //shouldn't it be 0x0C??? prog spec says 0x00
			pic_send(4,0x00,0x0000); //NOP
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(P11);
			pic_send(4,0x0C,0x0089); //shouldn't it be 0x0C??? prog spec says 0x00
			pic_send(4,0x00,0x0000); //NOP
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(P11);
			pic_send(4,0x0C,0x008A); //shouldn't it be 0x0C??? prog spec says 0x00
			pic_send(4,0x00,0x0000); //NOP
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(P11);
			set_address(picfamily, 0x3C0006);
			pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
			pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
			set_address(picfamily, 0x200000);
			pic_send(4,0x00,0x84A6); //BSF EECON1, WREN
			pic_send(4,0x00,0x88A6); //BSF EECON1, FREE
			pic_send(4,0x00,0x0E55); // MOVLW 55h
			pic_send(4,0x00,0x6EA7); // MOVWF EECON2
			pic_send(4,0x00,0x0EAA); // MOVLW AAh
			pic_send(4,0x00,0x6EA7); // MOVWF EECON2
			pic_send(4,0x00,0x82A6); // BSF EECON1, WR
					
			pic_send(4,0x00,0x0000); //NOP
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(P11);
			pic_send(4,0x00,0x94A6); //BCF EECON1, WREN
			break;
		case P18F872X:
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0xFFFF); //Write 0101h to 3C0005h
			set_address(picfamily, 0x3C0004);
			pic_send(4,0x0C,0x8787); //Write 8080h to 3C0004h
			pic_send(4,0x00,0x0000); //NOP
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(400);
			break;
		case P18F2XXX:           //also valid for 18F4XXX
		case P18F4XK22:
		case P18LF4XK22:
			set_address(picfamily, 0x3C0005);
			if(pictype == P18F2XXX)
			{
				pic_send(4,0x0C,0x3F3F); //Write 3F3Fh to 3C0005h
			} else { // // P18LF/F4XK22
				pic_send(4,0x0C,0x0F0F); //Write 0F0Fh to 3C0005h
			}
			set_address(picfamily, 0x3C0004);
			pic_send(4,0x0C,0x8F8F); //Write 8F8Fh to 3C0004h
			pic_send(4,0x00,0x0000); //NOP
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			if(pictype==P18F2XXX)
			{
				DelayMs(P11);
			} 
			else 
			{ // P18LF/F4XK22
				DelayMs(P11K);
			}
			break;
		case P18F6XKXX:
			set_address(picfamily, 0x3C0004); //Erase configuration words
			pic_send(4,0x0C,0x0202);
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0x0000);
			set_address(picfamily, 0x3C0006);
			pic_send(4,0x0C,0x8080);
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(6);
			
			set_address(picfamily, 0x3C0004); //Erase Data EEPROM
			pic_send(4,0x0C,0x0404);
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0x0000);
			set_address(picfamily, 0x3C0006);
			pic_send(4,0x0C,0x8080);
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(6);
			
			set_address(picfamily, 0x3C0004); //Erase Boot Block
			pic_send(4,0x0C,0x0505);
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0x0000);
			set_address(picfamily, 0x3C0006);
			pic_send(4,0x0C,0x8080);
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(6);
			
			
			set_address(picfamily, 0x3C0004); //Erase Block 0
			pic_send(4,0x0C,0x0404);
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0x0101);
			set_address(picfamily, 0x3C0006);
			pic_send(4,0x0C,0x8080);
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(6);
			
			
			set_address(picfamily, 0x3C0004); //Erase Block 1
			pic_send(4,0x0C,0x0404);
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0x0202);
			set_address(picfamily, 0x3C0006);
			pic_send(4,0x0C,0x8080);
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(6);
			
			
			set_address(picfamily, 0x3C0004); //Erase Block 2
			pic_send(4,0x0C,0x0404);
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0x0404);
			set_address(picfamily, 0x3C0006);
			pic_send(4,0x0C,0x8080);
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(6);
			
			
			set_address(picfamily, 0x3C0004); //Erase Block 3
			pic_send(4,0x0C,0x0404);
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0x0808);
			set_address(picfamily, 0x3C0006);
			pic_send(4,0x0C,0x8080);
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(6);
			
			
			set_address(picfamily, 0x3C0004); //Erase Block 4
			pic_send(4,0x0C,0x0404);
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0x1010);
			set_address(picfamily, 0x3C0006);
			pic_send(4,0x0C,0x8080);
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(6);
			
			
			set_address(picfamily, 0x3C0004); //Erase Block 5
			pic_send(4,0x0C,0x0404);
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0x2020);
			set_address(picfamily, 0x3C0006);
			pic_send(4,0x0C,0x8080);
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(6);
			
			
			set_address(picfamily, 0x3C0004); //Erase Block 6
			pic_send(4,0x0C,0x0404);
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0x4040);
			set_address(picfamily, 0x3C0006);
			pic_send(4,0x0C,0x8080);
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(6);
			
			
			set_address(picfamily, 0x3C0004); //Erase Block 7
			pic_send(4,0x0C,0x0404);
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0x8080);
			set_address(picfamily, 0x3C0006);
			pic_send(4,0x0C,0x8080);
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(6);
			break;
		case P18FX220:
		case P18FXX31:
		case P18F6X2X:
		case P18FXX2:            //also valid for 18FXX8
			set_address(picfamily, 0x3C0004);
			pic_send(4,0x0C,0x0080);
			pic_send(4,0x00,0x0000); //NOP
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(P11);
			break;
		case P18F97J60:
		case P18F45J10:
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0x0101); //Write 0101h to 3C0005h
			set_address(picfamily, 0x3C0004);
			pic_send(4,0x0C,0x8080); //Write 8080h to 3C0004h
			pic_send(4,0x00,0x0000); //NOP
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(500);
			break;
		case P18LF13K22:
		case P18LF14K22:
			PGD_LOW =0;
			TRISPGD_LOW =0;
			PGC_LOW =0;
			TRISPGC_LOW=0;
			//no break, just low voltage and continue with P18F1XK22
		case P18F13K22:
		case P18F14K22:
			set_address(picfamily, 0x3C0005);
			pic_send(4,0x0C,0x0F0F); //Write 0F0Fh to 3C0005h
			set_address(picfamily, 0x3C0004);
			pic_send(4,0x0C,0x8F8F); //Write 8080h to 3C0004h
			pic_send(4,0x00,0x0000); //NOP
			pic_send(4,0x00,0x0000); //hold PGD low until erase completes
			DelayMs(6);
			break;
		case P16F716:
			pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
			//no break... continue with 17F72.
		case P16F72:
		case P16F7X:
		case P16F7X7:
			pic_send_n_bits(6,0x09); //send 1001xx to erase device
			DelayMs(30);
			break;
		case P16F87X:
			read_code(picfamily, pictype, 0x2007, temp, 2, 3); //read the config word to see wether it is code protected or not
			set_vdd_vpp(pictype, picfamily,0);
			DelayMs(10);
			set_vdd_vpp(pictype, picfamily,1);
			DelayMs(10);
			if(((temp[1]&0x31)<0x31)||((temp[0]&0x30)<0x30))
			{
				pic_send_14_bits(6,0x00,0x3FFF);//Execute a Load Configuration command (00) with (0x3FFF)
				for(i=0;i<7;i++)
					pic_send_n_bits(6,0x06);//2. Execute Increment Address command (000110) to set address to configuration word location (0x2007)
				pic_send_n_bits(6,0x01);	//3. Execute a Bulk Erase Setup1 command (000001)
				pic_send_n_bits(6,0x07);	//4. Execute a Bulk Erase Setup2 command (000111)
				pic_send_n_bits(6,0x08);	//5. Execute a Begin Erase/Programming command (001000)
				DelayMs(8);			//6. Wait 8 ms
				pic_send_n_bits(6,0x01);	//7. Execute a Bulk Erase Setup1 command (000001)
				pic_send_n_bits(6,0x07);	//8. Execute a Bulk Erase Setup2 command (000111)
			}
			else //device is not code protected
			{
				//Execute a Load Data for Program Memory command
				pic_send_14_bits(6,0x02,0x3FFF);//(000010) with a ’1’ in all locations (0x3FFF)
				pic_send_n_bits(6,0x01);	//2. Execute a Bulk Erase Setup1 command (000001)
				pic_send_n_bits(6,0x07);	//3. Execute a Bulk Erase Setup2 command (000111)
				pic_send_n_bits(6,0x08);	//4. Execute a Begin Erase/Programming command (001000)
				DelayMs(8);			//5. Wait 8 ms
				pic_send_n_bits(6,0x01);	//6. Execute a Bulk Erase Setup1 command (000001)
				pic_send_n_bits(6,0x07);	//7. Execute a Bulk Erase Setup2 command (000111)
				//Procedure to bulk erase data memory:
				pic_send_14_bits(6,0x03,0x3FFF);//1. Execute a Load Data for Data Memory command (000011) with (0x3FFF)
				
				pic_send_n_bits(6,0x01);	//2. Execute a Bulk Erase Setup1 command (000001)
				pic_send_n_bits(6,0x07);	//3. Execute a Bulk Erase Setup2 command (000111)
				pic_send_n_bits(6,0x08);	//4. Execute a Begin Erase/Programming command (001000)
				DelayMs(8);			//5. Wait 8 ms
				pic_send_n_bits(6,0x01);	//6. Execute a Bulk Erase Setup1 command (000001)
				pic_send_n_bits(6,0x07);	//7. Execute a Bulk Erase Setup2 command (000111)
			}
			break;
		case P16F62X:
			pic_send_14_bits(6,0x00,0x3FFF);//Execute a Load Configuration command (dataword 0x3FFF) to set PC to 0x2000.
			for(i=0;i<7;i++)pic_send_n_bits(6,0x06);//set PC to 0x2007
			/*pic_send_14_bits(6,0x02,0x3FFF);*/
			pic_send_n_bits(6,0x01);//,0x0000);//Execute Bulk Erase Setup 1 command.
			pic_send_n_bits(6,0x07);//,0x0000);//Execute Bulk Erase Setup 2 command.
			pic_send_n_bits(6,0x08);//, 0x0000);//Execute Begin Erase Programming command.
			DelayMs(20);	//Wait Tera + Tprog.
			pic_send_n_bits(6,0x01);//,0x0000);//Execute Bulk Erase Setup 1 command.
			pic_send_n_bits(6,0x07);//,0x0000);//Execute Bulk Erase Setup 2 command.
			DelayMs(20);
			set_vdd_vpp(pictype, picfamily,0);//reset
			DelayMs(10);
			set_vdd_vpp(pictype, picfamily,1);
			DelayMs(10);

			pic_send_14_bits(6,0x00,0x3FFF);//Execute a Load Configuration command (dataword 0x0000) to set
			/*pic_send_14_bits(6,0x02,0x3FFF);*/
			pic_send_n_bits(6,0x09); //bulk erase program memory, userid and config memory
			pic_send_n_bits(6,0x08);//Execute Begin Erase Programming command.
			DelayMs(20);	//Wait Tera + Tprog.
			set_vdd_vpp(pictype, picfamily,0);//reset
			DelayMs(10);
			set_vdd_vpp(pictype, picfamily,1);
			DelayMs(10);
			pic_send_14_bits(6,0x02,0x3FFF);
			pic_send_n_bits(6,0x0B); //bulk erase data memory
			pic_send_n_bits(6,0x08);//Execute Begin Erase Programming command.
			DelayMs(6);		//wait terase
			break;
		case P16F182X:
		case P16F785:
		case P16F88X:
		case P16F91X:
			pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
			switch(pictype)
			{
				case P16F91X:
					for(i=0;i<8;i++)pic_send_n_bits(6,0x06);//set PC to 0x3008
					break;
			}
			pic_send_n_bits(6,0x09); //bulk erase program memory, userid and config memory
			DelayMs(6);		//wait terase
			set_vdd_vpp(pictype, picfamily,0);//reset
			DelayMs(10);
			set_vdd_vpp(pictype, picfamily,1);
			DelayMs(10);
			pic_send_n_bits(6,0x0B); //bulk erase data memory
			DelayMs(6);		//wait terase
			break;
		case P16F87:
		case P16F81X:
		case P16F87XA:
			pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
			pic_send_n_bits(6,0x1F); //send 11111x to erase device
			DelayMs(Tprog);
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
			set_vdd_vpp(pictype, picfamily,0);
			DelayMs(10);
			if(doRestore==NORESTORE)break;	//do not restore bandgap and osccal registers
			temp[0]=(unsigned char)(osccal&0xFF);
			temp[1]=(unsigned char)((osccal>>8)&0xFF);
			write_code(picfamily, pictype, 0x3FF, temp,2 ,3);
			temp[0]=0xFF;
			temp[1]=(unsigned char)((bandgap>>8)|0xF);
			write_config_bits(picfamily, pictype, 0x2007, temp,2 ,3);
			break;
		case P12F61X:
		case P16F84A:	//same as P16F62XA
		case P12F6XX:	//same as P16F62XA
		case P16F62XA:
			if(pictype==P16F84A)
			{
				pic_send_14_bits(6,0x02,0x3FFF);
				pic_send_n_bits(6,0x09); //perform bulk erase of the program memory
				pic_send_n_bits(6,0x08); //begin programming cycle
				DelayMs(20); //wait Tera for erase to complete
			}
			pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
			pic_send_14_bits(6,0x02,0x3FFF); //load data for program memory 0x3FFF << 1
			if(pictype==P16F84A)pic_send_n_bits(6,0x08); //begin programming cycle
			pic_send_n_bits(6,0x09); //perform bulk erase of the user memory
			DelayMs(20); //wait Tera for erase to complete
			/*PGD=0;
			set_vdd_vpp(pictype, picfamily,0);
			set_vdd_vpp(pictype, picfamily,1);    */
			if(pictype==P12F61X)break;	//does not have data memory
			pic_send_n_bits(6,0x0B); //perform bulk erase of the data memory
			if(pictype==P16F84A)pic_send_n_bits(6,0x08); //begin programming cycle
			DelayMs(20);
			PGD=0;
			break;
		case P16F59:
		case P16F57:
		case P16F54:
			pic_send_n_bits(6,0x06); //increment address
			pic_send_n_bits(6,0x09); //begin programming
			DelayMs(20);
			break;
		case P10F200:	//resets at 0x1FF
			if(doRestore==NORESTORE)set_address(picfamily,0x100); //erase backup osccal as well
			pic_send_n_bits(6,0x06);//increment address
			pic_send_n_bits(6,0x09);//c) Execute Bulk Erase Program Memory (001001).
			DelayMs(10);
			if(doRestore==NORESTORE)break;	//do not restore bandgap and osccal registers
			DelayMs(10);
			set_address(picfamily, 0x104);
			osccal=pic_read_14_bits(6,0x04);
			set_vdd_vpp(pictype, picfamily,0);
			temp[0]=(unsigned char)(osccal&0xFF);
			temp[1]=(unsigned char)((osccal>>8)&0xFF);
			write_code(picfamily, pictype, 0xFF, temp,2 ,3);
			DelayMs(10);
			break;
		case P12F508:			
		case P10F202:	//resets at 0x3FF
			if(doRestore==NORESTORE)set_address(picfamily,0x200); //erase backup osccal as well
			pic_send_n_bits(6,0x06);//increment address
			pic_send_n_bits(6,0x09);//c) Execute Bulk Erase Program Memory (001001).
			DelayMs(10);
			if(doRestore==NORESTORE)break;	//do not restore bandgap and osccal registers
			set_address(picfamily, 0x204);
			osccal=pic_read_14_bits(6,0x04);
			set_vdd_vpp(pictype, picfamily,0);
			DelayMs(10);
			temp[0]=(unsigned char)(osccal&0xFF);
			temp[1]=(unsigned char)((osccal>>8)&0xFF);
			write_code(picfamily, pictype, 0x1FF, temp,2 ,3);
			DelayMs(10);
			break;
		default:
			PGD=0;
			set_vdd_vpp(pictype, picfamily,0);
			return 3;
			break;
	}
	set_vdd_vpp(pictype, picfamily,0);
	return 1; //1 means "OK"
}






/**
address points to the first byte of the block
data contains the data MSB0, LSB0, MSB1, LSB1, etc...
blocksize is the block syze in BYTES
 */
char write_code(PICFAMILY picfamily, PICTYPE pictype, unsigned long address, unsigned char* data,char blocksize,char lastblock)
{
	unsigned int i,payload;
	char blockcounter;
	if(lastblock&1)set_vdd_vpp(pictype, picfamily,1);
	switch(pictype)
	{
		case I2C_EE_1:
		case I2C_EE_2:
			I2C_start();
			I2C_write(0xA0|((unsigned char)((address&&0x10000)>>13))); //Device Address + 0=write
			
			if(pictype==I2C_EE_2)
				I2C_write((unsigned char)((address&0xFF00)>>8)); //MSB
			I2C_write((unsigned char)((address&0x00FF))); //LSB
			
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
			{
				i=(unsigned int)I2C_write(data[blockcounter]);
				if(i==1)
				{
					I2C_stop();
					return 2;
				}
			}
			I2C_stop();
			DelayMs(10);
			break;
		case P24FXXKAXXX:			
		case dsP30F:
			if(pictype==P24FXXKAXXX)
			{
				dspic_send_24_bits(0x000000);	//NOP
				//Step 1: Exit the Reset vector.
				dspic_send_24_bits(0x040200);	//GOTO 0x200
				dspic_send_24_bits(0x000000);	//NOP
				//Step 2: Set the NVMCON to program 32 instruction words.
				dspic_send_24_bits(0x24004A);	//MOV #0x4004, W10
				dspic_send_24_bits(0x883B0A);	//MOV W10, NVMCON

			}
			else
			{
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				//Step 1: Exit the Reset vector.
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
				//Step 2: Set the NVMCON to program 32 instruction words.
				dspic_send_24_bits(0x24001A);	//MOV #0x4001, W10
				dspic_send_24_bits(0x883B0A);	//MOV W10, NVMCON
			}
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=12)
			{
				//Step 3: Initialize the write pointer (W7) for TBLWT instruction.
				dspic_send_24_bits(0x200000|(((((blockcounter+address)*2)/3)&0xFF0000)>>12));	//MOV #<DestinationAddress23:16>, W0
				dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
				dspic_send_24_bits(0x200007|(((((blockcounter+address)*2)/3)&0x00FFFF)<<4));	//MOV #<DestinationAddress15:0>, W7
				//Step 4: Initialize the read pointer (W6) and load W0:W5 with the next 4 instruction words to program.
				for(i=0;i<6;i++)
				{
					dspic_send_24_bits(0x200000|
						(((unsigned long)data[blockcounter+(i*2)])<<4)|
						(((unsigned long)data[blockcounter+(i*2)+1])<<12)|
						((unsigned long) i));
					/**
					MOV #<LSW0>, W0
					MOV #<MSB1:MSB0>, W1
					MOV #<LSW1>, W2
					MOV #<LSW2>, W3
					MOV #<MSB3:MSB2>, W4
					MOV #<LSW3>, W5
					*/
				}
				//Step 5: Set the read pointer (W6) and load the (next set of) write latches.
				dspic_send_24_bits(0xEB0300);	//CLR W6
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBB0BB6);	//TBLWTL [W6++], [W7]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBBDBB6);	//TBLWTH.B [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBBEBB6);	//TBLWTH.B [W6++], [++W7]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBB1BB6);	//TBLWTL [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBB0BB6);	//TBLWTL [W6++], [W7]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBBDBB6);	//TBLWTH.B [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBBEBB6);	//TBLWTH.B [W6++], [++W7]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBB1BB6);	//TBLWTL [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
			}//Step 6: Repeat steps 3-5 eight times to load the write latches for 32 instructions.	
			//if((address%96)==64)
			//{
			if(pictype==P24FXXKAXXX)
			{
				//Step 7: Unlock the NVMCON for writing.
				dspic_send_24_bits(0xA8E761);	//BSET NVMCON, #WR
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				//Step 8: CHECK bit 15 of NVMCON

				
				for(i=0;i<20;i++)
				{				
					//step 5
					dspic_send_24_bits(0x000000);	//NOP
					dspic_send_24_bits(0x040200);	//GOTO 0x200
					dspic_send_24_bits(0x000000);	//NOP
					dspic_send_24_bits(0x803B02);	//MOV NVMCON, W2
					dspic_send_24_bits(0x883C22);	//MOV W2, VISI				
					dspic_send_24_bits(0x000000);	//NOP
					payload=dspic_read_16_bits(0);
					dspic_send_24_bits(0x000000);	//NOP
					if((payload&&0x8000)==0)break;	//programming completed
					DelayMs(1);

				}//step 8: repeat step 5-7
				//Step 9: Reset device internal PC.
				dspic_send_24_bits(0x040200);	//GOTO 0x200
				dspic_send_24_bits(0x000000);	//NOP
			}
			else
			{
				//Step 7: Unlock the NVMCON for writing.
				dspic_send_24_bits(0x200558);	//MOV #0x55, W8
				dspic_send_24_bits(0x883B38);	//MOV W8, NVMKEY
				dspic_send_24_bits(0x200AA9);	//MOV #0xAA, W9
				dspic_send_24_bits(0x883B39);	//MOV W9, NVMKEY
				//Step 8: Initiate the write cycle.
				dspic_send_24_bits(0xA8E761);	//BSET NVMCON, #WR
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				DelayMs(3);						//Externally time 2 msec
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xA9E761);	//BCLR NVMCON, #WR
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				//Step 9: Reset device internal PC.
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
			}
			//}
			break;
		case P18F872X:
			if((address&0x20)==0) //package must be 64 bytes, so only do this every two packages.
			{
				pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
				pic_send(4,0x00,0x8CA6); //BSF EECON1, CFGS			
				pic_send(4,0x00,0x88A6); //BSF EECON1, WREN			
				pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD			
				pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
				set_address(picfamily, address);
			}
			for(blockcounter=0;blockcounter<(blocksize-2);blockcounter+=2)
			{
				//write 2 bytes and post increment by 2
				//				MSB				LSB
				pic_send(4,0x0D,((unsigned int)*(data+blockcounter))|
						(((unsigned int)*(data+1+blockcounter))<<8));
			}
			if((address&&0x20)||(lastblock&2))
			{
				//write last 2 bytes of the block and start programming
				pic_send(4,0x0F,((unsigned int)*(data+blockcounter))|(((unsigned int)*(data+1+blockcounter))<<8)); 
				pic_send_n_bits(3, 0);
				PGC=1;	//hold PGC high for P9 and low for P10
				DelayMs(P9);
				PGC=0;
				DelayMs(P10);
				pic_send_word(0x0000);
			}
			else
				pic_send(4,0x0D,((unsigned int)*(data+blockcounter))|(((unsigned int)*(data+1+blockcounter))<<8)); 
			break;
		case P18F6XKXX:
			if((address&0x20)==0) //package must be 64 bytes, so only do this every two packages.
			{
				pic_send(4,0x00,0x8E7F); //BSF EECON1, EEPGD
				pic_send(4,0x00,0x9C7F); //BSF EECON1, CFGS			
				pic_send(4,0x00,0x847F); //BSF EECON1, WREN			
				set_address(picfamily, address);
			}
			for(blockcounter=0;blockcounter<(blocksize-2);blockcounter+=2)
			{
				//write 2 bytes and post increment by 2
				//				MSB				LSB
				pic_send(4,0x0D,((unsigned int)*(data+blockcounter))|
						(((unsigned int)*(data+1+blockcounter))<<8));
			}
			if((address&&0x20)||(lastblock&2))
			{
				//write last 2 bytes of the block and start programming
				pic_send(4,0x0F,((unsigned int)*(data+blockcounter))|(((unsigned int)*(data+1+blockcounter))<<8)); 
				pic_send_n_bits(3, 0);
				PGC=1;	//hold PGC high for P9 and low for P10
				DelayMs(P9);
				PGC=0;
				DelayMs(P10);
				pic_send_word(0x0000);
			}
			else
				pic_send(4,0x0D,((unsigned int)*(data+blockcounter))|(((unsigned int)*(data+1+blockcounter))<<8)); 
			break;		
		case P18F67KXX:
			if((address&0x40)==0) //package must be 64 bytes, so only do this every two packages.
			{
				pic_send(4,0x00,0x8E7F); //BSF EECON1, EEPGD
				pic_send(4,0x00,0x9C7F); //BSF EECON1, CFGS			
				pic_send(4,0x00,0x847F); //BSF EECON1, WREN			
				set_address(picfamily, address);
			}
			for(blockcounter=0;blockcounter<(blocksize-2);blockcounter+=2)
			{
				//write 2 bytes and post increment by 2
				//				MSB				LSB
				pic_send(4,0x0D,((unsigned int)*(data+blockcounter))|
						(((unsigned int)*(data+1+blockcounter))<<8));
			}
			if((address&&0x40)||(lastblock&2))
			{
				//write last 2 bytes of the block and start programming
				pic_send(4,0x0F,((unsigned int)*(data+blockcounter))|(((unsigned int)*(data+1+blockcounter))<<8)); 
				pic_send_n_bits(3, 0);
				PGC=1;	//hold PGC high for P9 and low for P10
				DelayMs(P9);
				PGC=0;
				DelayMs(P10);
				pic_send_word(0x0000);
			}
			else
				pic_send(4,0x0D,((unsigned int)*(data+blockcounter))|(((unsigned int)*(data+1+blockcounter))<<8)); 
			break;				
		case P18F2XXX:
		case P18F4XK22:
		case P18LF4XK22:		
			pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
			pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
			if((pictype == P18F4XK22)||(pictype == P18LF4XK22))
				pic_send(4,0x00,0x84A6); //BSF EECON1, WREN			
			set_address(picfamily, address);
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
			PGC=1;	//hold PGC high for P9 and low for P10
			DelayMs(P9);
			PGC=0;
			DelayMs(P10);
			pic_send_word(0x0000);
			break;
		case P18LF14K22:
			PGD_LOW =0;
			TRISPGD_LOW =0;
			PGC_LOW =0;
			TRISPGC_LOW=0;
		case P18F14K22:
			//direct access to code memory
			pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
			pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
			pic_send(4,0x00,0x84A6);	//BSF EECON1, WREN
			set_address(picfamily, address);
			for(blockcounter=0;blockcounter<(blocksize);blockcounter+=16) //blocks of 16 bytes
			{
				for(i=0;i<14;i+=2)
				{
					//write 2 bytes and post increment by 2
					//				MSB				LSB
					pic_send(4,0x0D,((unsigned int)*(data+blockcounter+i))|
							(((unsigned int)*(data+1+blockcounter+i))<<8));
				}
				//write last 2 bytes of the block and start programming
				pic_send(4,0x0F,((unsigned int)*(data+blockcounter+6))|(((unsigned int)*(data+7+blockcounter))<<8)); 
				pic_send_n_bits(3, 0);
				PGC=1;	//hold PGC high for P9 and low for P10
				DelayMs(P9);
				PGC=0;
				DelayMs(P10);
				pic_send_word(0x0000);
				pic_read_byte2(4,0x09);	//perform 2 reads to increase the address by 2
				pic_read_byte2(4,0x09);
			}
			break;			
		case P18LF13K22:
			PGD_LOW =0;
			TRISPGD_LOW =0;
			PGC_LOW =0;
			TRISPGC_LOW=0;
		case P18FX220:
		case P18FXX31:
		case P18FXX39:
		case P18F6X2X:
		case P18FXX2:
		case P18F13K22:
			if(pictype!=P18FX220&&pictype!=P18F13K22&&pictype!=P18LF13K22)
			{
				//direct access to config memory
				pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
				pic_send(4,0x00,0x8CA6); //BSF EECON1, CFGS
				//configure the device for single panel writes
				set_address(picfamily, 0x3C0006);
				pic_send(4,0x0C,0x0000); //write 0x00 to the tblptr to disable multi-panel writes
			}
			//direct access to code memory
			pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
			pic_send(4,0x00,0x9CA6); //BCF EECON1, CFGS
			if(pictype==P18F13K22||pictype==P18LF13K22)
			{
				pic_send(4,0x00,0x84A6);	//BSF EECON1, WREN

			}
			set_address(picfamily, address);
			for(blockcounter=0;blockcounter<(blocksize);blockcounter+=8) //blocks of 8 bytes
			{
				for(i=0;i<6;i+=2)
				{
					//write 2 bytes and post increment by 2
					//				MSB				LSB
					pic_send(4,0x0D,((unsigned int)*(data+blockcounter+i))|
							(((unsigned int)*(data+1+blockcounter+i))<<8));
				}
				//write last 2 bytes of the block and start programming
				pic_send(4,0x0F,((unsigned int)*(data+blockcounter+6))|(((unsigned int)*(data+7+blockcounter))<<8)); 
				pic_send_n_bits(3, 0);
				PGC=1;	//hold PGC high for P9 and low for P10
				DelayMs(P9);
				PGC=0;
				DelayMs(P10);
				pic_send_word(0x0000);
				pic_read_byte2(4,0x09);	//perform 2 reads to increase the address by 2
				pic_read_byte2(4,0x09);
			}
			break;
		case P18F97J60:
		case P18F45J10:
			if(!(address&0x20)) 
			{
				pic_send(4,0x00,0x84A6); //BSF EECON1, WREN
				set_address(picfamily, address); //blocks of 64 bytes, but divided into two chunks
			}	
			for(blockcounter=0;blockcounter<(blocksize-2);blockcounter+=2)
			{
				//write 2 bytes and post increment by 2
				//				MSB				LSB
				pic_send(4,0x0D,((unsigned int)*(data+blockcounter))|
						(((unsigned int)*(data+1+blockcounter))<<8));
			}
			//write last 2 bytes of the block and start programming
			if(address&0x20)
			{
				pic_send(4,0x0F,((unsigned int)*(data+blockcounter))|(((unsigned int)*(data+1+blockcounter))<<8)); 
				pic_send_n_bits(3, 0);
				PGC=1;	//hold PGC high for P9 and low for P10
				DelayMs(10);
				PGC=0;
				DelayMs(1);
				pic_send_word(0x0000);
			}
			else
			{
				pic_send(4,0x0D,((unsigned int)*(data+blockcounter))|(((unsigned int)*(data+1+blockcounter))<<8)); 
				if(lastblock&2)	//if the last block is the first half of 64 bytes, it needs to be finished with a dummy block to finish.
				{
					for(blockcounter=0;blockcounter<30;blockcounter+=2)
						pic_send(4,0x0D,0xFFFF);
					pic_send(4,0x0F,0xFFFF);
					pic_send_n_bits(3, 0);
					PGC=1;	//hold PGC high for P9 and low for P10
					DelayMs(10);
					PGC=0;
					DelayMs(1);
					pic_send_word(0x0000);
				}
			}
			break;
		case P16F182X:
		case P16F87X:	//same as P16F62X
		case P16F84A:	//same as P16F62X
		case P16F62XA:	//same as P16F62X
		case P16F62X:
		case P12F629:
		case P12F61X:
			if((lastblock&1)&&(address>0))
			{
				set_address(picfamily, address); //set the initial address
			}
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				pic_send_14_bits(6,0x02,(((unsigned int)data[blockcounter]))|   //MSB
						(((unsigned int)data[blockcounter+1])<<8));//LSB
				switch(pictype)
				{
					case P12F61X:
						pic_send_n_bits(6,0x18);    //begin programming
						DelayMs(Tprog);
						pic_send_n_bits(6,0x0A);    //end programming
						break;
					case P16F182X:
						pic_send_n_bits(6,0x08);    //begin programming
						DelayMs(3);
					default:
						pic_send_n_bits(6,0x08);    //begin programming
						DelayMs(Tprog);
						break;
				}
				DelayMs(10);
				/*payload=pic_read_14_bits(6,0x04); //read code memory
				if(payload!=((((unsigned int)data[blockcounter]))|(((unsigned int)data[blockcounter+1])<<8)))
				{
					set_vdd_vpp(pictype, picfamily,0);	//do a hard reset to target processor
					set_vdd_vpp(pictype, picfamily,1);
					DelayMs(10);		//wait a while
					set_address(picfamily, address+(blockcounter>>1)); //go back to the address where it was
				
					pic_send_14_bits(6,0x02,(((unsigned int)data[blockcounter]))|   //MSB
						(((unsigned int)data[blockcounter+1])<<8));//LSB
					switch(pictype)
					{
						case P12F61X:
							pic_send_n_bits(6,0x18);    //begin programming
							DelayMs(Tprog);
							pic_send_n_bits(6,0x0A);    //end programming
							break;
						default:
							pic_send_n_bits(6,0x08);    //begin programming
							DelayMs(Tprog);
							break;
					}
					payload=pic_read_14_bits(6,0x04); //read code memory
					if(payload!=((((unsigned int)data[blockcounter]))|(((unsigned int)data[blockcounter+1])<<8)))
					{
						return 4; //verify error
					}
				}*/
				pic_send_n_bits(6,0x06);	//increment address
			}
			/*if(pictype==P12F629&&((lastblock&2)&&((address+blocksize)<0x3FF))) //restore osccal register
			{
				for(i=0;i<(0x3FF-(address+blocksize));i++)
								pic_send_n_bits(6,0x06);	//increment address
				pic_send_14_bits(6,0x02,osccal);
				pic_send_n_bits(6,0x08);    //begin programming, internally timed
				DelayMs(8);
				//pic_send_n_bits(6,0x0A); 	//end programming
			}*/
			break;
		case P16F72:
		case P16F7X:
		case P16F7X7:
			//2 word programming
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=4) //2 words of data = 4 bytes
			{
				for(i=0;i<4;i+=2)
				{
					pic_send_14_bits(6,0x02,(((unsigned int)data[blockcounter+i]))|   //MSB
							(((unsigned int)data[blockcounter+i+1])<<8));//LSB
					if(i<2)pic_send_n_bits(6,0x06);	//increment address
				}
				pic_send_n_bits(6,0x08);    //begin programming only, externally timed
				DelayMs(2);
				pic_send_n_bits(6,0x0E);    //end programming
				//for(i=0;i<100;i++);		//wait Tdis
				pic_send_n_bits(6,0x06);	//increment address
			}
			break;
		case P16F785:
		case P16F716:
		case P16F87:
		case P16F91X:
		case P16F81X:
		case P12F6XX:
		case P16F88X:
			//4 word programming
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=8) //4 words of data = 8 bytes
			{
				for(i=0;i<8;i+=2)
				{
					pic_send_14_bits(6,0x02,(((unsigned int)data[blockcounter+i]))|   //MSB
							(((unsigned int)data[blockcounter+i+1])<<8));//LSB
					if(i<6)pic_send_n_bits(6,0x06);	//increment address
				}
				switch(pictype)
				{
					case P16F88X:
					case P16F785:
						pic_send_n_bits(6,0x18);    //begin programming
						DelayMs(Tprog);
						pic_send_n_bits(6,0x0A);    //end programming
						break;
					case P16F716:
						pic_send_n_bits(6,0x18);    //begin programming
						DelayMs(Tprog);
						pic_send_n_bits(6,0x0E);    //end programming
						break;
					case P12F6XX:
						pic_send_n_bits(6,0x8);    //begin programming, externally timed
						DelayMs(Tprog);
						break;
					default:
						pic_send_n_bits(6,0x18);    //begin programming only, externally timed
						DelayMs(2);
						pic_send_n_bits(6,0x17);    //end programming
						break;
				}
				//for(i=0;i<100;i++);		//wait Tdis
				pic_send_n_bits(6,0x06);	//increment address
			}
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
				pic_send_n_bits(6,0x08);    //begin erase / programming
				DelayMs(16);
				//pic_send_n_bits(5,0x17);    //end programming
				pic_send_n_bits(6,0x06);	//increment address
			}
			break;
		case P12F508:
		case P16F54:
		case P16F57:
		case P16F59:
		case P10F200:
		case P10F202:
			if(lastblock&1)	
			{
				pic_send_n_bits(6,0x06);//increment address to go from 1FF / 3FF to 0
				set_address(picfamily, address); //set the initial address
				//pic_send_n_bits(6,0x09);//bulk erase, which is necessary anyway...
				//DelayMs(20);
			}
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				pic_send_14_bits(6,0x02,(((unsigned int)data[blockcounter]))|   //MSB
						(((unsigned int)data[blockcounter+1])<<8));//LSB
				DelayMs(1);
				pic_send_n_bits(6,0x08);    //begin programming
				DelayMs(3);
				pic_send_n_bits(6,0x0E);	//end programming
				DelayMs(1);
				//payload=pic_read_14_bits(6,0x04); //read code memory
				//if(payload!=((((unsigned int)data[blockcounter]))|(((unsigned int)data[blockcounter+1])<<8)))
				//{ 
				//	set_vdd_vpp(pictype,picfamily,0);
				//	return 4;//verify error.
				//}
				pic_send_n_bits(6,0x06);	//increment address
			}
			break;
		default:
			set_vdd_vpp(pictype, picfamily,0);
			return 3;
			break;
	}
	if(lastblock&2)
	{
		set_vdd_vpp(pictype, picfamily,0);
		return 1;	//ok
	}
	else
	{
		return 2;	//ask for next block
	}
}



char write_data(PICFAMILY picfamily, PICTYPE pictype, unsigned long address, unsigned char* data, char blocksize, char lastblock)
{
	unsigned int payload;
	char blockcounter,i;
	char receiveddata;
	if(lastblock&1)set_vdd_vpp(pictype, picfamily,1);
	if((pictype==P10F200)||(pictype==P10F202))return 3;	//these devices have no data memory.
	switch(picfamily)
	{
		case dsPIC30:
		case dsP30F_LV:
			//Step 1: Exit the Reset vector.
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x040100);	//GOTO 0x100
			dspic_send_24_bits(0x040100);	//GOTO 0x100
			dspic_send_24_bits(0x000000);	//NOP
			//Step 2: Set the NVMCON to write 16 data words.
			dspic_send_24_bits(0x24005A);	//MOV #0x4005, W10
			dspic_send_24_bits(0x883B0A);	//MOV W10, NVMCON
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=8)
			{
				//Step 3: Initialize the write pointer (W7) for TBLWT instruction.
				//dspic_send_24_bits(0x200000|((blockcounter+address&0xFF0000)>>12));	//MOV #0x7F, W0
				dspic_send_24_bits(0x2007F0);//|((blockcounter+address&0xFF0000)>>12));	//MOV #0x7F, W0
				dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
				dspic_send_24_bits(0x2F0007|((((unsigned long)blockcounter+address)&0x0FFF)<<4));	//MOV #<DestinationAddress15:0>, W7
				//Step 4: Load W0:W3 with the next 4 data words to program.
				dspic_send_24_bits(0x200000|(((unsigned long)data[blockcounter])<<4)|(((unsigned long)data[blockcounter+1])<<12));
				dspic_send_24_bits(0x200001|(((unsigned long)data[blockcounter+2])<<4)|(((unsigned long)data[blockcounter+3])<<12));
				dspic_send_24_bits(0x200002|(((unsigned long)data[blockcounter+4])<<4)|(((unsigned long)data[blockcounter+5])<<12));
				dspic_send_24_bits(0x200003|(((unsigned long)data[blockcounter+6])<<4)|(((unsigned long)data[blockcounter+7])<<12));
				/*for(i=0;i<4;i++)
				{
					dspic_send_24_bits(0x200000|
							(((unsigned long)data[blockcounter+(i*2)])<<4)|
							(((unsigned long)data[blockcounter+(i*2)+1])<<12)|
							((unsigned long) i));
				}*/
				//Step 5: Set the read pointer (W6) and load the (next set of) write latches.
				dspic_send_24_bits(0xEB0300);	//CLR W6
				dspic_send_24_bits(0x000000);	//NOP
				for(i=0;i<4;i++)
				{
					dspic_send_24_bits(0xBB1BB6);	//TBLWTL [W6++], [W7++]
					dspic_send_24_bits(0x000000);	//NOP
					dspic_send_24_bits(0x000000);	//NOP
				}
			}//Step 6: Repeat steps 3-4 four times to load the write latches for 16 data words.
			//Step 7: Unlock the NVMCON for writing.
			dspic_send_24_bits(0x200558);	//MOV #0x55, W8
			dspic_send_24_bits(0x883B38);	//MOV W8, NVMKEY
			dspic_send_24_bits(0x200AA9);	//MOV #0xAA, W9
			dspic_send_24_bits(0x883B39);	//MOV W9, NVMKEY
			//Step 8: Initiate the write cycle.
			dspic_send_24_bits(0xA8E761);	//BSET NVMCON, #WR
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			DelayMs(2);						//Externally time 2 msec
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0xA9E761);	//BCLR NVMCON, #WR
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			//Step 9: Reset device internal PC.
			dspic_send_24_bits(0x040100);	//GOTO 0x100
			dspic_send_24_bits(0x000000);	//NOP
			break;
		case PIC18:
		case PIC18J:
			if(pictype==P18LF13K22||pictype==P18LF14K22)
			{
				PGD_LOW =0;
				TRISPGD_LOW =0;
				PGC_LOW =0;
				TRISPGC_LOW=0;	
			}
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
				if((pictype==P18FXX2)||(pictype==P18FXX31)||(pictype==P18FX220))
				{
					//Step 5: Perform required sequence.
					pic_send(4,0,0x0E55); //MOVLW 0X55
					pic_send(4,0,0x6EA7); //MOVWF EECON2
					pic_send(4,0,0x0EAA); //MOVLW 0XAA
					pic_send(4,0,0x6EA7); //MOVWF EECON2
				}
				pic_send(4,0x00,0x82A6); //BSF EECON1, WR
				if((pictype==P18F4XK22)||(pictype==P18LF4XK22))
				{
					pic_send(4,0x00,0x0000); //NOP, two NOPs required before polling
					pic_send(4,0x00,0x0000); //NOP, write starts on 4th clock of this instruction
				}				
				//pic_send(4,0x00,0x0000); //NOP, when not polling for the WR bit, the PIC still needs at least 4 clocks
				startTimerMs( P11A );
				do
				{
					pic_send(4,0x00,0x50A6); //movf EECON1, W, 0
					pic_send(4,0x00,0x6EF5); //movwf TABLAT
					pic_send(4,0x00,0x0000); //nop
					receiveddata=pic_read_byte2(4,0x02); //Shift TABLAT register out
				}while(((receiveddata&0x02)==0x02) && timerRunning); //poll for WR bit to clear
				//PGC=0;	//hold PGC low for P10 (100us)
				DelayMs(P10);
				pic_send(4,0x00,0x94A6); //BCF EECON1, WREN
			}
			break;
		case PIC18K:	
			pic_send(4,0x00,0x9E7F); //BCF EECON1, EEPGD
			pic_send(4,0x00,0x9C7F); //BCF EECON1, CFGS
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
			{
				pic_send(4,0x00,(0x0E00|( address+(unsigned int)blockcounter)    &0xFF)); //MOVLW Addr [7:0]
				pic_send(4,0x00,0x6E62); //MOVWF EEADR
				pic_send(4,0x00,(0x0E00|((address+(unsigned int)blockcounter)>>8)&0xFF)); //MOVLW Addr [15:8]
				pic_send(4,0x00,0x6E63); //MOVWF EEADRH
				pic_send(4,0x00,0x0E00|(unsigned int)data[blockcounter]); //MOVLW data
				pic_send(4,0x00,0x6e61); //MOVWF EEDATA
				pic_send(4,0x00,0x847F); //BSF EECON1, WREN
				pic_send(4,0x00,0x827F); //BSF EECON1, WR
				
				//pic_send(4,0x00,0x0000); //NOP, when not polling for the WR bit, the PIC still needs at least 4 clocks
				startTimerMs( P11A );
				do
				{
					pic_send(4,0x00,0x507F); //movf EECON1, W, 0
					pic_send(4,0x00,0x6EF5); //movwf TABLAT
					pic_send(4,0x00,0x0000); //nop
					receiveddata=pic_read_byte2(4,0x02); //Shift TABLAT register out
				}while(((receiveddata&0x02)==0x02) && timerRunning ); //poll for WR bit to clear
				//PGC=0;	//hold PGC low for P10 (100us)
				DelayMs(P10);
				pic_send(4,0x00,0x947F); //BCF EECON1, WREN
			}
			break;
		case PIC16:
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
			{
				//load data
				pic_send_14_bits(6,0x03,((unsigned int)data[blockcounter]));//LSB only
				//begin programming command
				switch(pictype)
				{
					case P16F785:
					case P12F629:
					case P16F84A:
					case P16F87:
					case P16F81X:
					case P16F91X:
						pic_send_n_bits(6,0x18);// begin programming only cycle
						break;
					default:
						pic_send_n_bits(6,0x08);// begin erase programming cycle
						break;
				}
				//wait Tprog
				// 8MS, should be 20 ms for other?
				DelayMs(Tdprog);
				switch(pictype)
				{
					case P16F87:
					case P16F81X:
						pic_send_n_bits(6,0x17);//end programming
						break;
					case P16F785:
					case P12F629:
					case P16F91X:
						pic_send_n_bits(6,0x0A);//end programming
						break;
				}
				//read data from data memory (to verify) not yet impl...
				//increment address
				pic_send_n_bits(6,0x06);
			}
			break;
		default:
			set_vdd_vpp(pictype, picfamily,0);
			return 3;     //unknown pic type
			break;
	}
	if(lastblock&2)
	{
		set_vdd_vpp(pictype, picfamily,0);
		return 1;	//ok
	}
	else
	{
		return 2;	//ask for next block
	}
}

/**
use program_memory in stead with address=0x200000 and blocksize=8
void program_ids(PICFAMILY picfamily,char address, char* data, char blocksize)
{
}
**/

/**
program_config_bits writes 2 configuration bytes each time (*data has to be 2 bytes big)
the address will be 0x300000 + the id location
before calling this function, make configstate CONFIGSTART
keep calling this function until configstate==CONFIGSUCCESS
**/
char write_config_bits(PICFAMILY picfamily, PICTYPE pictype, unsigned long address, unsigned char* data, char blocksize, char lastblock)
{
	char i;
	static char blockcounter;
	unsigned int payload;
	if(lastblock&1)set_vdd_vpp(pictype, picfamily,1);
	switch(pictype)
	{
		case dsP30F:
			if((lastblock&1)==1)
			{
				//Step 1: Exit the Reset vector.
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
				//Step 2: Initialize the write pointer (W7) for the TBLWT instruction.
				
				dspic_send_24_bits(0x200007);//|((address&0xFFFF)<<4));	//MOV #0x0000, W7
			}
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				//Step 3: Set the NVMCON to program 1 Configuration register.
				dspic_send_24_bits(0x24008A);	//MOV #0x4008, W10
				dspic_send_24_bits(0x883B0A);	//MOV W10, NVMCON
				//Step 4: Initialize the TBLPAG register.
				//dspic_send_24_bits(0x200000|((blockcounter+address&0xFF0000)>>12));	//MOV #0xF8, W0
				dspic_send_24_bits(0x200F80);//|((blockcounter+address&0xFF0000)>>12));	//MOV #0xF8, W0
				dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
				//Step 5: Load the Configuration register data to W6.
				payload=(((unsigned int)data[blockcounter])|(((unsigned int)data[blockcounter+1])<<8));
				dspic_send_24_bits(0x200006|((((unsigned long)payload)&0xFFFF)<<4));	//MOV #<CONFIG_VALUE>, W6
				//dspic_send_24_bits(0x200036);	//write 0x0003 in the config register
				dspic_send_24_bits(0x000000);	//NOP
				//Step 6: Write the Configuration register data to the write latch and increment the write pointer.
				dspic_send_24_bits(0xBB1B86);	//TBLWTL W6, [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				//Step 7: Unlock the NVMCON for programming.
				dspic_send_24_bits(0x200558);	//MOV #0x55, W8
				dspic_send_24_bits(0x883B38);	//MOV W8, NVMKEY
				dspic_send_24_bits(0x200AA9);	//MOV #0xAA, W9
				dspic_send_24_bits(0x883B39);	//MOV W9, NVMKEY
				//Step 8: Initiate the write cycle.
				dspic_send_24_bits(0xA8E761);	//BSET NVMCON, #WR
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				DelayMs(2);						//Externally time 2 msec
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xA9E761);	//BCLR NVMCON, #WR
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				//Step 9: Reset device internal PC.
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
			}//Step 10: Repeat steps 3-9 until all 7 Configuration registers are cleared.
			break;
		case P18F14K22:
		case P18LF14K22:
		case P18F872X:
		case P18FX220:
		case P18FXX39:
		case P18F6X2X:
		case P18FXX2:
		case P18F2XXX:
		case P18F4XK22:
		case P18LF4XK22:	
			if((pictype==P18F6XKXX)	|| (pictype==P18F67KXX))
			{
				pic_send(4,0x00,0x8E7F); //BSF EECON1, EEPGD
				pic_send(4,0x00,0x8C7F); //BSF EECON1, CFGS
			}
			else
			{
				pic_send(4,0x00,0x8EA6); //BSF EECON1, EEPGD
				pic_send(4,0x00,0x8CA6); //BSF EECON1, CFGS
				if((pictype==P18F4XK22)||
					(pictype==P18LF4XK22)||
					(pictype==P18F13K22)||
					(pictype==P18LF13K22)||
					(pictype==P18F14K22)||
					(pictype==P18LF14K22))
				{
					pic_send(4,0x00,0x84A6); //BSF EECON1, WREN			
				}
				if((pictype==P18FXX2)||(pictype==P18FXX31)||(pictype==P18FXX31))
				{
					//goto 0x100000
					pic_send(4,0x00,0xEF00);
					pic_send(4,0x00,0xF800);
				}
			}
			//address=0x300000;
			//start for loop
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				set_address(picfamily, address+((unsigned int)blockcounter));
				//LSB first
				pic_send(4,0x0F,((unsigned int)*(data+blockcounter))|(((unsigned int)*(data+blockcounter))<<8));
				pic_send_n_bits(3, 0);
				PGC=1;	//hold PGC high for P9 (or P9A for 4XF/LFK22 config word)
				if((pictype==P18F4XK22)||(pictype==P18LF4XK22))
				{
				        DelayMs(P9A);
				} 
				else 
				{
				        DelayMs(P9);
				}
				PGC=0;	//hold PGC low for time P10
				DelayMs(P10);
				pic_send_word(0x0000); //last part of the nop
				set_address(picfamily, address+((unsigned int)blockcounter)+1);
				pic_send(4,0x0F, ((unsigned int)*(data+1+blockcounter))|(((unsigned int)*(data+1+blockcounter))<<8)); //load MSB and start programming
				pic_send_n_bits(3, 0);
				PGC=1;	//hold PGC high for P9 (or P9A for 4XF/LFK22 config word)
				if((pictype==P18F4XK22)||(pictype==P18LF4XK22))
				{
				        DelayMs(P9A);
				} else {
				        DelayMs(P9);
				}
				PGC=0;	//hold PGC low for time P10
				DelayMs(P10);
				pic_send_word(0x0000); //last part of the nop
			}
			break;		
		case P12F629:
			/*if(lastblock&1)
			{
				pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
				for(i=0;i<((char)address);i++)pic_send_n_bits(6,0x06);   //increment address until ADDRESS is reached
			}
                        for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				//load data for config memory
				if(((((char)address)+(blockcounter>>1))<4))
				{
					pic_send_14_bits(6,0x02,(((unsigned int)data[blockcounter]))|   //MSB
							(((unsigned int)data[blockcounter+1])<<8));//LSB
					pic_send_n_bits(6,0x08);    //begin programming
					DelayMs(Tprog);
				}
				if(((((char)address)+(blockcounter>>1))==7))      //restore bandgap bits
				{
					payload=bandgap|(0x0FFF&((((unsigned int)data[blockcounter+1])<<8)|   //MSB
							(((unsigned int)data[blockcounter]))));
					pic_send_14_bits(6,0x02,payload);
					pic_send_n_bits(6,0x08);    //begin programming
					DelayMs(Tprog);
				}
				//read data from program memory (to verify) not yet impl...
				pic_send_n_bits(6,0x06);	//increment address
			}
			break;*/
		case P16F785:
		case P16F87:
		case P16F88X:
		case P16F716:
		case P16F72:
		case P16F7X:
		case P16F7X7:
		case P16F87X:
		case P16F91X:
		case P16F81X:
		case P16F84A:
		case P16F87XA:
		case P12F6XX: //same as P16F62X
		case P16F62XA: //same as P16F62X
		case P16F62X:
		case P12F61X:
		case P16F182X:
			if(lastblock&1)
			{
				pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
				for(i=0;i<((char)address);i++)pic_send_n_bits(6,0x06);   //increment address until ADDRESS is reached
			}
                        for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				//load data for config memory
				if(((((char)address)+(blockcounter>>1))<4)||
					((((char)address)+(blockcounter>>1))==7)||
					(((((char)address)+(blockcounter>>1))==8)&&((pictype==P16F87)|(pictype==P16F7X7)|(pictype==P16F88X))))
				{
					payload=(((unsigned int)data[blockcounter]))|(((unsigned int)data[blockcounter+1])<<8);
					pic_send_14_bits(6,0x02,payload); //load data for programming
					switch(pictype)
					{
						case P16F87:
						case P16F785:
						case P16F716:
						case P12F61X:
							pic_send_n_bits(6,0x18);    //begin programming
						default:
							pic_send_n_bits(6,0x08);    //begin programming
							break;
					}
					DelayMs(Tprog);
					switch(pictype)
					{
						case P16F716:
						case P16F72:
						case P16F7X:
						case P16F7X7:
							pic_send_n_bits(6,0x0E);    //end programming
							break;
						case P16F785:
						case P12F61X:
							pic_send_n_bits(6,0x0A);	//end programming
							break;
						case P16F87:
							pic_send_n_bits(6,0x17);	//end programming
							break;
					}
				}
				//read data from program memory (to verify) not yet impl...
				pic_send_n_bits(6,0x06);	//increment address
			}
			break;
		case P12F508:
		case P16F54:
		case P16F57:
		case P16F59:
		case P10F200:
		case P10F202:
                        for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				payload=(((unsigned int)data[blockcounter]))|(((unsigned int)data[blockcounter+1])<<8);
				pic_send_14_bits(6,0x02,payload); //load data for programming
				pic_send_n_bits(6,0x08);    //begin programming
				DelayMs(Tprog);
				pic_send_n_bits(6,0x0E);    //end programming
				pic_send_n_bits(6,0x06);	//increment address
			}
			break;
			
		default:
			set_vdd_vpp(pictype, picfamily,0);
			return 3;
			break;
	}
	if(lastblock&2)
	{
		set_vdd_vpp(pictype, picfamily,0);
		return 1;	//ok
	}
	else
	{
		return 2;	//ask for next block
	}
}

/**
read_program will read program memory, id's and configuration bits
**/
char read_code(PICFAMILY picfamily, PICTYPE pictype, unsigned long address, unsigned char* data, char blocksize, char lastblock)
{
	unsigned int configAddress=0;
	unsigned int i;
	char blockcounter=0;
	unsigned int payload;
	unsigned int ConfigOffset;
	if(lastblock&1)set_vdd_vpp(pictype, picfamily,1);
	switch(picfamily)
	{
		case I2C:
			I2C_start();
			I2C_write(0xA0|((unsigned char)((address&&0x10000)>>13))); //Device Address + 0=write
			if(pictype==I2C_EE_2)
				I2C_write((unsigned char)((address&0xFF00)>>8)); //MSB
			I2C_write((unsigned char)((address&0x00FF))); //LSB
			I2C_start();
			I2C_write(0xA1|((unsigned char)((address&&0x10000)>>13))); //Device Address + 1=read
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
			{
				data[blockcounter] = I2C_read(0);
			}
			I2C_stop();
			break;
		case dsPIC30:
		case dsP30F_LV:
			if(address>=0xF80000)
			{
				if(lastblock&1)
				{
					//Step 1: Exit the Reset vector.
					dspic_send_24_bits(0x000000);	//NOP
					dspic_send_24_bits(0x000000);	//NOP
					dspic_send_24_bits(0x040100);	//GOTO 0x100
					dspic_send_24_bits(0x000000);	//NOP
					//Step 2: Initialize TBLPAG, and the read pointer (W6) and the write pointer (W7) for TBLRD instruction.
					dspic_send_24_bits(0x200000|((address&0xFF0000)>>12));	//MOV #0xF8, W0
					dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
					//dspic_send_24_bits(0x200006|((address&0x00FFFF)<<4));	//MOV #<SourceAddress15:0>, W6
					//ignore the lower 16 bits off the address, they are always 0.
					dspic_send_24_bits(0xEB0300);	//CLR W6
					dspic_send_24_bits(0xEB0380);	//CLR W7
					dspic_send_24_bits(0x000000);	//NOP
				}
				for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
				{
					//Step 3: Read the Configuration register and write it to the VISI register (located at 0x784).
					dspic_send_24_bits(0xBA0BB6);	//TBLRDL [W6++], [W7]
					dspic_send_24_bits(0x000000);	//NOP
					dspic_send_24_bits(0x000000);	//NOP
					dspic_send_24_bits(0x883C20);	//MOV W0, VISI
					dspic_send_24_bits(0x000000);	//NOP
					//Step 4: Output the VISI register using the REGOUT command.
					payload=dspic_read_16_bits(pictype==dsP30F_LV);	//read <VISI>
					data[blockcounter]=(unsigned char)payload;
					data[blockcounter+1]=(unsigned char)((payload&0xFF00)>>8);
					dspic_send_24_bits(0x000000);	//NOP
					//Step 5: Reset device internal PC.
					dspic_send_24_bits(0x040100);	//GOTO 0x100
					dspic_send_24_bits(0x000000);	//NOP
				}
				//Step 6: Repeat steps 3-5 six times to read all of configuration memory.
			}
			else
			{
				//Step 1: Exit the Reset vector.
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
				//Step 2: Initialize TBLPAG and the read pointer (W6) for TBLRD instruction.
				dspic_send_24_bits(0x200000|(((((address)*2)/3)&0xFF0000)>>12));	//MOV #<SourceAddress23:16>, W0
				dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
				dspic_send_24_bits(0x200006|(((((address)*2)/3)&0x00FFFF)<<4));	//MOV #<SourceAddress15:0>, W6
				//Step 3: Initialize the write pointer (W7) and store the next four locations of code memory to W0:W5.
				dspic_send_24_bits(0xEB0380);	//CLR W7
				//dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBA1B96);	//TBLRDL [W6], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBADBB6);	//TBLRDH.B [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBADBD6);	//TBLRDH.B [++W6], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBA1BB6);	//TBLRDL [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBA1B96);	//TBLRDL [W6], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBADBB6);	//TBLRDH.B [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBADBD6);	//TBLRDH.B [++W6], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBA0BB6);	//TBLRDL [W6++], [W7]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				//Step 4: Output W0:W5 using the VISI register and REGOUT command.
				for(i=0;i<6;i++)
				{
					dspic_send_24_bits(0x883C20|(unsigned long) i);	//MOV W0, VISI
					dspic_send_24_bits(0x000000);	//NOP
					payload=dspic_read_16_bits(pictype==dsP30F_LV);	//Clock out contents of VISI register
					data[blockcounter+i*2]=(unsigned char)payload&0xFF;
					data[blockcounter+i*2+1]=(unsigned char)((payload&0xFF00)>>8);
					dspic_send_24_bits(0x000000);	//NOP
				}
				//Step 5: Reset the device internal PC.
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
			}
			break;
		case PIC18J:
		case PIC18:
			set_address(picfamily, address);
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)
			{
				*(data+blockcounter)=pic_read_byte2(4,0x09);
				if(picfamily==PIC18J)TRISPGD_LOW=0;	//switch to 3.3V again
				if(pictype==P18LF13K22||pictype==P18LF14K22)TRISPGD_LOW=0;	//switch to 3.3V again
			}
			break;
		case PIC16:
			if(pictype==P16F182X)ConfigOffset=0x8000;
			else ConfigOffset=0x2000;
			if(address>=ConfigOffset) //read configuration memory
			{
				pic_send_14_bits(6,0x00,0x0000);//Execute a Load Configuration command (dataword 0x0000) to set PC to 0x2000.
				if(lastblock&1)
				{
					for(i=0;i<(((unsigned int)address)-ConfigOffset);i++)pic_send_n_bits(6,0x06);	//increment address
				}
				for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
				{
					payload=pic_read_14_bits(6,0x04); //read code memory
					data[blockcounter+1]=(char)(payload>>8);
					data[blockcounter]=(char)payload;
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
					data[blockcounter+1]=(char)(payload>>8);
					data[blockcounter]=(char)payload;
					pic_send_n_bits(6,0x06);	//increment address
				}
			}
			break;
		case PIC10:
			switch(pictype)
			{
				case P10F200:
					configAddress=0x1FF;
					break;
				case P10F202:
				case P16F54:
					configAddress=0x3FF;
					break;
				case P16F57:
				case P16F59:
					configAddress=0xFFF;
					break;
			}
			if(address==configAddress)
			{			
				payload=pic_read_14_bits(6,0x04); //read config memory
				data[1]=(char)(payload>>8);
				data[0]=(char)payload;
			}
			else
			{
				if(lastblock&1)
				{
					//pic_read_14_bits(6,0x04); //read code memory
					pic_send_n_bits(6,0x06);	//increment address
		//			for(i=0;i<10;i++);
					//pic_read_14_bits(6,0x04); //read code memory
		//			for(i=0;i<10;i++);
					for(i=0;i<(unsigned int)address;i++)
					{
						pic_send_n_bits(6,0x06);	//increment address
		//				for(i=0;i<10;i++);
					}
				}
				for(blockcounter=0;blockcounter<blocksize;blockcounter+=2)
				{
					payload=pic_read_14_bits(6,0x04); //read code memory
		//			for(i=0;i<10;i++);
					data[blockcounter+1]=(char)(payload>>8);
					data[blockcounter]=(char)payload;
					pic_send_n_bits(6,0x06);	//increment address
		//			for(i=0;i<10;i++);
				}
			}
			break;
		default:
			for(blockcounter=0;blockcounter<blocksize;blockcounter++)         //fill with zeros
				*(data+blockcounter)=0;
			break;
	}
	if(lastblock&2)set_vdd_vpp(pictype, picfamily,0);

}


/**
This function reads a block of data from the data eeprom of size blocksize into *data
call this function only once.
**/
unsigned char read_data(PICFAMILY picfamily, PICTYPE pictype, unsigned long address, unsigned char* data, char blocksize, char lastblock)
{
	
	unsigned int i,payload;
	char blockcounter=0;
	//if(lastblock&1)
	if(lastblock&1)set_vdd_vpp(pictype, picfamily,1);
	switch(picfamily)
	{
		case dsPIC30:
		case dsP30F_LV:
			
			//Step 1: Exit the Reset vector.
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x040100);	//GOTO 0x100
			dspic_send_24_bits(0x000000);	//NOP
			//Step 2: Initialize TBLPAG and the read pointer (W6) for TBLRD instruction.
			dspic_send_24_bits(0x2007F0);	//MOV #0x7F, W0
			dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
			dspic_send_24_bits(0x2F0006|((unsigned long)((address&0x0FFF)<<4)));	//MOV #<SourceAddress15:0>, W6
			for(blockcounter=0;blockcounter<blocksize;blockcounter+=8)
			{				
				//Step 3: Initialize the write pointer (W7) and store the next four locations of code memory to W0:W5.
				dspic_send_24_bits(0xEB0380);	//CLR W7
				dspic_send_24_bits(0x000000);	//NOP
				for(i=0;i<4;i++)
				{
					dspic_send_24_bits(0xBA1BB6);	//TBLRDL [W6++], [W7++]
					dspic_send_24_bits(0x000000);	//NOP
					dspic_send_24_bits(0x000000);	//NOP
				}
				//Step 4: Output W0:W5 using the VISI register and REGOUT command.
				for(i=0;i<4;i++)
				{
					dspic_send_24_bits(0x883C20|(unsigned long)i);	//MOV W0, VISI
					dspic_send_24_bits(0x000000);	//NOP
					payload=dspic_read_16_bits(pictype==dsP30F_LV);	//VISI
					data[blockcounter+(i*2)]=(unsigned char)payload;
					data[blockcounter+((i*2)+1)]=(unsigned char)((payload&0xFF00)>>8);
					dspic_send_24_bits(0x000000);	//NOP
				}
				//Step 5: Reset device internal PC.
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
			}
			break;
		case PIC18:
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
				if(pictype==P18LF13K22||pictype==P18LF14K22)TRISPGD_LOW=0;	//switch to 3.3V again
			}
			break;
		case PIC16:
			if((lastblock&1)&&(address>0))
			{
				data[0]=pic_read_14_bits(6,0x05);	//read first byte
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
	if(lastblock&2)set_vdd_vpp(pictype, picfamily,0);
	return 0;
}

		


