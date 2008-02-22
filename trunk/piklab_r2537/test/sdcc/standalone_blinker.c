#define __16F873
#include "pic16f873.h"

typedef unsigned int word;
word at 0x2007  __CONFIG = _CP_OFF & _WDT_OFF & _BODEN_ON & \
         _PWRTE_ON & _HS_OSC & _WRT_ENABLE_ON & \
         _LVP_OFF & _DEBUG_OFF & _CPD_OFF;

unsigned char count, x;
unsigned char ms_delay;

void Intr() interrupt 0 {
  ms_delay++;
  PIR1 = 0;
  PEIE = 1;
}

void main() {
  NOT_RBPU=0;
  T2CON=0x7f;
  GIE = 1;
  PEIE = 1;
  //INTCON=0xc0;
  PIR1 = 0;
  PIE1 = 2;
  PR2 = 200;

  TRISB = 0;
  ms_delay = 0;
  while(1) {
    count = ms_delay;
    x = PIE1;
    PORTB = (count & 0xf0) | (x & 0xf);
  }
}
