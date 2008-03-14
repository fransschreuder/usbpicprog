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
#include "prog.h"

/** V A R I A B L E S ********************************************************/
#ifndef SDCC
#pragma udata
#endif
byte old_sw2,old_sw3;

byte input_buffer[USBGEN_EP_SIZE];
byte output_buffer[USBGEN_EP_SIZE];

rom char upp_version[]={"UsbPicProg v0.1\r\n"};
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

extern unsigned long tick, lasttick;
//unsigned int count_number_of_blocks=0; //for test use only, normally this is done by the PC
unsigned int input_buffer_offset=0;
char sentNextBlockRequest=0;
PICTYPE pictype=PIC18;
PICVARIANT picvariant=P18F2XXX;

void ProcessIO(void)
{
    char oldPGDtris;
    char PIN;
    static byte counter=0;
    int nBytes;
    unsigned long address;
    unsigned int intaddress;

    BlinkUSBStatus();
    // User Application USB tasks
    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;

    
	nBytes=USBGenRead((byte*)input_buffer,64);
	if(nBytes>0)
	{
		if((input_buffer[0])==0x10) 
		{
		output_buffer[0]=bulk_erase(pictype,picvariant);
		counter=1;
		//setLeds(0x07);
		}
		if((input_buffer[0])==0x20) 
		{
			read_code(pictype,picvariant,0x3FFFFE,(char*)output_buffer,2,3);  //devid is at location 3ffffe
			counter=2;
		}
		if((input_buffer[0])==0x30) 
		{
			address=((unsigned long)input_buffer[2])<<16|
					((unsigned long)input_buffer[3])<<8|
					((unsigned long)input_buffer[4]);
			output_buffer[0]=write_code(pictype,picvariant,address, (char*)(input_buffer+6),input_buffer[1],input_buffer[5]);
			counter=1;
		}
		if((input_buffer[0])==0x40) 
		{
				address=((unsigned long)input_buffer[2])<<16|
						((unsigned long)input_buffer[3])<<8|
						((unsigned long)input_buffer[4]);
				
				read_code(pictype,picvariant,address,(char*)output_buffer,input_buffer[1],input_buffer[5]);
				counter=input_buffer[1];
		}
		if((input_buffer[0])==0x50) 
		{
			intaddress=((unsigned int)input_buffer[2])<<8|
					((unsigned int)input_buffer[3]);
	
			output_buffer[0]=write_data(pictype,picvariant,intaddress, (char*)(input_buffer+5),input_buffer[1],input_buffer[4]); 
			counter=1;
		}
		if((input_buffer[0])==0x60) 
		{
			intaddress=((unsigned int)input_buffer[2])<<8|
					((unsigned int)input_buffer[3]);
			read_data(pictype,picvariant,intaddress,(char*)output_buffer,input_buffer[1],input_buffer[4]); 
			counter=input_buffer[1];
		}
		if((input_buffer[0])==0x70)
		{
			address=((unsigned long)input_buffer[2])<<16|
					((unsigned long)input_buffer[3])<<8|
					((unsigned long)input_buffer[4]);
			output_buffer[0]=write_config_bits(pictype, picvariant, address, (char*)(input_buffer+6),input_buffer[1],input_buffer[5]);
			counter=1;
		}
		if((input_buffer[0])==0x80)
		{
			set_pictype(input_buffer+1);
		}
		if((input_buffer[0])==0x90)
		{
			strcpypgm2ram((char*)output_buffer,(const far rom char*)upp_version);
			counter=18;
		}
		
	}
    if(counter != 0)
    {
       if(!mUSBGenTxIsBusy())
	       USBGenWrite((byte*)&output_buffer,counter);
       counter=0;
    }//end if
}//end ProcessIO


void set_pictype(unsigned char* data)
{
	switch(data[0])
	{
		case 0:	pictype=PIC16;	break;
		case 1:	pictype=PIC18;	break;
		default:pictype=PIC18;	break;
	}
	switch(data[1])
	{
		case 0:	picvariant=P18F2XXX;break;
		case 1:	picvariant=P18FXX2; break;
		case 2:	picvariant=P16F87XA;break;
		case 3:	picvariant=P16F62XA;break;
		default: picvariant=P18F2XXX;break;
	}	
}



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
	    //USBWakeFromSuspend();
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
