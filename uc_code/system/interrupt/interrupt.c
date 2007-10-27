/******************************************************************************
* PIC USB
* interrupt.c
******************************************************************************/

/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system/typedefs.h"
#include "system/interrupt/interrupt.h"
#include "io_cfg.h"
/** V A R I A B L E S ********************************************************/

/** I N T E R R U P T  V E C T O R S *****************************************/

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

/** D E C L A R A T I O N S **************************************************/
/******************************************************************************
 * Function:        void high_isr(void)
 * PreCondition:    None
 * Input:
 * Output:
 * Side Effects:
 * Overview:
 *****************************************************************************/
#pragma interrupt high_isr
void high_isr(void)
{
	if(PIR1bits.TMR1IF)
	{	
		Pump1=!Pump1;
		Pump2=!Pump1;
		TMR1H=TMR1H_PRESET;
		TMR1L=TMR1L_PRESET;
		PIR1bits.TMR1IF=0;
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
#pragma interruptlow low_isr
void low_isr(void)
{
}
#pragma code

/** EOF interrupt.c **********************************************************/
