#include <pic18f452.h>

void main(void)
{
  float f = TRISA;
  int a = TRISB;
  LATA = 0xFF;
  LATAbits.LATA1 = 0;
//       UIE = 0x55;
  f /= a;
  a = f;
  TRISA = a;
  while(1);
}
