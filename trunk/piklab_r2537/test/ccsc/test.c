// This file is used to test the CCS compiler from within Piklab
// The resulting HEX file has not been verified in hardware

#include </home/azhyd/prog/ccs/devices/16f877.h>
#fuses HS,NOWDT,PUT,NOPROTECT
#use delay(clock=4000000)

#define LED_PIN PIN_B0

void main(void)
{
	while(1){
		output_low(LED_PIN);
		delay_ms(500);
		output_high(LED_PIN);
		delay_ms(500);
	}
}