/*********************************************************************
 *
 *                UsbPicProg v0.1
 *                Frans Schreuder 16-06-2007
 ********************************************************************/
#ifdef SDCC
#include <pic18f2550.h>
#else
#include <p18cxxx.h>
#endif

#include "system\typedefs.h"

#include "system\usb\usb.h"

#include "io_cfg.h"             // I/O pin mapping
#include "upp\upp.h"
#include <string.h>

/*#ifndef SDCC
#pragma udata DECLARE_OVER_PORTC=0xF82
struct _LEDPORT{
	unsigned Leds:3;
	unsigned unused:5;
}Ledport;

#pragma code
#endif
*/
/** V A R I A B L E S ********************************************************/
#ifndef SDCC
#pragma udata
#endif
byte old_sw2,old_sw3;

//DATA_PACKET dataPacket;

byte input_buffer[64];
byte output_buffer[64];

rom char welcome[]={"UsbPicProg - Open Source USB PIC programmer\r\n\r\n"};
rom char ansi_clrscr[]={"\x1b[2J"};         // ANSI Clear Screen Command

/** P R I V A T E  P R O T O T Y P E S ***************************************/
void BlinkUSBStatus(void);
BOOL Switch2IsPressed(void);
BOOL Switch3IsPressed(void);
void setLeds(char n);


/** D E C L A R A T I O N S **************************************************/
#ifndef SDCC
#pragma code
#endif
void UserInit(void)
{
    mInitAllLEDs();
    mInitAllSwitches();
    old_sw2 = sw2;
    Pump1tris = 0;
    Pump2tris = 0;
    
	TRISVPP = 0; //output
	TRISVPP_RST=0; //output
	TRISPGD=0;    
	TRISPGC=0;    
	TRISVDD=0;
    VPP = 1; //VPP is low (inverted)
	VPP_RST=0; //No hard reset (inverted
	PGD=0;		
	PGC=0;
	INTCON2bits.RBPU=1; //disable Portb pullup resistors
    timer1Init();
}//end UserInit

void timer1Init(void)
{
	INTCON=0xC0; //global and periferal interrupts enabled
	PIE1bits.TMR1IE=1;//enable timer1 interrupt
	PIR1bits.TMR1IF=0;//clear interrupt flag
	IPR1bits.TMR1IP=1;//high priority interrupt
	TMR1H=TMR1H_PRESET;
	TMR1L=TMR1L_PRESET;
	T1CON=0x01; //timer1 on, prescaler 1;1, internal clock
}

void setLeds(char n)
{
	mLED_1=~(n&1);
	mLED_2=~((n&2)>>1);
	mLED_3=~((n&4)>>2);
}
	
/******************************************************************************
 * Function: 		void VoltagePump(void)
 * PreCondition:	None
 * Input:			None
 * Output:			None
 * Side Effects:	None
 * Overview:		This function has to be called every .. us and will 
 
******************************************************************************/

/*void VoltagePump(void)
{
	static char pumpcount=0;
    if(pumpcount == 0)
    {
	    pumpcount = 2;
	    Pump1=!Pump1;
	    Pump2=!Pump1;
	    //Pump3=Pump1;
	}
    pumpcount--;
	
}
*/	



/******************************************************************************
 * Function:        void ProcessIO(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is a place holder for other user routines.
 *                  It is a mixture of both USB and non-USB tasks.
 *
 * Note:            None
 *****************************************************************************/

void ProcessIO(void)
{
    char oldPGDtris;
    char PIN;
    static byte counter=0;
	byte UsbData[100];
    
	BlinkUSBStatus();
	/*mLED_3_On();
	mLED_2_Toggle();
	mLED_1_Off();
	*/
    // User Application USB tasks
    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	//VoltagePump();
   
	if(USBGenRead((byte*)UsbData,1)>0)
   	{
		mLED_3_Toggle();
		if (UsbData[0]==0x10)
		{
			UsbData[0]=1;
			UsbData[1]=2;
			USBGenWrite(UsbData,2);
		}
	}
	if(counter != 0)
    {
       if(!mUSBGenTxIsBusy())
	       USBGenWrite((byte*)&output_buffer,counter);
    }//end if
}//end ProcessIO



/******************************************************************************
 * Function:        void BlinkUSBStatus(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        BlinkUSBStatus turns on and off LEDs corresponding to
 *                  the USB device state.
 *
 * Note:            mLED macros can be found in io_cfg.h
 *                  usb_device_state is declared in usbmmap.c and is modified
 *                  in usbdrv.c, usbctrltrf.c, and usb9.c
 *****************************************************************************/
void BlinkUSBStatus(void)
{
	static word led_count=0;

    

	if(led_count == 0)led_count = 10000U;

	led_count--;



#define mLED_Both_Off()         {mLED_1_Off();mLED_2_Off();}

#define mLED_Both_On()          {mLED_1_On();mLED_2_On();}

#define mLED_Only_1_On()        {mLED_1_On();mLED_2_Off();}

#define mLED_Only_2_On()        {mLED_1_Off();mLED_2_On();}



	if(UCONbits.SUSPND == 1)

	{

		if(led_count==0)

		{

			mLED_1_Toggle();

			mLED_2 = mLED_1;        // Both blink at the same time

		}//end if

	}

	else

	{

		if(usb_device_state == DETACHED_STATE)

		{

			mLED_Both_Off();

		}

		else if(usb_device_state == ATTACHED_STATE)

		{

			mLED_Both_On();

		}

		else if(usb_device_state == POWERED_STATE)

		{

			mLED_Only_1_On();

		}

		else if(usb_device_state == DEFAULT_STATE)

		{

			mLED_Only_2_On();

		}

		else if(usb_device_state == ADDRESS_STATE)

		{

			if(led_count == 0)

			{

				mLED_1_Toggle();

				mLED_2_Off();

			}//end if

		}

		else if(usb_device_state == CONFIGURED_STATE)

		{

			if(led_count==0)

			{

				mLED_1_Toggle();

				mLED_2 = !mLED_1;       // Alternate blink                

			}//end if

		}//end if(...)

	}//end if(UCONbits.SUSPND...)
}//end BlinkUSBStatus

BOOL Switch2IsPressed(void)
{
    if(sw2 != old_sw2)
    {
        old_sw2 = sw2;                  // Save new value
        if(sw2 == 0)                    // If pressed
            return TRUE;                // Was pressed
    }//end if
    return FALSE;                       // Was not pressed
}//end Switch2IsPressed



/** EOF upp.c ***************************************************************/
