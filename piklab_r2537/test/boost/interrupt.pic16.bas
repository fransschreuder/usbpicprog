#include <basic\PIC16F877.bas>
'
'  Basic sample for BoostBasic compiler.
'  Use the "Led Block" plugin to see
'  changing value on port B.
'

' Set configuration word (sample only, ajust for your particular case)
#pragma DATA _CONFIG, _HS_OSC & _WDT_OFF

Sub interrupt()

	portb = portb + 1

	intcon = intcon & ~(1 << T0IF) 'clear TMR0 overflow flag
	
End Sub

Sub main()

	trisb = 0		'configure port B
	portb = 0		'clear port B

	option_reg = 7	'set prescaler

	' enable interrupts
	intcon = intcon | (1 << T0IE) 'enable TMR0 overflow bit    
	intcon = intcon | (1 << GIE)  'set global interrupt bit
	
	' endless loop
	Do while 1 
	Loop
	
End Sub
