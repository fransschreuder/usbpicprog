#include <system.h>

//Target PIC16F628 configuration word
#pragma DATA _CONFIG, _PWRTE_OFF & _WDT_OFF & _INTRC_CLKOUT & _CP_OFF

//Set clock frequency
#pragma CLOCK_FREQ	4000000



void interrupt( void )
{
}


void main( void )
{
	//Configure port A
	trisa = 0x00;
	//Configure port B
	trisb = 0x00;
	//Configure port C
	trisc = 0x00;
	//Configure port D
//	trisd = 0x00;
	//Configure port E
//	trise = 0x00;
	//Configure port GPIO
	//trisio = 0x00;
	//Initialize port A
	//porta = 0x00;
	//Initialize port B
	//portb = 0x00;
	//gpio=0;
	//cmcon = 7; //disable comparators
	//adcon0 = 6; //disable ad converter

	
	intcon = 0x00; //disable interrupts



	//Endless loop
	while( 1 ) 
	{
		porta ^= 0xFF; //blink all leds on port b
		portb ^= 0xFF; //blink all leds on port a
		portc ^= 0xFF; //blink all leds on port a
	//	portd ^= 0xFF; //blink all leds on port a
	//	porte ^= 0xFF; //blink all leds on port a
		//portf ^= 0xFF; //blink all leds on port a
		//gpio ^= 0xFF; //blink all leds on port b
		
		delay_ms(250);
		delay_ms(250);
	}
}
