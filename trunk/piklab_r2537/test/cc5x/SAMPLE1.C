
/* global variables */
char a;
bit b1, b2;

/* assign names to port pins */
bit in  @ PORTB.0;
bit out @ PORTB.1;

void sub( void)
{
    char i;      /* a local variable */

    /* generate 20 pulses */
    for ( i = 0; i < 20; i++)  {
        out = 1;
        nop();
        out = 0;
    }
}



void main( void)
{
    // if (TO == 1 && PD == 1 /* power up */)  {
    //   WARM_RESET:
    //     clearRAM(); // clear all RAM
    // }

    /* NOTE 1: some devices have a comparator module
       that have to be switched off to use certain
       pins as digital IO */
    //CMCON = 0x07;  // switch comparators off

    /* NOTE 2: devices having AD converter may need to
       configure the pins assigned to 'in' and 'out' as
       digital IO */
    //ADCON1 = ..

    /* First decide the initial output level on the
       output port pins, and then define the input/
       output configuration. This avoids spikes at the
       output pins. */

    PORTB = 0b.0000.0010;  /* out = 1 */
    TRISB = 0b.1111.0001;  /* xxxx 0001 */

    a = 9;  /* value assigned to global variable */

    do  {
        if (in == 0)  /* stop if 'in' is low */
             break;
        sub();
    } while ( -- a > 0);  /* 9 iterations */

    // if (some condition)
    //    goto WARM_RESET;

    /* main is terminated by a SLEEP instruction */
}
