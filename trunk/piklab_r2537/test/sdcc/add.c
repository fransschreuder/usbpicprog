#include <pic16f873.h>

extern unsigned char ms_delay;

void test() {
  ms_delay++;
  PIR1 = 0;
  PEIE = 1;
}
