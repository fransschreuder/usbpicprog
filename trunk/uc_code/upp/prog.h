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
typedef enum _DATASTATE{DATAIDLE=0, DATASTART, DATA, DATASTOP, DATASUCCESS}DATASTATE;
typedef enum _CONFIGSTATE{CONFIGIDLE=0, CONFIGSTART, CONFIG1, CONFIG2, CONFIG3, CONFIG4, CONFIG5, CONFIGSTOP, CONFIGSUCCESS}CONFIGSTATE;

extern ERASESTATE erasestate;
extern PROGSTATE progstate;
extern DATASTATE datastate;
extern CONFIGSTATE configstate;

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
/**
for program_ids, program_memory can be used in stead with address 0x200000 and blocksize=8
void program_ids(PICTYPE pictype,char address, char* data, char blocksize);
**/
void program_data(PICTYPE pictype,unsigned int address, char* data, char blocksize);
void read_program(PICTYPE pictype,unsigned long address, char* data, char blocksize);
void read_data(PICTYPE pictype,unsigned int address, char* data, char blocksize);
void program_config_bits(PICTYPE pictype,unsigned long address, char* data);
char pic18_read(char command);
void pic18_send(char command, unsigned int payload);
//delay times according to programming specification:
#define P9 1
#define P10 1
#define P11 5
#define P11A 4