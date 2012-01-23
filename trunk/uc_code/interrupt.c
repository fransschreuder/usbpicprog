/******************************************************************************
* PIC USB
* interrupt.c
* to use the interrupt table with the bootloader in SDCC, the linker must
* be called with the option --lvt-loc=0x800
******************************************************************************/

/** I N C L U D E S **********************************************************/
#ifdef SDCC
#include <pic18f2550.h>
#else
#include <p18cxxx.h>
#endif
#include "typedefs.h"
#include "interrupt.h"
#include "io_cfg.h"
/** V A R I A B L E S ********************************************************/
long timerCnt;
unsigned char timerRunning;
/** I N T E R R U P T  V E C T O R S *****************************************/
#ifndef SDCC
#pragma code high_vector=0x08
void interrupt_at_high_vector(void)
{
    _asm goto high_isr _endasm
}
#pragma code

#pragma code low_vector=0x18
void interrupt_at_low_vector(void)
{
    _asm goto low_isr _endasm
}
#pragma code
#endif
/** D E C L A R A T I O N S **************************************************/
/******************************************************************************
 * Function:        void high_isr(void)
 * PreCondition:    None
 * Input:
 * Output:
 * Side Effects:
 * Overview:
 *****************************************************************************/
#ifndef SDCC
#pragma interrupt high_isr
void high_isr(void)
#else
void high_isr(void) interrupt 2
#endif
{
	if(PIR1bits.TMR1IF)
	{
		if( timerRunning && --timerCnt <= 0 )
			timerRunning = 0;
		/*Pump1=!Pump1;
		Pump2=!Pump1;*/
		TMR1H=TMR1H_PRESET;
		TMR1L=TMR1L_PRESET;
		PIR1bits.TMR1IF=0;
	}
	if(INTCONbits.TMR0IF)
	{
		Pump1=!Pump1;
		Pump2=!Pump1;
		TMR0L=68;//TMR0L_PRESET;
		INTCONbits.TMR0IF=0;
	}

}


/******************************************************************************
 * Function:        void low_isr(void)
 * PreCondition:    None
 * Input:
 * Output:
 * Side Effects:
 * Overview:
 *****************************************************************************/
#ifndef SDCC
#pragma interruptlow low_isr
void low_isr(void)
#else
void low_isr(void) interrupt 1
#endif
{
}
#pragma code

/******************************************************************************
 * This function set up timerCnt
 * at the moment timer1 is a 1 ms timer, if it is changes cnt needs to be adjusted
 */
void startTimerMs( unsigned int cnt )
{
	timerRunning = 0;		// no surprises from timer interrupt
	timerCnt = cnt + 1;
	timerRunning = 1;
}

void DelayMs(unsigned cnt)
{
	startTimerMs( cnt );
	while( timerRunning )continue;
}
/** EOF interrupt.c **********************************************************/
