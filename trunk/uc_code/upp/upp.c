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
#include "svn_revision.h"
/** V A R I A B L E S ********************************************************/
#ifndef SDCC
#pragma udata
#endif
byte old_sw2,old_sw3;
#pragma udata BUFFERS=0x100

byte input_buffer[USBGEN_EP_SIZE];
byte output_buffer[USBGEN_EP_SIZE];
#pragma udata

rom char upp_version[]={SVN_REVISION};
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
    timer0Init();
}//end UserInit

void timer1Init(void)
{
	INTCON|=0xC0; //global and periferal interrupts enabled
	PIE1bits.TMR1IE=1;//enable timer1 interrupt
	PIR1bits.TMR1IF=0;//clear interrupt flag
	IPR1bits.TMR1IP=1;//high priority interrupt
	TMR1H=TMR1H_PRESET;
	TMR1L=TMR1L_PRESET;
	T1CON=0x01; //timer1 on, prescaler 1;1, internal clock
}

void timer0Init(void)
{
	INTCON|=0xE0; //global, periferal and t0ie bits enabled
	INTCONbits.TMR0IF=0; //clear interrupt flag
	TMR0L=68;//TMR0L_PRESET;
	T0CON=0xC3; //prescaler div by 1:16
}

void setLeds(char n)
{
	mLED_1=~(n&1);
	mLED_2=~((n&2)>>1);
	mLED_3=~((n&4)>>2);
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

extern unsigned long tick, lasttick;
unsigned int input_buffer_offset=0;
PICFAMILY picfamily=PIC18;
PICTYPE pictype=P18F2XXX;

void ProcessIO(void)
{
    char oldPGDtris;
    char PIN;
    static byte counter=0;
    int nBytes;
    unsigned long address;
    unsigned int intaddress;

    // When the device is plugged in, the leds give the numbers 1, 2, 3, 4, 5. 
    //After configured state, the leds are controlled by the next lines in this function
    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1))
    {
		BlinkUSBStatus();
    	return;
    }

    
	nBytes=USBGenRead((byte*)input_buffer,64);
	if(nBytes>0)
	{
		if((input_buffer[0])==0x10)  //CMD_BULK_ERASE
		{
			setLeds(0x01);
			output_buffer[0]=bulk_erase(picfamily,pictype);
			counter=1;
			setLeds(0x05);
		}
		if((input_buffer[0])==0x20) //CMD_GET_ID
		{
			if(picfamily==PIC18)
				read_code(picfamily,pictype,0x3FFFFE,(unsigned char*)output_buffer,2,3);  //devid is at location 0x3ffffe   for PIC18 devices
			else
				read_code(picfamily,pictype,0x2006,(unsigned char*)output_buffer,2,3);  //devid is at location 0x2006  for PIC16 devices
			counter=2;
			setLeds(0x05);
		}
		if((input_buffer[0])==0x30) //CMD_WRITE_CODE
		{
			address=((unsigned long)input_buffer[2])<<16|
					((unsigned long)input_buffer[3])<<8|
					((unsigned long)input_buffer[4]);
			output_buffer[0]=write_code(picfamily,pictype,address, (unsigned char*)(input_buffer+6),input_buffer[1],input_buffer[5]);
			counter=1;
			setLeds(0x05);
		}
		if((input_buffer[0])==0x40) //CMD_READ_CODE
		{
			address=((unsigned long)input_buffer[2])<<16|
					((unsigned long)input_buffer[3])<<8|
					((unsigned long)input_buffer[4]);

			read_code(picfamily,pictype,address,(unsigned char*)output_buffer,input_buffer[1],input_buffer[5]);
			counter=input_buffer[1];
			setLeds(0x05);
		}
		if((input_buffer[0])==0x50) //CMD_WRITE_DATA
		{
			intaddress=((unsigned int)input_buffer[3])<<8|
					((unsigned int)input_buffer[4]);
	
			output_buffer[0]=write_data(picfamily,pictype,intaddress, (unsigned char*)(input_buffer+6),input_buffer[1],input_buffer[5]); 
			counter=1;
			setLeds(0x05);
		}
		if((input_buffer[0])==0x60) //CMD_READ_DATA
		{
			intaddress=((unsigned int)input_buffer[3])<<8|
					((unsigned int)input_buffer[4]);
			read_data(picfamily,pictype,intaddress,(unsigned char*)output_buffer,input_buffer[1],input_buffer[5]); 
			counter=input_buffer[1];
			setLeds(0x05);
		}
		if((input_buffer[0])==0x70) //CMD_WRITE_CONFIG
		{
			address=((unsigned long)input_buffer[2])<<16|
					((unsigned long)input_buffer[3])<<8|
					((unsigned long)input_buffer[4]);
			output_buffer[0]=write_config_bits(picfamily, pictype, address, (unsigned char*)(input_buffer+6),input_buffer[1],input_buffer[5]);
			counter=1;
			setLeds(0x05);
		}
		if((input_buffer[0])==0x80) //CMD_SET_PICTYPE
		{
			output_buffer[0]=set_pictype(input_buffer+1);
			//output_buffer[0]=1; //Ok
			counter=1;
			setLeds(0x05);
		}
		if((input_buffer[0])==0x90) //CMD_FIRMWARE_VERSION
		{
			strcpypgm2ram((char*)output_buffer,(const far rom char*)upp_version);
			counter=18;
			setLeds(0x05);
		}
		
	}
    if(counter != 0)
    {
       if(!mUSBGenTxIsBusy())
	       USBGenWrite((byte*)&output_buffer,counter);
       counter=0;
    }//end if
}//end ProcessIO


unsigned char set_pictype(unsigned char* data)
{
	pictype=data[0];
	switch(data[0])
	{
        	case P10F200:
		case P10F202:
		case P16F87XA:
		case P16F62XA:
		case P16F62X:
		case P12F629:
		case P12F6XX:
		case P16F84A:
		case P16F81X:
		case P16F7X:
		case P16F7X7:
		case P16F87X:
		case P16F72:
		case P16F87:
		case P16F54:
		case P16F57:
		case P16F785:
		case P16F59:
		case P16F91X:
		case P16F88X:
		case P16C6XX:
		case P16C55X:
		case P16C7XX:
		case P16C64x:
		case P14000:
		case P16C50X:
		case P16C432:
		case P17CXX:
		case P17C7XX:picfamily=PIC16;break;
		case P18F2XXX:
		case P18FXX2:picfamily=PIC18;break;
		case P18FXX39:
		case P18F6X2X:
		case P18FXX80:
		case P18F8410:
		case P18F6XJXX:
		case P18F45J10:
		case P18F97J60:
		case P18F1X30:
		case P18FXX23:
		case P18FXXK20:picfamily=PIC18;break;
        case P24FJXXXGA0XX:
        case P24H:
        case P24FJXXXGA1:picfamily=PIC24;break;
        case dsP30F:picfamily=dsPIC30;break;
        case dsP33F:picfamily=dsPIC33;break;
   		default: pictype=P18F2XXX;picfamily=PIC18;return 3;break;
	}
	return 1;	
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
