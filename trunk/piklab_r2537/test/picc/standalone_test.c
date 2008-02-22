/*
 * level - electronic level using the ADXL202
 *
 * $Id: standalone_test.c 713 2006-03-15 00:01:20Z azhyd $
 * $Source$
 *
 * This program consists of an interrupt routine that measures the pulse
 * width from the ADXL202 and a main routine that updates the dot matrix
 * display on the DISPLAY board.
 *
 * Timer1 and the two Capture/Compare registers are used to measure the
 * pulse width.
 *
 * Use PICCLITE to compile this program for the PIC16F877.
 *
 * Status: Sept 25, 2003
 *   Working. The code is pretty brute force and the resolution isn't
 *   what I expected. Need to review calculations for angle vs. accel
 *   (gravity). I determined the zero offsets empirically and hard coded
 *   them. For coding simplicity I am discarding any measurements where
 *   the counter overflows during the measurement.
 *
 *   Improvements should include: a method to zero the offsets at
 *   runtime and increased gain (which probably means going to 16 bit
 *   values). Also consider filtering the measured values to gain stability.
 *   Add code to handle counter overflow.
 */

#include <pic.h>

/*
 * bit macros suggested by the PICCLITE manual
 */
#define BITSET(var, bitno) ((var) |= 1 << (bitno))
#define BITCLR(var, bitno) ((var) &= ~(1 << (bitno)))


/*
 * pulse with variables
 *
 * These are set up the measurePW() ISR and read by the main loop
 */
unsigned char PeriodStartX;
unsigned char PeriodEndX;
unsigned char PulseEndX;
unsigned char PulseEndTempX;
unsigned char PWSyncX;

unsigned char PeriodStartY;
unsigned char PeriodEndY;
unsigned char PulseEndY;
unsigned char PulseEndTempY;
unsigned char PWSyncY;

/*
 * interrupt service routine to measure pulse width
 *
 * This routine uses the capture facility of the 16F877 to measure the
 * period and the pulse width of a signal. This routine has two states
 * the first looks for the rising edge of the signal which indicates the
 * end of one period and the beginning of the next. The second state
 * looks for the falling edge of the signal to determine the pulse
 * width.
 *
 * Only the high 8 bits of the captured time are record. At 20MHz this
 * yields a time resolution of approximately 51us.
 *
 * RC0 and RC1 indicate ISR over-run (in other words the main loop did
 * not handle the measured values quickly enough).
 */
void interrupt
measurePW(void)
{
  /* determined which capture register requires service */
  if (CCP1IF == 1)
  {
    /* CCP1 */
    if ((CCP1CON & 0b00000111) == 0b00000101)
    {
      /* rising edge-
       *   record last rise time (period_end -> period_start)
       *   record last falling time (pulse_end_temp -> pulse_end)
       *   record this rise time (capture -> period_end)
       *   set synchronization flag
       *   change mode to look for falling edge
       */
      PeriodStartX = PeriodEndX;
      PulseEndX = PulseEndTempX;
      PeriodEndX = CCPR1H;
      PWSyncX++;
      CCP1CON = 0b00000100;

      /* Indicated an ISR over-run if the sync flag was
       * not cleared by the main loop
       */
      if (PWSyncX > 1)
      {
        RC0 = 0;
      }
    }
    else /* assume falling edge */
    {
      /* falling-
       *   save this falling time for later
       *   change mode to look for rising edge
       */
      PulseEndTempX = CCPR1H;
      CCP1CON = 0b00000101;
    }
    /*
     * clear interrupt flags and return
     */
    CCP1IF = 0;
  }
  else
  {
    /* CCP2 */
    if ((CCP2CON & 0b00000111) == 0b00000101)
    {
      /* rising edge-
       *   record last rise time (period_end -> period_start)
       *   record last falling time (pulse_end_temp -> pulse_end)
       *   record this rise time (capture -> period_end)
       *   set synchronization flag
       *   change mode to look for falling edge
       */
      PeriodStartY = PeriodEndY;
      PulseEndY = PulseEndTempY;
      PeriodEndY = CCPR2H;
      PWSyncY++;
      CCP2CON = 0b00000100;

      /* Indicated an ISR over-run if the sync flag was
       * not cleared by the main loop
       */
      if (PWSyncY > 1)
      {
        RC3 = 0;
      }
    }
    else /* assume falling edge */
    {
      /* falling-
       *   save this falling time for later
       *   change mode to look for rising edge
       */
      PulseEndTempY = CCPR2H;
      CCP2CON = 0b00000101;
    }
    /*
     * clear interrupt flags and return
     */
    CCP2IF = 0;
  }
}
    

