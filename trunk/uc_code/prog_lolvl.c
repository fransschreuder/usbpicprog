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

#ifdef __XC8
#include <xc.h>
#elif defined(SDCC)
#include <pic18f2550.h>
#else
#include <p18cxxx.h>
#endif
#include "typedefs.h"
#include "interrupt.h"
#include "prog.h"
#include "upp.h" 
#include "io_cfg.h"             // I/O pin mapping
#include "prog_lolvl.h"
#include "debug_link.h"

#ifdef TEST
#undef I2C_delay
#undef set_vdd_vpp
#undef exit_ISCP
#undef enter_ISCP
#elif !defined(__XC8)
#include <delays.h>
#endif
#ifdef __XC8
#define I2C_delay()	_delay(20)		// same as C18's Delay10TCYx(2)
#else
#define I2C_delay()	Delay10TCYx(2)		// approx 2x 1.3us min
#endif

unsigned char ConfigDisableVDD=0;
unsigned char ConfigLimitVPP=0;
unsigned char ConfigLimitPGDPGC=0;

void set_vdd_vpp( PICTYPE pictype, PICFAMILY picfamily, char level )
{
	if( level == 0 )
		exit_ISCP();
	else
		enter_ISCP();
}

unsigned char iscp_active = 0;
unsigned char prog_error = 0;

static void eprom_pulse( unsigned int payload )
{
	pic_send_14_bits( 6, 0x02, payload ); //a load before every begin programming
	pic_send_n_bits( 6, 0x08 ); //begin programming
	DelayUs( 100 );
	pic_send_n_bits( 6, 0x0E ); //end programming
}

/*
 * Program one word of a PIC16C EPROM part (DS30228 figures 2-2 and 2-3) at
 * the current address: 100us pulses until the word verifies, at most 25 (N),
 * then 3*N more to overprogram. The configuration word instead gets 100
 * pulses and one verify. A word that does not verify sets prog_error.
 */
void program_eprom_word( unsigned int payload, char config_word )
{
	unsigned char n, k;

	payload &= 0x3FFF;
	if( config_word )
	{
		for( k = 0; k < 100; k++ )
			eprom_pulse( payload );
		if( (pic_read_14_bits( 6, 0x04 ) & 0x3FFF) != payload )
			prog_error = 1;
		return;
	}
	for( n = 1; n <= 25; n++ )
	{
		eprom_pulse( payload );
		if( (pic_read_14_bits( 6, 0x04 ) & 0x3FFF) == payload )
		{
			for( k = 0; k < 3 * n; k++ )
				eprom_pulse( payload );
			return;
		}
	}
	prog_error = 1;
}

void enter_ISCP( void )
{
	// A host that gave up in the middle of a session leaves the target in
	// programming mode. Leave it first: re-entering without a reset would,
	// for example, keep a PIC16's address counter from the old session.
	if( iscp_active )
		exit_ISCP();
	// End a debug session first: it leaves the target running and the PGD
	// pull-down on.
	if( debug_attached )
	{
		debug_detach( 0 );
		DelayMs( 200 );
	}
	// Every supported family has an entry routine; none means no valid
	// pictype was selected, so apply no voltages at all. (This used to fall
	// back to enter_ISCP_simple, i.e. VDD and ~12V VPP.)
	if( !currDevice.enter_ISCP )
		return;
	currDevice.enter_ISCP();
	iscp_active = 1;
}
void enter_ISCP_simple()
{
	enablePGC_D(); //PGC/D output & PGC/D_LOW appropriate
	PGDlow(); // initial value for programming mode
	PGClow(); // initial value for programming mode

	clock_delay(); // dummy tempo
	VDDon(); //high, (inverted)
	DelayMs( 100 );
	VPPon(); //high, (inverted)
	DelayMs( 100 );
}
void enter_ISCP_P16_Vpp()
{
	//			P16F62X, P16F62XA, P12F629, P12F6XX, P16F87;		//VPP first
	enablePGC_D(); //PGC/D output & PGC/D_LOW appropriate

	PGDlow(); // initial value for programming mode
	PGClow(); // initial value for programming mode
	clock_delay(); // dummy tempo
	VPPon();
	DelayMs( 100 );
	VDDon();
	DelayMs( 100 );
}
/*
 * Low-voltage entry for enhanced mid-range PIC12F/16F1xxx (DS41390 section
 * 4.2, DS41573 section 4.2). Their VIHH is 8.0-9.0V, below this programmer's
 * ~12V VPP, so no high voltage is used: MCLR is held at VIL, the 32-bit key
 * "MCHP" (0x4D434850) is clocked in LSb first followed by a 33rd clock, and
 * MCLR stays at VIL while programming. Needs the LVP configuration bit set,
 * which is the erased default; it cannot be cleared while in LVP mode.
 */
