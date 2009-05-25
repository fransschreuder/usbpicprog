/******************************************************************************
* PIC USB
* interrupt.h
******************************************************************************/

#ifndef INTERRUPT_H
#define INTERRUPT_H

/** I N C L U D E S **********************************************************/
#include "typedefs.h"

/** D E F I N I T I O N S ****************************************************/
#define mEnableInterrupt()          INTCONbits.GIE = 1;


/** S T R U C T U R E S ******************************************************/

/** E X T E R N S ************************************************************/
extern long tick;
extern long lasttick;
/** P R O T O T Y P E S ******************************************************/
void low_isr(void);
void high_isr(void);
void DelayMs(unsigned long cnt);

#endif //INTERRUPT_H