/*
 * measure tilt and update dot matrix display
 *
 * This program uses the times recorded by the measurePW() ISR to
 * calculate pulse width. The pulse width is then translated into a tilt
 * measurement and finally displayed on the dot matrix display.
 */
void
main(void)
{
  unsigned char periodX = 0;
  unsigned char pulseX = 0;
  unsigned char periodY = 0;
  unsigned char pulseY = 0;
  int tiltX;
  int tiltY;

  /*
   * Setup I/O ports
   *  RC0 and RC3 outputs, the rest are inputs.
   *  RA5 output (power to ADXL202)
   *  Set port C outputs high to turn off the LEDs
   */
  TRISC = 0b11110110;
  RA5 = 1;
  PORTC = 0xff;

  /*
   * Setup the I/O ports that control the dot matrix display
   */
  TRISA = 0b11000000;
  TRISB = 0b11100000;
  TRISD = 0b10000000;
  ADCON1 = 0x06;        /* disable ADC so that port A is digital I/O */
  PORTD = 0;            /* turn off all rows */
  PORTB = 0xff;         /* turn off all columns */
  PORTA = 0xff;         /* turn off all columns */

  /* configure timer1
   *   1:1 prescale
   *   Internal clock (Fosc/4)
   *   Enabled
   */
  T1CON = 0b00000001;

  /*
   * configure capture registers
   * capture on rising edge
   */
  CCP1CON = 0b00000101;
  CCP2CON = 0b00000101;

  /*
   * clear sync flag
   */
  PWSyncX = 0;
  PWSyncY = 0;

  /*
   * enable interrupts
   */
  PEIE = 1;
  CCP1IE = 1;
  CCP2IE = 1;
  ei();

  /*
   * main loop
   */
  for (;;)
  {
    /*
     * kick watchdog
     */
    CLRWDT();

    if (PWSyncX > 0)
    {
      /*
       * Test for easy calculations
       * i.e., no counter roll over during the measurment
       */
      if (PeriodEndX > PeriodStartX)
      {
        if (PulseEndX > PeriodStartX)
        {
          /*
           * no roll over, so proceed
           */
          periodX = PeriodEndX - PeriodStartX;
          pulseX = PulseEndX - PeriodStartX;

          periodX = (periodX / 2) + 9;  /* offset = 9 */
          tiltX = (periodX) - pulseX;

          /*
           * turn off all rows and then figure out which row to turn
           * on based on the tilt
           */
          PORTD = 0;
          if (tiltX < -5)
          {
            RD6 = 1;
          }
          else if (tiltX < -4)
          {
            RD5 = 1;
          }
          else if (tiltX < -2)
          {
            RD4 = 1;
          }
          else if (tiltX > 5)
          {
            RD0 = 1;
          }
          else if (tiltX > 4)
          {
            RD1 = 1;
          }
          else if (tiltX > 2)
          {
            RD2 = 1;
          }
          else
          {
            RD3 = 1;
          }
        }
      }
      PWSyncX = 0;
    }

    if (PWSyncY > 0)
    {
      /*
       * Test for easy calculations
       * i.e., no counter roll over during the measurment
       */
      if (PeriodEndY > PeriodStartY)
      {
        if (PulseEndY > PeriodStartY)
        {
          /*
           * no roll over, so proceed
           */
          periodY = PeriodEndY - PeriodStartY;
          pulseY = PulseEndY - PeriodStartY;

          periodY = (periodY / 2) - 1;  /* offset = -1 */
          tiltY = (periodY) - pulseY;

          /*
           * turn off all columns then figure out which column
           * to turn on based on the tilt
           */
          PORTA = PORTA | 0b00111111;
          PORTB = PORTB | 0b00011111;
          if (tiltY < -5)
          {
            RB1 = 0;
          }
          else if (tiltY < -4)
          {
            RB0 = 0;
          }
          else if (tiltY < -2)
          {
            RA4 = 0;
          }
          else if (tiltY > 5)
          {
            RA0 = 0;
          }
          else if (tiltY > 4)
          {
            RA1 = 0;
          }
          else if (tiltY > 2)
          {
            RA2 = 0;
          }
          else
          {
            RA3 = 0;
          }
        }
      }
      PWSyncY = 0;
    }
  }
}

