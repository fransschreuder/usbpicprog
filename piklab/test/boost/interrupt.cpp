/*
  Basic sample for BoostC++ compiler.
  Use the 'Led Block' plugin to see
  changing value on port B.
*/
#include <system.h>

// Set configuration word (sample only, ajust for your particular case)
#ifdef _PIC16
	#pragma DATA 0x2007, _HS_OSC & _WDT_OFF
#endif //_PIC16

class CPort
{
public:	
	CPort();
	
	void Toggle( void )
	{
#ifdef _PIC16
		portb++;
#else
		latb++;
#endif

	}
};

CPort::CPort()
{
	trisb = 0;		//configure port B

#ifdef _PIC16
	portb = 0;		//clear port B
#else
	latb = 0;		//clear port B
#endif
}

class CTimer
{
public:
	CTimer();
};

CTimer::CTimer()
{
#ifdef _PIC16
	option_reg = 7;	//set prescaler
#else
	// configure Timer0
	set_bit( t0con, TMR0ON );	//enable timer
	clear_bit( t0con, T08BIT );	//set 16-bit mode
	clear_bit( t0con, T0CS );	// select internal clock
	clear_bit( t0con, PSA ); 	// select prescaler
	set_bit( t0con, T0PS0 );	// set 1:64 prescale ratio
	clear_bit( t0con, T0PS1 );  
	set_bit( t0con, T0PS2 );
#endif
	// enable interrupts
	set_bit( intcon, T0IE ); //enable TMR0 overflow bit    
}

class CPort port; //configure port
class CTimer timer; //configure prescaler
			
void interrupt( void )
{
	port.Toggle();

	clear_bit( intcon, T0IF );  //clear TMR0 overflow flag
}

void main()
{
	// enable interrupts
	set_bit( intcon, GIE );
	
	while( 1 ); //endless loop
}
