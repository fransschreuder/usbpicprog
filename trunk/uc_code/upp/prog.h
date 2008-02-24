/**
Low level programming routines for PIC16 and PIC18 

Programming flow:
Init VDD and VPP
Bulk erase
Program Memory
Program IDs
Program Data EE
Verify Program
Verify IDs
Verify Data
Program Configuration bits
Verify Configuration bits
close VDD and MCLR

**/
typedef enum _PICTYPE{PIC12=0,PIC16,PIC18,dsPIC}PICTYPE;
typedef enum _ERASESTATE{ERASEIDLE=0, ERASESTART, ERASE1, ERASE2, ERASESTOP, ERASESUCCESS}ERASESTATE;
typedef enum _PROGSTATE{PROGIDLE=0, PROGSTART, PROG1, PROG2, PROG3, PROG4, PROGNEXTBLOCK, PROGSTOP, PROGSUCCESS}PROGSTATE;

extern ERASESTATE erasestate;
extern PROGSTATE progstate;

/**
This function has to be called as many times until erasestate==ERASESUCCESS
 */
void bulk_erase(PICTYPE pictype);

/**
This function has to be called as many times until progstate==PROGNEXTBLOCK
or when lastblock=1:
call as many times until progstate==PROGSUCCESS
address points to the first byte of the block
data contains the data MSB0, LSB0, MSB1, LSB1, etc...
blocksize is the block syze in BYTES
lastblock is 1 if this block is the last block to program, otherwise lastblock is 0
 */
void program_memory(PICTYPE pictype,unsigned long address, char* data,char blocksize,char lastblock);
void program_ids(PICTYPE pictype,char address, char* data, char blocksize);
void program_data_ee(PICTYPE pictype,char address, char* data, char blocksize);
char verify_program(PICTYPE pictype,char address, char* data, char blocksize);
char verify_ids(PICTYPE pictype,char address, char* data, char blocksize);
char verify_data(PICTYPE pictype,char address, char* data, char blocksize);
void program_config_bits(PICTYPE pictype,char address, char* data, char blocksize);

void pic18_send(char command, unsigned int payload);
//delay times according to programming specification:
#define P9 1
#define P10 1
#define P11 5
#define P11A 4