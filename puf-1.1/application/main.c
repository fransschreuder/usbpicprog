/*-------------------------------------------------------------------------
  main.c - Application main function

             (c) 2006 Pierre Gaufillet <pierre.gaufillet@magic.fr> 

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

#include <pic18fregs.h>
#include "common_types.h"
#include "debug.h"
#include "boot_iface.h"

#define ftoggle_A0() { PORTAbits.AN0 = !PORTAbits.AN0; }


/******************************************************************/

// beware : this is not a C main function, but the application
// entry point called from the boot.

unsigned long count;

void application_main(void) 
{
    
    PORTA = 0x01;
    
    // Reset the Timer0 value
    TMR0H = 0;
    TMR0L = 0;
    
    // Configure the Timer0 and unmask ITs
    T0CON = 0x86; // TMR0ON, 16bits, CLKO, PSA on, 1:256
    INTCONbits.TMR0IE = 1;
    INTCONbits.GIE = 1;

    init_debug();
    debug("init application\n");  
    
    while(usb_active_cfg > 2)
    {
        usb_sleep();
        dispatch_usb_event();
    }
}

/* Interrupt vectors */
#pragma code high_priority_isr 0x2020
void high_priority_isr(void) interrupt
{
    if(INTCONbits.TMR0IF)
    {
        ftoggle_A0();
        INTCONbits.TMR0IF = 0;
    }
}

#pragma code low_priority_isr 0x4000
void low_priority_isr(void) interrupt
{
}