void enter_ISCP_P16_LVP()
{
	enablePGC_D(); //PGC/D output & PGC/D_LOW appropriate
	PGDlow();
	PGClow();
	VPPoff();
	VPP_RUNoff();
	VPP_RSTon(); //MCLR at 0V
	VDDon();
	DelayMs( 10 );
	pic_send_word( 0x4850 ); //key, LSb first: low word...
	pic_send_word( 0x4D43 ); //...then high word
	pic_send_n_bits( 1, 0 ); //33rd clock
	DelayMs( 1 );
}

/*
 * Low-voltage entry for PIC18F1XK22/LF1XK22 (DS41357B section 3.3). Their
 * VIHH is 8-9V, below this programmer's ~12V VPP, and they have no key
 * sequence: low-voltage mode is selected by the RC3/PGM pin being high while
 * MCLR rises from 0V to VDD. PGM is not on the ICSP header, so it must be
 * held high on the target (for example with a jumper to VDD) while
 * programming. Without it the target simply starts running.
 */
void enter_ISCP_PIC18_PGM()
{
	enablePGC_D(); //PGC/D output & PGC/D_LOW appropriate
	PGDlow();
	PGClow();
	VPPoff();
	VPP_RUNoff();
	VPP_RSTon(); //MCLR at 0V
	VDDon();
	DelayMs( 10 ); //also P15: PGM high 2us before MCLR rises
	VPP_RSToff(); //release MCLR...
	VPP_RUNon(); //...to VDD level
	DelayMs( 1 ); //P12: 2us before data
}

/*
 * dsPIC30F ICSP entry, DS70102K Figure 11-4 (VIHH 9.00-13.25V): with PGC/PGD
 * low, VDD up, MCLR to VIHH for 4ms, MCLR low for at least 10us, back to
 * VIHH and wait 4ms. The first SIX command after entry needs five extra PGC
 * clocks (section 11.2, note 1), which were missing before 1.1.0, as was a
 * real 10us low pulse on MCLR.
 */
void enter_ISCP_dsPIC30()
{
	enablePGC_D(); //PGC/D output & PGC/D_LOW appropriate

	PGDlow(); // initial value for programming mode
	PGClow(); // initial value for programming mode
	clock_delay(); // dummy tempo
	VDDon();
	DelayMs( 100 ); // let the target's supply settle
	VPPon();
	DelayMs( 4 );
	VPPoff();
	VPP_RSTon(); // MCLR low...
	DelayUs( 20 ); // ...for at least 10us
	VPP_RSToff();
	VPPon();
	DelayMs( 100 ); // at least 4ms before the first command
	pic_send_n_bits( 5, 0 ); // five extra clocks: the first SIX is 9 bits
}

/*
 * Key-sequence entry without high voltage: MCLR briefly high then low, key
 * "MCHP" MSb first, then MCLR high and held. Used for PIC18 J parts and, per
 * DS41398B section 2.4 (VIHH max 9V), for PIC18(L)F2XK22/4XK22.
 */
void enter_ISCP_PIC18J()
{
	int i;

	enablePGC_D(); //PGC/D output & PGC/D_LOW appropriate

	VPP_RUNoff(); //MCLR low
	VDDon();
	DelayMs( 10 );
	VPP_RUNon(); //VPP to 4.5V
	for( i = 0; i < 300; i++ )
		continue; //aprox 0.5ms
	VPP_RUNoff(); //and immediately back to 0...
	VPP_RSTon();
	DelayMs( 4 );		//FIXME: should be 4ms only?
	DelayMs( 6 );
	//clock_delay();	//P19 = 40ns min
	//write 0x4D43, high to low, other than the rest of the commands which are low to high...
	//0x3D43 => 0100 1101 0100 0011
	//from low to high => 1100 0010 1011 0010
	//0xC2B2
	pic_send_word( 0xC2B2 );
	//write 0x4850 => 0100 1000 0101 0000 => 0000 1010 0001 0010 => 0x0A12
	pic_send_word( 0x0A12 );
	DelayMs( 2 );
	VPP_RSToff(); //release from reset
	VPP_RUNon();
	DelayMs( 1 );

}

