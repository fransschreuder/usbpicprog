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

void bulk_erase(PICTYPE pictype);
void pic18_send(char command, unsigned int payload);
void program_memory(PICTYPE pictype,char address, char length, char* data);
void program_ids(PICTYPE pictype,char address, char length, char* data);
void program_data_ee(PICTYPE pictype,char address, char length, char* data);
char verify_program(PICTYPE pictype,char address, char length, char* data);
char verify_ids(PICTYPE pictype,char address, char length, char* data);
char verify_data(PICTYPE pictype,char address, char length, char* data);
void program_config_bits(PICTYPE pictype,char address, char length, char* data);

//delay times according to programming specification:
#define P9 1
#define P10 1
#define P11 5
#define P11A 4