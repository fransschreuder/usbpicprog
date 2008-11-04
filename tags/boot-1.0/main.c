/*********************************************************************
 *
 *                Microchip USB Bootloader Version 1.0
 *
 *********************************************************************
 * FileName:        main.c
 * Dependencies:    See INCLUDES section below
 * Processor:       PIC18
 * Compiler:        C18 2.30.01+
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the �Company�) for its PICmicro� Microcontroller is intended and
 * supplied to you, the Company�s customer, for use solely and
 * exclusively on Microchip PICmicro Microcontroller products. The
 * software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN �AS IS� CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Rawin Rojvanit       11/19/04     Original.
 ********************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "typedefs.h"                        // Required
#include "usb.h"                         // Required
#include "io_cfg.h"                                 // Required

#include "usb_compile_time_validation.h" // Optional

/** V A R I A B L E S ********************************************************/
#pragma udata

/** P R I V A T E  P R O T O T Y P E S ***************************************/

/** V E C T O R  R E M A P P I N G *******************************************/

#pragma code _HIGH_INTERRUPT_VECTOR = 0x000008
void _high_ISR (void)
{
    _asm goto RM_HIGH_INTERRUPT_VECTOR _endasm
}

#pragma code _LOW_INTERRUPT_VECTOR = 0x000018
void _low_ISR (void)
{
    _asm goto RM_LOW_INTERRUPT_VECTOR _endasm
}

#pragma code

/** D E C L A R A T I O N S **************************************************/
#pragma code
/******************************************************************************
 * Function:        void main(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Main program entry point.
 *
 * Note:            None
 *****************************************************************************/
void main(void)
{
	static byte i;
	
	// initial settings
	TRISA = 0xFF;
	TRISB = 0xFF;
	TRISC = 0xF8;
	PORTA = 0x00;
	PORTB = 0x04;
	PORTC = 0x00;
	T0CON = 0x00;
	T1CON = 0x00;
	T2CON = 0x00;
	T3CON = 0x00;
	CMCON = 0x07;
	CCP1CON = 0x00;
	CCP2CON = 0x00;
	INTCON2 = 0x00;

        if (!PORTBbits.RB6)
	{
		_asm goto RM_RESET_VECTOR _endasm
	}

    // initialize usb
    mInitializeUSBDriver();     // See usbdrv.h
    USBCheckBusStatus();        // Modified to always enable USB module
    
    LED_0 = 1;
    LED_1 = 1;
    LED_2 = 1;
    

	
	// led 0 on 
    LED_0 = 0;
    
    // init i and j
    i = DETACHED_STATE;
    while(1)
    {
	    // do usb things
        USBDriverService();     // See usbdrv.c
        BootService();          // See boot.c
        
        // set i to current usb_device state
        if (usb_device_state != i && i != CONFIGURED_STATE)
        {
	    	i = usb_device_state;
	    	
	    	if (usb_device_state == CONFIGURED_STATE)
	    	{
		    	// set led 1 when connected to usb
		    	LED_1 = 0;
		    }
	    }
    }//end while
}//end main

#pragma code user = RM_RESET_VECTOR

/** EOF main.c ***************************************************************/
