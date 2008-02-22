// read and write eeprom of PIC18F2550 (and 18F2455, 18F4455, 18F4550)
// EEPROM size is 256 bytes
// (c) Raphael Wimmer. Licensed under GNU GPL v2 or higher
#include <pic18f452.h>
//#include <pic18fregs.h>
#include <stdio.h>
#include <usart.h>
#include <adc.h>

code char at __CONFIG1H config1h = 0xFF & _OSC_XT_1H & _OSCS_OFF_1H;
code char at __CONFIG2L config2l = 0xFF & _PUT_ON_2L & _BODEN_OFF_2L & _BODENV_2_7V_2L;
code char at __CONFIG2H config2h = 0xFF & _WDT_OFF_2H;
code char at __CONFIG3H config3h = 0xFF & _CCP2MUX_RC1_3H;
code char at __CONFIG4L config4l = 0xFF & _LVP_OFF_4L & _BACKBUG_ON_4L & _STVR_OFF_4L;
code char at __CONFIG5L config5l = 0xFF & _CP_0_OFF_5L & _CP_1_OFF_5L;
code char at __CONFIG5H config5h = 0xFF & _CPD_OFF_5H & _CPB_OFF_5H;
code char at __CONFIG6L config6l = 0xFF & _WRT_0_OFF_6L & _WRT_1_OFF_6L;
code char at __CONFIG6H config6h = 0xFF & _WRTD_OFF_6H & _WRTB_OFF_6H & _WRTC_OFF_6H;
code char at __CONFIG7L config7l = 0xFF & _EBTR_0_OFF_7L & _EBTR_1_OFF_7L;
code char at __CONFIG7H config7h = 0xFF & _EBTRB_OFF_7H;

#pragma stack 0x300 0xff // set 64 byte stack at 0x300, needed by sdcc

static void isr(void) interrupt 1 {
/*
n Interrupt     Vector  Interrupt Vector Address
0 RESET         vector  0x000000
1 HIGH priority interrupts 0x000008
2 LOW priority interrupts 0x000018
*/
}

void ee_write_byte(unsigned char address, unsigned char *_data){

    EEDATA = *_data;
    EEADR = address;
    // start write sequence as described in datasheet, page 91
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1; // enable writes to data EEPROM
    INTCONbits.GIE = 0;  // disable interrupts
    EECON2 = 0x55;
    EECON2 = 0x0AA;
    EECON1bits.WR = 1;   // start writing
    while(EECON1bits.WR){
        _asm nop _endasm;}
    if(EECON1bits.WRERR){
       // printf("ERROR: writing to EEPROM failed!\n");
    }
    EECON1bits.WREN = 0;
    INTCONbits.GIE = 1;  // enable interrupts
}

void adc_start() {

        adc_open(ADC_CHN_1, ADC_FOSC_4, ADC_CFG_5A_0R, ADC_FRM_RJUST);
        while (adc_busy()) ;
        adc_read();
//unsigned char channel, unsigned char fosc, unsigned char pcfg, unsigned char config

}

void ee_read_byte(unsigned char address, unsigned char *_data){
    EEADR = address;
    EECON1bits.CFGS = 0;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    *_data = EEDATA;
}

void initUsart()
{
    usart_open(    // Use USART library to initialise the hardware
            USART_TX_INT_OFF
            & USART_RX_INT_OFF
            & USART_BRGH_HIGH
            & USART_ASYNCH_MODE
            & USART_EIGHT_BIT,
            10                      // '10' = 115200 Baud with 20 MHz oscillator and BRGH=1
            );
    stdout = STREAM_USART;
}



// very simple example. use on an erased eeprom

void main(){

    char save_me = 'c';
    char from_eeprom;
       adc_start();
    initUsart();

    printf("E");
    ee_read_byte(0x00, &from_eeprom);
    printf("%c", from_eeprom);

    ee_write_byte(0x00, &save_me);
    printf("%c", save_me);

    ee_read_byte(0x00, &from_eeprom);
    printf("%c", from_eeprom);
while (1){}
}
