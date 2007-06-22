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