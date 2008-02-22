/* Compile options:  -ml (Large code model) */

#include <stdio.h>

#pragma config WDT = OFF

void main (void)
{
  printf ("Hello, world!\n");

  while (1)
    ;
}
