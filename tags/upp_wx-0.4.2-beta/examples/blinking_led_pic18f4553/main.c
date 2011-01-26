/*
	BLINKING LED EXAMPLE
	====================
	
	This firmware just configures as output one of the PIC's pin
	and blinks it at a frequency of about 0.5Hz.

	To test this firmware just:
    1) connect to the D1 pin (pin 20 on a 40-pin 18F4553 DIP) a
       LED in series with a 270 ohm resistor which connects to ground
    2) run upp_wx (the UsbPicProg software) and select the led.hex
       file contained in the folder of this example
    3) choose Actions -> Program or hit F7
    4) detach your programmer and power the PIC

	Code adapted from:
    http://www.pic18f.com/18f4550-c-tutorial/2009/11/16/tutorial-4-hello-world-program-in-c/
*/
#include <p18f4553.h>
#include <delays.h>

// PIC config
#pragma config FOSC   = INTOSCIO_EC	// internal oscillator
#pragma config WDT    = OFF			// watchdog timer OFF

// values for the TRISxx register bits:
#define OUTPUT_PIN          (0)
#define INPUT_PIN           (1)

#define LEDPin 				(LATDbits.LATD1)
#define LEDTris 			(TRISDbits.TRISD1)

void main()
{
	LEDTris = OUTPUT_PIN;	// set LED Pin data direction to OUTPUT
	LEDPin = 1;				// set LED Pin
	
	while (1)
	{
		LEDPin = ~LEDPin;	// toggle LED Pin
		Delay10KTCYx(25);	// delay 250K cycles (1 second at 1MHz since each instruction takes 4 cycles)
	}
}

