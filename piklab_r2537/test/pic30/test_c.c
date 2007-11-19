#include <p30f3010.h>
_FOSC(XT_PLL16 & FRC & CSW_FSCM_OFF);
_FWDT(WDTPSB_16 & WDTPSA_512 & WDT_OFF);
_FBORPOR(PWRT_64 & BORV_20 & PBOR_ON & MCLR_EN);
//_FGS(CODE_PROT_ON);

void delay(void);

int main (void)
{
int i = 0xFFFF;

for (;;) {
  delay();
}
}

void delay()
{
int i;
for (i = 0; i < 0xFFFF; i++) {;}
} 
