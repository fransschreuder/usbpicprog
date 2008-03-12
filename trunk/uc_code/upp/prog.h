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
typedef enum _PICTYPE{PIC10=0,PIC12,PIC16,PIC18,dsPIC}PICTYPE;
typedef enum _PICVARIANT{P18F2XXX,P18FXX2,P16F87XA,P16F62XA}PICVARIANT;

typedef enum _ERASESTATE{ERASEIDLE=0, ERASESTART, ERASE1, ERASE2, ERASESTOP, ERASESUCCESS}ERASESTATE;
typedef enum _PROGSTATE{PROGIDLE=0, PROGSTART, PROG1, PROG2, PROG3, PROG4, PROGNEXTBLOCK, PROGSTOP, PROGSUCCESS}PROGSTATE;
typedef enum _DATASTATE{DATAIDLE=0, DATASTART, DATA, DATASTOP, DATASUCCESS}DATASTATE;
typedef enum _CONFIGSTATE{CONFIGIDLE=0, CONFIGSTART, CONFIG, CONFIGNEXTBLOCK, CONFIGSTOP, CONFIGSUCCESS}CONFIGSTATE;

extern ERASESTATE erasestate;
//extern PERASESTATE perasestate;
extern PROGSTATE progstate;
extern DATASTATE datastate;
extern CONFIGSTATE configstate;


/**
before calling this function, erasestate has to be ERASESTART
This function has to be called as many times until erasestate==ERASESUCCESS
 */
void bulk_erase(PICTYPE pictype, PICVARIANT picvariant);
//void program_erase(PICTYPE pictype);

/**
before calling this function, progstate must be PROGSTART
This function has to be called as many times until progstate==PROGNEXTBLOCK, then load next block and make progstate PROG2 to continue programming
or when lastblock=1:
call as many times until progstate==PROGSUCCESS
address points to the first byte of the block
data contains the data MSB0, LSB0, MSB1, LSB1, etc...
blocksize is the block syze in BYTES
lastblock is 1 if this block is the last block to program, otherwise lastblock is 0
 */
void write_code(PICTYPE pictype, PICVARIANT picvariant, unsigned long address, char* data,char blocksize,char lastblock);

/**
for program_ids, program_memory can be used in stead with address 0x200000 and blocksize=8
void program_ids(PICTYPE pictype,char address, char* data, char blocksize);
**/

/**
before calling this function, datastate must be DATASTART
call as many times until progstate==PROGSUCCESS
 */
void write_data(PICTYPE pictype, PICVARIANT picvariant, unsigned int address, char* data, char blocksize, char lastblock);


/**
program_config_bits writes 2 configuration bytes each time (*data has to be 2 bytes big)
the address will be 0x300000 + the id location
before calling this function, make configstate CONFIGSTART
keep calling this function until configstate==CONFIGSUCCESS
 **/
void write_config_bits(PICTYPE pictype, PICVARIANT picvariant, unsigned long address, char* data, char blocksize, char lastblock);

/**
This function has to be called only once per block
read_program will read program memory, id's and configuration bits
 **/
void read_code(PICTYPE pictype, PICVARIANT picvariant, unsigned long address, char* data, char blocksize, char lastblock);

/**
This function reads a block of data from the data eeprom of size blocksize into *data
call this function only once.
 **/
void read_data(PICTYPE pictype, PICVARIANT picvariant, unsigned int address, char* data, char blocksize, char lastblock);




//delay times according to programming specification for PIC18F:
#define P9 2
#define P10 1
#define P11 10
#define P11A 5
//delay times according to programming specification for PIC16F:
#define Tera 6
#define Tprog 4
#define Tdprog 6