void enter_ISCP_PIC18K()
{
	int i;

	enablePGC_D(); //PGC/D output & PGC/D_LOW appropriate

	// Enter low voltage programming mode:
	// Pulse MCLR low, then high, then low
	VPP_RUNoff(); // MCLR low
	VPP_RSTon(); // Force MCLR low
	DelayMs( 1 ); // Small delay

	// Turn VDD supply on
	VDDon();
	DelayMs( 10 ); // Allow voltage to stabilize

	// MCLR high
	VPP_RSToff(); // Release from reset
	VPP_RUNon(); // VPP to 4.5V
	DelayMs( 3 ); // Allow to stabilize

	// MCLR low, and write secret word
	VPP_RUNoff(); //MCLR low (this would enter low power mode)
 	VPP_RSTon(); // Force MCLR low (this would enter low power mode)
	DelayMs( 1 ); // P12: at least 250us from MCLR low to the key (DS30009947C); the loop was ~200us
	//clock_delay();	//P19 = 40ns min
	//write 0x4D43, high to low, other than the rest of the commands which are low to high...
	//0x3D43 => 0100 1101 0100 0011
	//from low to high => 1100 0010 1011 0010
	//0xC2B2
	pic_send_word( 0xC2B2 );
	//write 0x4850 => 0100 1000 0101 0000 => 0000 1010 0001 0010 => 0x0A12
	pic_send_word( 0x0A12 );

	// Turn MCLR back on
	DelayMs( 1 ); // <- IF ...
	VPP_RUNon(); // ... Low power ...
	VPP_RSToff(); // ... Mode is used ...
	DelayMs( 1 ); // Some time for MCLR to rise
}

