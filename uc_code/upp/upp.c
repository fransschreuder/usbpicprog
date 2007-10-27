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
    
}//end UserInit

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

void VoltagePump(void)
{
	static char pumpcount=0;
    if(pumpcount == 0)
    {
	    pumpcount = 2;
	    Pump1=!Pump1;
		Pump2=!Pump1;
	}
    pumpcount--;
	
}
	



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
    BlinkUSBStatus();
    // User Application USB tasks
    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
	VoltagePump();
   
    if(USBGenRead((byte*)input_buffer,1)>0)
   	{
       	if((input_buffer[0]&0xC0)==0x80) //last bit indicates write
        {
		PIN=(input_buffer[0]&0x0F);
		switch(PIN)
		{
			case 0: //~VPP
				if((input_buffer[0]&0x10)==0x10)VPP=0;
				else VPP=1;
				//setLeds(0);
				break;
			case 1: //~VPP_RST
				if((input_buffer[0]&0x10)==0x10)VPP_RST=0;
				else VPP_RST=1;
				//setLeds(1);
				break;			
			case 2: //PGD_OUT
				if((input_buffer[0]&0x10)==0x10)PGD=1;
				else PGD=0;
				//setLeds(2);
				break;	
			case 3: //PGD_IN
				//setLeds(3);
				break;
			case 4: //PGC
				if((input_buffer[0]&0x10)==0x10)PGC=1;
				else PGC=0;
				//setLeds(4);
				break;
			case 5: //GND
				//setLeds(5);
				break;
			case 6: //VDD
				if((input_buffer[0]&0x10)==0x10)VDD=0;
				else VDD=1;
				//setLeds(6);
				break;
			case 7: //TRIS
				if((input_buffer[0]&0x10)==0x10)TRISPGD=0;
				else TRISPGD=1;
				//setLeds(7);
				break;		
			default:
				break;
				
		}
		setLeds(input_buffer[0]&0x07);
		
	    }
        else if ((input_buffer[0]&0xC0)==0xC0) //last two bits indicate read of PGD
		{
			if(TRISPGD)
			{
				if (PGD_READ)output_buffer[0]=0xC1;
				else output_buffer[0]=0xC0;
			}
			else
			{
				if (PGD)output_buffer[0]=0xC1;
				else output_buffer[0]=0xC0;
			}
			counter=1;
			
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
    static char startup_state=0xFF;
    
    if(led_count == 0)led_count = 10000U;
    led_count--;

    if(UCONbits.SUSPND == 1)
    {
	    USBWakeFromSuspend();
        if(led_count==0)
        {
            mLED_1_Toggle();
            mLED_2_Off();
            mLED_3_Off();
        }//end if
    }
    else
    {
        if(usb_device_state == DETACHED_STATE)
        {
            setLeds(1);
        }
        else if(usb_device_state == ATTACHED_STATE)
        {
            setLeds(2);
        }
        else if(usb_device_state == POWERED_STATE)
        {
            setLeds(3);
        }
        else if(usb_device_state == DEFAULT_STATE)
        {
            setLeds(4);
        }
        else if(usb_device_state == ADDRESS_STATE)
        {
            setLeds(5);
        }
        else if(usb_device_state == CONFIGURED_STATE)
        {
	        startup_state=0;
			//			setLeds(7);
			//Do nothing with the leds, just leave it to the rest of the program!
        }//end if(...)
    }//end if(UCONbits.SUSPND...)
	/*if(!mUSBGenTxIsBusy()&&startup_state==0)
	{
		strcpypgm2ram(output_buffer,ansi_clrscr);
		USBGenWrite(output_buffer,strlen(output_buffer));
		startup_state++;
	}
	if(!mUSBGenTxIsBusy()&&startup_state==1)
	{
		strcpypgm2ram(output_buffer,welcome);
		USBGenWrite(output_buffer,strlen(output_buffer));
		startup_state++;
	}*/	
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
