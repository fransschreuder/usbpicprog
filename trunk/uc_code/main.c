/*********************************************************************
 *
 *                Microchip USB C18 Firmware Version 1.0
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
 * Rawin Rojvanit       11/19/04    Original.
 ********************************************************************/

/** I N C L U D E S **********************************************************/
#include "system/interrupt/interrupt.h"
#ifdef SDCC
#include <pic18f2550.h>
#else
#include <p18cxxx.h>
#endif
#include "system/typedefs.h"                        // Required
#include "system/usb/usb.h"                         // Required
#include "io_cfg.h"                                 // Required

#include "system/usb/usb_compile_time_validation.h" // Optional
#include "upp/upp.h"                              // Modifiable

#ifndef SDCC_MODEL_SMALL
#pragma config PLLDIV = 2, CPUDIV = OSC1_PLL2, USBDIV = 2, FOSC = HSPLL_HS //CPU=48 MHz
#pragma config PWRT = ON
#pragma config BOR = OFF
#pragma config VREGEN = ON 	// USB Voltage Regulator Enable: ein
#pragma config WDT = OFF 	// Watchdog Timer: aus
#pragma config MCLRE = ON // MCLR Enable:
//#pragma config MCLRE = OFF 	// MCLR Disabled    RE3 enabled
#pragma config PBADEN = OFF // PORTB<4:0> pins are configured as digital I/O on Reset
#pragma config LVP = OFF 	// Low Voltage ICSP: aus
#pragma config CPB = OFF	//ON	// boot block code protection: on
#pragma config WRTB = OFF	//ON// boot block write protection: on
#pragma config CCP2MX = OFF
#pragma config STVREN = OFF
#else
code char at __CONFIG1L CONFIG1L = _PLLDIV_DIVIDE_BY_2__8MHZ_INPUT__1L & _CPUDIV__OSC1_OSC2_SRC___1__96MHZ_PLL_SRC___2__1L & _USBPLL_CLOCK_SRC_FROM_96MHZ_PLL_2_1L;
code char at __CONFIG1H CONFIG1H = _OSC_HS__HS_PLL__USB_HS_1H & _FCMEN_OFF_1H & _IESO_OFF_1H;
code char at __CONFIG2L CONFIG2L = _PUT_ON_2L & _BODEN_OFF_2L & _BODENV_2_0V_2L & _VREGEN_ON_2L;
code char at __CONFIG2H CONFIG2H = _WDT_DISABLED_CONTROLLED_2H & _WDTPS_1_32768_2H;
code char at __CONFIG3H CONFIG3H = _CCP2MUX_RC1_3H & _PBADEN_PORTB_4_0__CONFIGURED_AS_DIGITAL_I_O_ON_RESET_3H & _LPT1OSC_OFF_3H & _MCLRE_MCLR_ON_RE3_OFF_3H;
code char at __CONFIG4L CONFIG4L = _STVR_ON_4L & _LVP_OFF_4L & _ENICPORT_OFF_4L & _ENHCPU_OFF_4L & _BACKBUG_OFF_4L;
code char at __CONFIG5L CONFIG5L = _CP_0_OFF_5L & _CP_1_OFF_5L & _CP_2_OFF_5L & _CP_3_OFF_5L;
code char at __CONFIG5H CONFIG5H = _CPB_OFF_5H;
code char at __CONFIG6L CONFIG6L = _WRT_0_OFF_6L & _WRT_1_OFF_6L & _WRT_2_OFF_6L & _WRT_3_OFF_6L;
code char at __CONFIG6H CONFIG6H = _WRTC_OFF_6H & _WRTB_OFF_6H;
code char at __CONFIG7L CONFIG7L = _EBTR_0_OFF_7L & _EBTR_1_OFF_7L & _EBTR_2_OFF_7L & _EBTR_3_OFF_7L;
code char at __CONFIG7H CONFIG7H = _EBTRB_OFF_7H;
#endif

/** V A R I A B L E S ********************************************************/
#ifndef SDCC
#pragma udata
#endif

/** P R I V A T E  P R O T O T Y P E S ***************************************/
static void InitializeSystem(void);
void USBTasks(void);

/** V E C T O R  R E M A P P I N G *******************************************/
#ifndef SDCC
extern void _startup (void);        // See c018i.c in your C18 compiler dir
#pragma code _RESET_INTERRUPT_VECTOR = 0x000800
void _reset (void)
{
    _asm goto _startup _endasm
}
#pragma code

//#ifdef bootloader
#pragma code _HIGH_INTERRUPT_VECTOR = 0x000808
//#else
//#pragma code _HIGH_INTERRUPT_VECTOR = 0x000008
//#endif
void _high_ISR (void)
{
	_asm goto high_isr _endasm
}


//#ifdef bootloader
#pragma code _LOW_INTERRUPT_VECTOR = 0x000818
//#else
//#pragma code _LOW_INTERRUPT_VECTOR = 0x000018
//#endif

void _low_ISR (void)
{
	_asm goto low_isr _endasm
}
#pragma code
#endif

extern byte usb_device_state;

/******************************************************************************
This function resets the usb module to bring it in the same state as after a
power on reset (because it may have been altered by the bootloader)
******************************************************************************/
void usb_reset(void)
{
     UEP15=0;UEP14=0;UEP13=0;UEP12=0;UEP11=0;UEP10=0;UEP9=0;UEP8=0;
     UEP7=0;UEP6=0;UEP5=0;UEP4=0;UEP3=0;UEP2=0;UEP1=0;UEP0=0;
     UCFG=0;
     UADDR=0;
     UCON=0;
     UEIE=0;
     UEIR=0;
     UIE=0;
     UIR=0;
}

/** D E C L A R A T I O N S **************************************************/
#ifndef SDCC
#pragma code
#endif
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
	//setLeds(7);
	//while(1)continue;
	//USBProtocolResetHandler();
    //usb_reset();
    InitializeSystem();
    
    //usb_device_state = DETACHED_STATE;    //if the bootloader has initialized the USB-bus, this will disable it again
    while(1)
    {
        USBTasks();         // USB Tasks
        ProcessIO();        // See user\user.c & .h
    }//end while
}//end main

/******************************************************************************
 * Function:        static void InitializeSystem(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        InitializeSystem is a centralize initialization routine.
 *                  All required USB initialization routines are called from
 *                  here.
 *
 *                  User application initialization routine should also be
 *                  called from here.                  
 *
 * Note:            None
 *****************************************************************************/
static void InitializeSystem(void)
{
    ADCON1 |= 0x0F;                 // Default all pins to digital
    
    #if defined(USE_USB_BUS_SENSE_IO)
    tris_usb_bus_sense = INPUT_PIN; // See io_cfg.h
    #endif
    
    #if defined(USE_SELF_POWER_SENSE_IO)
    tris_self_power = INPUT_PIN;
    #endif
    
    mInitializeUSBDriver();         // See usbdrv.h
    
    UserInit();                     // See upp.c & .h

}//end InitializeSystem

/******************************************************************************
 * Function:        void USBTasks(void)
 *
 * PreCondition:    InitializeSystem has been called.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Service loop for USB tasks.
 *
 * Note:            None
 *****************************************************************************/
void USBTasks(void)
{
    /*
     * Servicing Hardware
     */
    USBCheckBusStatus();                    // Must use polling method
    if(UCFGbits.UTEYE!=1)
        USBDriverService();                 // Interrupt or polling method

}// end USBTasks

/** EOF main.c ***************************************************************/
