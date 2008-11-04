

#ifndef PROG_LOLVL_H
#define PROG_LOLVL_H



#include "prog.h"
/**
Sets or clears the VDD and VPP voltages
**/
void set_vdd_vpp(PICFAMILY picfamily, char level);

/**
sets the address pointer to a certain address location
**/
void set_address(PICFAMILY picfamily, unsigned long address);

/**
Would one Nop() cylce be enough delay for all PIC's?
It works for PIC18F2550
**/
#define clock_delay() Nop()
//void clock_delay(void);

/**
reads 8 bits from a pic device with a given cmd_size bits command (only the 2nd byte is read)
 **/
char pic_read_byte2(char cmd_size,char command);

/**
reads 14 bits from a pic16 device (0 payload 0)
**/
unsigned int pic_read_14_bits(char cmd_size, char command);

/**
Writes a n-bit command
**/
void pic_send_n_bits(char cmd_size, char command);

/**
Write 14 bit word with a start and a stop bit (16F devices)
**/
void pic_send_word_14_bits(unsigned int payload);

/**
Writes 16 bits to the PIC (18F devices)
**/
void pic_send_word(unsigned int payload);

/**
Writes a cmd_size bit command + 14 bit payload to a pic16 device
 **/
void pic_send_14_bits(char cmd_size,char command, unsigned int payload);


/**
Writes a cmd_size bit command + 16 bit payload to a pic18 device
 **/
void pic_send(char cmd_size,char command, unsigned int payload);

#endif //PROG_LOLVL_H