void enter_ISCP_PIC24()
{

    enablePGC_D(); //PGC/D output & PGC/D_LOW appropriate

    VPP_RUNoff(); //MCLR low
    VDDon();
    DelayMs( 10 );
    VPP_RUNon(); //VPP to 4.5V
    DelayMs( 1 );                       // PIC24E 500us min
    VPP_RUNoff(); //and immediately back to 0...
    VPP_RSTon();
    DelayMs( 2 );   //P19 = 40ns min        PIC24E 1ms min
    //write 0x4D43, high to low, other than the rest of the commands which are low to high...
    //0x4D43 => 0100 1101 0100 0011
    //from low to high => 1100 0010 1011 0010
    //0xC2B2
    pic_send_word( 0xC2B2 );
    //write 0x4851 => 0100 1000 0101 0001 => 1000 1010 0001 0010 => 0x8A12
    pic_send_word( 0x8A12 );
    DelayMs( 1 );
    VPP_RSToff(); //release from reset
    VPP_RUNon();

    DelayMs( 60 );                                      //PIC24E 50ms min
    pic_send_n_bits( 5, 0 );
//    dspic_send_24_bits( 0x000000 );     //NOP           //PIC24E (doc 70633) says 3 nops
//    dspic_send_24_bits( 0x000000 );     //NOP
    dspic_send_24_bits( 0x000000 );     //NOP
    dspic_send_24_bits( 0x040200 );     //GOTO 0x200
    dspic_send_24_bits( 0x000000 );     //NOP
    dspic_send_24_bits( 0x000000 );     //NOP
    dspic_send_24_bits( 0x000000 );     //NOP
}
void enter_ISCP_PIC24E()
{

    enablePGC_D(); //PGC/D output & PGC/D_LOW appropriate

    VPP_RUNoff(); //MCLR low
    VDDon();
    clock_delay();              // P6 100ns    DelayMs( 10 );
    VPP_RUNon(); //VPP to 4.5V
    DelayMs( 1 );               //P21 500us        // PIC24E 500us min
    VPP_RUNoff(); //and immediately back to 0...
    VPP_RSTon();
    DelayMs( 2 );               //P18 = 1ms min        PIC24E 1ms min
    //write 0x4D43, high to low, other than the rest of the commands which are low to high...
    //0x4D43 => 0100 1101 0100 0011
    //from low to high => 1100 0010 1011 0010
    //0xC2B2
    pic_send_word( 0xC2B2 );
    //write 0x4851 => 0100 1000 0101 0001 => 1000 1010 0001 0010 => 0x8A12
    pic_send_word( 0x8A12 );
                                // P19 25ns   DelayMs( 1 );
    VPP_RSToff(); //release from reset
    VPP_RUNon();

    DelayMs( 60 );              // P7 50ms  + P1*5           //PIC24E 50ms min
    pic_send_n_bits( 5, 0 );
    dspic_send_24_bits( 0x000000 );     //NOP           //PIC24E (doc 70633) says 3 nops
    dspic_send_24_bits( 0x000000 );     //NOP
    dspic_send_24_bits( 0x000000 );     //NOP
    dspic_send_24_bits( 0x040200 );     //GOTO 0x200
    dspic_send_24_bits( 0x000000 );     //NOP
    dspic_send_24_bits( 0x000000 );     //NOP
    dspic_send_24_bits( 0x000000 );     //NOP
}
void enter_ISCP_PIC24K()
{
	int i;

	enablePGC_D(); //PGC/D output & PGC/D_LOW appropriate

	VPP_RUNoff(); //MCLR low
	VDDon();
	DelayMs( 10 );
	VPP_RUNon(); //VPP to 4.5V
	for( i = 0; i < 300; i++ )
		continue; //aprox 0.5ms
	//clock_delay();	//P19 = 40ns min
	//write 0x4D43, high to low, other than the rest of the commands which are low to high...
	//0x3D43 => 0100 1101 0100 0011
	//from low to high => 1100 0010 1011 0010
	//0xC2B2
	pic_send_word( 0xC2B2 );
	//write 0x4851 => 0100 1000 0101 0001 => 1000 1010 0001 0010 => 0x0A12
	pic_send_word( 0x8A12 );
	DelayMs( 1 );

	DelayMs( 25 );
	pic_send_n_bits( 5, 0 );
	dspic_send_24_bits( 0 ); //send a nop instruction with 5 additional databits
	dspic_send_24_bits( 0x000000 ); 	//NOP
	dspic_send_24_bits( 0x040200 ); 	//GOTO 0x200
	dspic_send_24_bits( 0x000000 ); 	//NOP
}
void enter_ISCP_I2C_EE()
{
	enablePGC_D(); //PGC/D output & PGC/D_LOW appropriate

	PGDhigh();
	PGChigh();
	clock_delay(); // dummy tempo
	VDDon(); //no VPP needed
	DelayMs( 100 );
}

void exit_ISCP()
{
	iscp_active = 0;
	VPPoff(); //low, (inverted)
	VPP_RUNoff();
	VPP_RSTon(); //hard reset, low (inverted)
	DelayMs( 40 );
	VPP_RSToff(); //hard reset, high (inverted)
	VDDoff(); //low, (inverted)
	disablePGC_D();
	DelayMs( 200 );
}

void set_address_P16( unsigned long address );
void set_address_P18( unsigned long address );
void set_address( PICFAMILY picfamily, unsigned long address )
{
	switch( picfamily ) {
	case PIC18:
	case PIC18J:
	case PIC18K:
		set_address_P18( address );
		break;
	case PIC10:
	case PIC16:
		set_address_P16( address );
	default:
		break;
	}
}
void set_address_P16( unsigned long address )
{
	unsigned long int i;

	for( i = 0; i < address; i++ )
		pic_send_n_bits( 6, 0x06 ); //increment address
}
void set_address_P18( unsigned long address )
{
	pic_send( 4, 0x00, (unsigned int) (0x0E00 | ((address >> 16) & 0xFF)) ); //MOVLW Addr [23:16]
	pic_send( 4, 0x00, 0x6EF8 ); //MOVWF TBLPTRU
	pic_send( 4, 0x00, (unsigned int) (0x0E00 | ((address >> 8) & 0xFF)) ); //MOVLW Addr [15:8]
	pic_send( 4, 0x00, 0x6EF7 ); //MOVWF TBLPTRU
	pic_send( 4, 0x00, (unsigned int) (0x0E00 | ((address) & 0xFF)) ); //MOVLW Addr [7:0]
	pic_send( 4, 0x00, 0x6EF6 ); //MOVWF TBLPTRU
}

