/*
  Random number generator sample for BoostC compiler.
  Use the 'Led Block' plugin to see
  changing random value on port B.
*/
	
#include <system.h>
#include <rand.h>

#pragma CLOCK_FREQ 20000000

// Set configuration word
#ifdef _PIC16
	#pragma DATA 0x2007, _HS_OSC & _WDT_OFF
#endif //_PIC16
	

void main()
{
    trisb = 0;		//configure port B
    portb = 0;

#ifdef _PIC16
    option_reg = 7;	//set prescaler

    set_bit( intcon, GIE );
    set_bit( intcon, T0IE );  //enable TMR0 overflow bit
#else
    clear_bit( t0con, T0CS );  // select internal clock
    clear_bit( t0con, PSA );  // select pres-scaler
    set_bit( t0con, T0PS0 );  // set pres-scaler to 1:256
    set_bit( t0con, T0PS1 );  
    set_bit( t0con, T0PS2 );

    // enable interrupts
    set_bit( intcon, GIE );
    set_bit( intcon, TMR0IE ); //enable TMR0 overflow bit    
#endif

    //Set ramdom number generator seed
    srand( 0x1234 );

    while( 1 ) //endless loop
    {
        portb = rand();
        delay_ms( 100 );
    }
}
