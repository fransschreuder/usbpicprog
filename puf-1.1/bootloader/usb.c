/*-------------------------------------------------------------------------
  usb.c - Main USB functions

             (c) 2005-2006 Pierre Gaufillet <pierre.gaufillet@magic.fr> 

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
-------------------------------------------------------------------------*/

/* $Id: usb.c,v 1.5 2006/04/21 20:48:25 gaufille Exp $ */

#include <pic18fregs.h>
#include "common_types.h"
#include "usb.h"
#include "debug.h"
#include "usb_descriptors.h"
#include <stdio.h>

/* Buffer descriptors Table */
volatile BufferDescriptorTable __at (0x400) ep_bdt[32];

const USB_Device_Descriptor *device_descriptor;
const void **configuration_descriptor;
const uchar* const *string_descriptor;
void (*** ep_init)(void);
void (*** ep_in)(void);
void (*** ep_out)(void);
void (*** ep_setup)(void);

#pragma udata access usb_device_state
uchar __at(0x005f) usb_device_state;
#pragma udata access usb_active_cfg
uchar __at(0x005e) usb_active_cfg; 
#pragma udata access usb_active_alt_setting
uchar __at(0x005d) usb_active_alt_setting; 

void init_usb(void)
{
    UIE  = 0;
    UCFG = 0x14;
    UCON = 0x08;
    while(UCONbits.SE0);
    UIR  = 0;
    UIE  = 0x11;
    
    // Put the device in powered state
    SET_DEVICE_STATE(POWERED_STATE);
    
    // Clear active configuration and alternate setting
    SET_ACTIVE_CONFIGURATION(0);
    SET_ACTIVE_ALTERNATE_SETTING(0);
}

void reset_usb(void)
{
    static uchar i;
    
    debug("USB reset\n");
    UIR = 0;
    UIE = 0x7b;
    UEIE = 0;
    UADDR = 0x00;
    
    while(UIRbits.TRNIF)
        {
         UIRbits.TRNIF = 0;
        }
        
    // Put the device in default state
    SET_DEVICE_STATE(DEFAULT_STATE);
    
    // Clear active configuration and alternate setting
    SET_ACTIVE_CONFIGURATION(0);
    SET_ACTIVE_ALTERNATE_SETTING(0);
    
    UEP0  = 0; UEP1  = 0; UEP2  = 0; UEP3  = 0;
    UEP4  = 0; UEP5  = 0; UEP6  = 0; UEP7  = 0;
    UEP8  = 0; UEP9  = 0; UEP10 = 0; UEP11 = 0;
    UEP12 = 0; UEP13 = 0; UEP14 = 0; UEP15 = 0;
    
    UCONbits.PKTDIS = 0;
    
    for(i = 0; i < 16; i++)
    {
        ep_init[0][i]();
    }
}

void usb_sleep(void)
{
    // if USB is suspended, go to sleep until 
    // an authorized interrupt is detected
    if(UCONbits.SUSPND)
    {
        // Power saving
        
        // USB interrupt can wake up the device
        // Other interrupts can also wake up the device
        // depending on the application configuration
        PIE2bits.USBIE = 1; 
        Sleep();
        PIR2bits.USBIF = 0;
    }
}

void dispatch_usb_event(void)
{
    // USB activity interrupt
    if(UIEbits.ACTVIE && UIRbits.ACTVIF)
    {
        UCONbits.SUSPND = 0;
        UIEbits.ACTVIE = 0;
        UIRbits.ACTVIF = 0;
    }

    // USB Reset interrupt
    if(UIEbits.URSTIE && UIRbits.URSTIF)
    {
        reset_usb();
    }

    // USB idle interrupt
    if(UIEbits.IDLEIE && UIRbits.IDLEIF)
    {
        debug("Idle\n");
        UIEbits.ACTVIE = 1;
        UIRbits.IDLEIF  = 0;
        UCONbits.SUSPND = 1;
    }
    
    // USB stall interrupt
    if(UIEbits.STALLIE && UIRbits.STALLIF)
    // to be verified. Is it really required ?
    // should EPx be processed ?
    {
        debug("Stalled\n");
        if(UEP0bits.EPSTALL == 1)
        {
            ep_init[GET_ACTIVE_CONFIGURATION()][0]();
            UEP0bits.EPSTALL = 0;
        }
        UIRbits.STALLIF = 0;
    }

    // Transaction interrupt
    // The device has to be at least in DEFAULT mode to
    // enable packet processing 
    if((GET_DEVICE_STATE() >= DEFAULT_STATE) &&
        UIEbits.TRNIE && UIRbits.TRNIF)
    {
       // Process event for endpoint EPx
       if(USTATbits.DIR == OUT)
        {
            if(EP_OUT_BD(USTATbits.ENDP).Stat.PID == SETUP_TOKEN)
            {
                // SETUP packet has been received
                ep_setup[GET_ACTIVE_CONFIGURATION()][USTATbits.ENDP]();
            }
            else
            {
                // OUT packet has been received
                ep_out[GET_ACTIVE_CONFIGURATION()][USTATbits.ENDP]();
            }
        }
        else if(USTATbits.DIR == IN)
        {
            // IN packet has been sent
            ep_in[GET_ACTIVE_CONFIGURATION()][USTATbits.ENDP]();
        }
        
        // Flush transaction
        UIRbits.TRNIF = 0;
    }
}

void fill_in_buffer(uchar EPnum,
                  uchar **source,
                  uint buffer_size, 
                  uint *nb_byte)
{    
    static uint byte_to_send;
    static uchar *dest;
    
    // First, have to figure out how many byte of data to send.
    if(*nb_byte < buffer_size)
    {
        byte_to_send = *nb_byte;
    }
    else
    {
        byte_to_send = buffer_size;
    }
    
    EP_IN_BD(EPnum).Cnt = byte_to_send;
        
    // Set destination pointer
    dest = (uchar __data *) EP_IN_BD(EPnum).ADR;

    // Subtract the number of bytes just about to be sent from the total.
    *nb_byte -= byte_to_send;

    // Copy bytes to be sent    
    while(byte_to_send)
    {
        *dest = **source;
        dest++;
        (*source)++;
        byte_to_send--;
    }
}

