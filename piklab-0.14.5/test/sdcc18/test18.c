#include <pic18f2455.h>

void main(void)
{
       TRISA=0x00;
       LATA=0xFF;
       LATAbits.LATA1=0;
       UIE = 0x55;
       while(1);
}
