#include <system.h>
#include "dice.h"


#ifdef _PIC16

#pragma DATA 0x2007, _CP_OFF & _MCLRE_OFF & _PWRTE_ON & _WDT_OFF & _INTRC_OSC_NOCLKOUT
#endif //_PIC16
#pragma CLOCK_FREQ 8000000

const char numbers[6]={0x10,0x01,0x11,0x05,0x15,0x07};
const char turn[4]={0x01,0x02,0x04,0x10};

void main()
{ 
   char a=0;
   char b;
   char c;
   //cmcon=7;
   //ansel=0;
   trisio=0x28;
   gpio=0x17;
   while(1)
   {
	a++;
	if(a>5)a=0;
	if((gpio&0x20)==0x00)
	{
		for(c=0;c<5;c++) 
		{
			for(b=0;b<4;b++)
			{
				gpio=turn[b];
				delay_ms(100);
			}
		}
		gpio=numbers[a];
	}
   }
}