#ifndef TEST
/**
 Writes a n-bit command
 **/
void pic_send_n_bits( char cmd_size, char command )
{
	char i;
	//	enablePGD();
	//	enablePGC();
	PGClow();
	PGDlow();
	for( i = 0; i < cmd_size; i++ )
	{
		if( command & 1 )
			PGDhigh();
		else
			PGDlow();
		PGChigh();
		command >>= 1;
		clock_delay();
		PGClow();
		clock_delay();
	}
	for( i = 0; i < 10; i++ )
		continue; //wait at least 1 us <<-- this could be tweaked to get the thing faster
}

void pic_send_word( unsigned int payload )
{
	char i;
	for( i = 0; i < 16; i++ )
	{
		if( payload & 1 )
			PGDhigh();
		else
			PGDlow();
		PGChigh();
		payload >>= 1;
		clock_delay();
		PGClow();
		clock_delay();
	}
	clock_delay();
}

void pic_send_word_14_bits( unsigned int payload )
{
	char i;
	payload = payload <<1;
	payload &= 0x7FFE;

	for( i = 0; i < 16; i++ )
	{
		if( payload & 1 )
			PGDhigh();
		else
			PGDlow();
		PGChigh();
		payload >>= 1;
		clock_delay();
		PGClow();
		clock_delay();

	}
}

/**
 Writes a n-bit command + 16 bit payload to a pic18 device
 **/
void pic_send( char cmd_size, char command, unsigned int payload )
{
	pic_send_n_bits( cmd_size, command );
	pic_send_word( payload );
	PGDlow(); //  <=== Must be low at the end, at least when VPP and VDD go low.

}

/**
 Writes a n-bit command + 14 bit payload to a pic16 device
 **/
void pic_send_14_bits( char cmd_size, char command, unsigned int payload )
{
	pic_send_n_bits( cmd_size, command );
	pic_send_word_14_bits( payload );
	PGDlow(); //  <=== Must be low at the end, at least when VPP and VDD go low.
}

unsigned int pic_read_14_bits( char cmd_size, char command )
{
	char i;
	unsigned int result;
	pic_send_n_bits( cmd_size, command );
	//for(i=0;i<80;i++)continue;	//wait at least 1us
	///PIC10 only...

	setPGDinput(); //PGD = input
	for( i = 0; i < 10; i++ )
		continue;
	result = 0;
	PGChigh();
	clock_delay();
	PGClow();
	clock_delay();
	for( i = 0; i < 14; i++ )
	{

		PGChigh();
		clock_delay();
		result |= ((unsigned int) PGD_READ) << i;
		clock_delay();
		PGClow();
		clock_delay();
	}
	PGChigh();
	clock_delay();
	PGClow();
	clock_delay();
	setPGDoutput();
	clock_delay();
	return result;
}

/**
 reads 8 bits from a pic device with a given cmd_size bits command
 **/
char pic_read_byte2( char cmd_size, char command )
{
	char i;
	char result;
	pic_send_n_bits( cmd_size, command );
	//	for(i=0;i<80;i++)continue;	//wait at least 1us
	for( i = 0; i < 8; i++ )
	{
		PGDlow();
		clock_delay();
		PGChigh();
		clock_delay();
		PGClow();
		clock_delay();
	}
	setPGDinput();
	for( i = 0; i < 10; i++ )
		continue;
	result = 0;
	for( i = 0; i < 8; i++ )
	{

		PGChigh();
		clock_delay();
		result |= ((char) PGD_READ) << i;
		clock_delay();
		PGClow();
		clock_delay();
	}
	setPGDoutput();
	clock_delay();
	return result;
}

/// read a 16 bit "word" from a dsPIC
unsigned int dspic_read_16_bits( unsigned char isLV )
{
	char i;
	unsigned int result;

	PGDlow();
	PGDhigh(); //send 1
	PGChigh(); //clock pulse
	PGClow();
	PGDlow(); //send 3 zeroes
	for( i = 0; i < 3; i++ )
	{
		PGChigh();
		clock_delay();
		PGClow();
		clock_delay();
	}
	//pic_send_n_bits(4,1);
	result = 0;
	for( i = 0; i < 8; i++ )
	{
		PGChigh();
		clock_delay();
		PGClow();
		clock_delay();
	}
	//pic_send_n_bits(8,0);
	setPGDinput();
	clock_delay();
	for( i = 0; i < 16; i++ )
	{
		PGChigh();
		clock_delay();
		result |= ((unsigned int) PGD_READ) << i;
		PGClow();
	}
	setPGDoutput();
	PGDlow();
	return result;
}

