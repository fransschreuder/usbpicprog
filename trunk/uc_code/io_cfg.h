/*********************************************************************
 *
 *                Microchip USB C18 Firmware Version 1.0
 *
 *********************************************************************
 * FileName:        io_cfg.h
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
 * Rawin Rojvanit       7/21/04     Original.
 ********************************************************************/

#ifndef IO_CFG_H
#define IO_CFG_H

/** I N C L U D E S *************************************************/
#include "usbcfg.h"

/** T R I S *********************************************************/
#define INPUT_PIN           1
#define OUTPUT_PIN          0

/** U S B ***********************************************************/
/*#define tris_usb_bus_sense  TRISAbits.TRISA1    // Input

#if defined(USE_USB_BUS_SENSE_IO)
#define usb_bus_sense       PORTAbits.RA1
#else
#define usb_bus_sense       1
#endif
  */
/*#define tris_self_power     TRISAbits.TRISA2    // Input

#if defined(USE_SELF_POWER_SENSE_IO)
#define self_power          PORTAbits.RA2
#else
#define self_power          1
#endif    */

/** L E D ***********************************************************/
#define mInitAllLEDs()      LATC |= 0x07; TRISC &= 0xF8;

#define mLED_1              LATCbits.LATC0
#define mLED_2              LATCbits.LATC1
#define mLED_3              LATCbits.LATC2


#define mLED_1_On()         mLED_1 = 0;
#define mLED_2_On()         mLED_2 = 0;
#define mLED_3_On()         mLED_3 = 0;


#define mLED_1_Off()        mLED_1 = 1;
#define mLED_2_Off()        mLED_2 = 1;
#define mLED_3_Off()        mLED_3 = 1;

#define mLED_1_Toggle()     mLED_1 = !mLED_1;
#define mLED_2_Toggle()     mLED_2 = !mLED_2;
#define mLED_3_Toggle()     mLED_3 = !mLED_3;



/** S W I T C H *****************************************************/
#define mInitAllSwitches()  TRISBbits.TRISB6=1;
#define mInitSwitch2()      TRISBbits.TRISB6=1;
#define sw2                 PORTBbits.RB6

/** Programmer pins *************************************************/
#define VPP LATBbits.LATB3
#define TRISVPP TRISBbits.TRISB3
#define VPP_RST LATBbits.LATB5
#define TRISVPP_RST TRISBbits.TRISB5
#define PGD LATBbits.LATB0
#define TRISPGD TRISBbits.TRISB0
#define PGD_READ PORTBbits.RB0
#define PGC LATBbits.LATB1
#define TRISPGC TRISBbits.TRISB1
#define VDD LATBbits.LATB2
#define TRISVDD TRISBbits.TRISB2
#define VPP_RUN LATAbits.LATA3
#define TRISVPP_RUN TRISAbits.TRISA3
#define PGD_LOW LATAbits.LATA4
#define TRISPGD_LOW TRISAbits.TRISA4
#define PGC_LOW LATAbits.LATA5
#define TRISPGC_LOW TRISAbits.TRISA5

/** Manipulate Programmer pins **************************************/
#define setPGDinput()	do { TRISPGD=1;TRISPGD_LOW=1; } while(0)
#define setPGDoutput()	do { if( is3_3V() ) TRISPGD_LOW=0; TRISPGD=0;} while(0)
#define enablePGC_D()	do { if( is3_3V() ) { TRISPGC_LOW=0; TRISPGD_LOW=0; } TRISPGC=0;TRISPGD=0;} while(0)
#define disablePGC_D()	do { TRISPGC_LOW=1; TRISPGD_LOW=1; TRISPGC=1;TRISPGD=1;} while(0)
//#define enablePGC_LOW()	TRISPGC_LOW=0
//#define trisPGC_LOW()	TRISPGC_LOW=1
//#define PGC_LOWoff()	PGC_LOW=1
//#define PGC_LOWon()	PGC_LOW=0
//#define enablePGD_LOW()	TRISPGD_LOW=0
//#define trisPGD_LOW()	TRISPGD_LOW=1
//#define PGD_LOWoff()	PGD_LOW=1
//#define PGD_LOWon()	PGD_LOW=0

#define enableI2CPullup do { TRISPGD_LOW=0; PGD_LOW=1; } while(0)
#define disableI2CPullup do { TRISPGD_LOW=1; PGD_LOW=0; } while(0)

#define enableVPP_RUN()	TRISVPP_RUN=0		//FIXME: should rename VPP and VPP_RUN - perhaps VPP_HI, VPP_5V
#define trisVPP_RUN()	TRISVPP_RUN=1
#define VPP_RUNoff()	VPP_RUN=0
#define VPP_RUNon()	VPP_RUN=1
#define enableVDD()	TRISVDD=0
#define trisVDD()	TRISVDD=1
#define VDDoff()	VDD=1
#define VDDon()		if(ConfigDisableVDD==0)VDD=0
#define enablePGC()	TRISPGC=0
//#define trisPGC()	TRISPGC=1
#define PGClow()	PGC=0
#define PGChigh()	PGC=1
//#define enablePGD()	TRISPGD=0
//#define trisPGD()	TRISPGD=1
#define PGDlow()	PGD=0
#define PGDhigh()	PGD=1
#define enableVPP_RST()	TRISVPP_RST=0
#define trisVPP_RST()	TRISVPP_RST=1
#define VPP_RSToff()	VPP_RST=0
#define VPP_RSTon()	VPP_RST=1
#define enableVPP()	TRISVPP=0
#define trisVPP()	TRISVPP=1
#define VPPoff()	{if(ConfigLimitVPP==0)VPP=1;else{VPP_RUNoff();}}
#define VPPon()		{if(ConfigLimitVPP==0)VPP=0;else{VPP_RUNon();}}

/** Voltage pump ****************************************************/
#define Pump1 LATAbits.LATA0
#define Pump2 LATAbits.LATA1
#define Pump1tris TRISAbits.TRISA0
#define Pump2tris TRISAbits.TRISA1
#define TMR1_PERIOD 1000 
#define CLOCK_FREQ 48
#define TMR1_PRESET (65535-((CLOCK_FREQ/4)*TMR1_PERIOD))
#define TMR1H_PRESET ((TMR1_PRESET & 0xFF00)>>8)
#define TMR1L_PRESET (TMR1_PRESET & 0x00FF)

#define TMR0_PERIOD 250
//TMR0 has a 1:16 prescaler
#define TMR0L_PRESET (255-((CLOCK_FREQ*TMR0_PERIOD)/(4*16))


//volatile struct __PORTC_LEDS_t __at (0xF82) PORTCleds;



/** P O T ***********************************************************/
//#define mInitPOT()          TRISAbits.TRISA0=1;ADCON0=0x01;ADCON2=0x3C;

/** S P I : Chip Select Lines ***************************************/
/*#define tris_cs_temp_sensor TRISBbits.TRISB2    // Output
#define cs_temp_sensor      LATBbits.LATB2

#define tris_cs_sdmmc       TRISBbits.TRISB3    // Output
#define cs_sdmmc            LATBbits.LATB3
*/
/** S D M M C *******************************************************/
/*
#define TRIS_CARD_DETECT    TRISBbits.TRISB4    // Input
#define CARD_DETECT         PORTBbits.RB4

#define TRIS_WRITE_DETECT   TRISAbits.TRISA4    // Input
#define WRITE_DETECT        PORTAbits.RA4
*/
#endif //IO_CFG_H