void dspic_send_24_bits( unsigned long p )
{
    unsigned char i, b, payload;

    PGDlow();
    for( i = 0; i < 4; i++ )
    {
        PGChigh();
        clock_delay();
        PGClow();
    }
    payload = ((unsigned char *)&p)[0];
    for( i = 0; i < 8; i++ )
    {

        if( payload & 1 )
            PGDhigh();
        else
            PGDlow();
        payload >>= 1;
//        clock_delay();
        PGChigh();
        clock_delay();
        PGClow();
    }
    PGDlow();

    payload = ((unsigned char *)&p)[1];
    for( i = 0; i < 8; i++ )
    {

        if( payload & 1 )
            PGDhigh();
        else
            PGDlow();
        payload >>= 1;
//        clock_delay();
        PGChigh();
        clock_delay();
        PGClow();
    }
    PGDlow();

    payload = ((unsigned char *)&p)[2];
    for( i = 0; i < 8; i++ )
    {

        if( payload & 1 )
            PGDhigh();
        else
            PGDlow();
        payload >>= 1;
//        clock_delay();
        PGChigh();
        clock_delay();
        PGClow();
    }
    PGDlow();

}

void I2C_start( void )
{
	//initial condition
	PGDhigh();
	I2C_delay();
	PGChigh();
	I2C_delay();
	PGDlow();
	I2C_delay();
	PGClow();
	I2C_delay();
}

void I2C_stop( void )
{
	PGDlow();
	I2C_delay();
	PGChigh();
	I2C_delay();
	PGDhigh();
	I2C_delay();
}

unsigned char I2C_write( unsigned char d )
{
	unsigned char i, j;
	j = d;
	for( i = 0; i < 8; i++ )
	{
		if( (j & 0x80) == 0x80 )
			PGDhigh();
		else
			PGDlow();
		j <<= 1;
		I2C_delay();
		PGChigh();
		I2C_delay();
		PGClow();
		I2C_delay();
	}
	setPGDinput();
	PGChigh();
	I2C_delay();
	i = (unsigned char) PGD_READ;
	PGClow();
	I2C_delay();
	setPGDoutput();
	return i;
}

unsigned char I2C_read( unsigned char ack ) {
    unsigned char i, d;
    setPGDinput();
	TRISPGD_LOW = 0;	// going to use the 1K res as a pull-up
	PGD_LOW = 1;

    d = 0;
    for( i = 0; i < 8; i++ ) {
        PGChigh();
        I2C_delay();
        d <<= 1;
        if( PGD_READ )
            d |= 0x01;
        PGClow();
        I2C_delay();
    }
	TRISPGD_LOW = 1;	// remove the pull-up
	PGD_LOW = 0;
    setPGDoutput();
    I2C_delay();
    if( ack == 1 )
        PGDhigh();
        else PGDlow();
    PGChigh();
    I2C_delay();
    PGClow();
    I2C_delay();
    return d;
}


/*#define pulseclock() PGChigh();PGClow()

 unsigned char jtag2w4p( unsigned char TDI, unsigned char TMS, unsigned char nbits ) {
 unsigned char i;
 unsigned char res = 0;
 unsigned char orval = 1 << (nbits - 1);
 for( i = 0; i < nbits; i++ ) {
 res >>= 1;
 if( TDI & 1 )
 PGDhigh();
 elsePGDlow();
 pulseclock();
 if( TMS & 1 )
 PGDhigh();
 elsePGDlow();
 pulseclock();
 trisPGD();
 pulseclock();
 PGChigh();
 if( PGD_READ )
 res |= orval;
 PGClow();
 enablePGD();
 TDI >>= 1;
 TMS >>= 1;

 }
 return res;
 }

 XferFastData() {
 //TMS header 100
 unsigned char Ack;
 Ack = jtag2w5p( 0, 0b001, 3 );
 ///TODO: check Ack


 }
 */

#endif
