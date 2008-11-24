#ifndef PROG_H
#define PROG_H
/**
programming routines for PIC16 and PIC18

**/
typedef enum _PICFAMILY{PIC10=0,PIC12,PIC16,PIC18,PIC24,dsPIC30,dsPIC33}PICFAMILY;
typedef enum _PICTYPE{
		P18F2XXX,P18FXX2,P16F87XA,P16F62XA,P16F62X,P12F629,P12F6XX,P16F84A,P16F81X,
        P16F7X,P16F7X7,P16F87X,P16F72,P16F87,P16F54,P16F57,P16F785,P16F59,P16F91X,P16F88X,
        P16C6XX,P16C55X,P16C7XX,P16C64x,P14000,P16C50X,P16C432,P17CXX,P17C7XX,
        P18FXX39,P18F6X2X,P18FXX80,P18F8410,P18F6XJXX,P18F45J10,P18F97J60,P18F1X30,P18FXX23,P18FXXK20,
        P24FJXXXGA0XX,P24FJXXXGA1,dsP30F,dsP33F,P24H,P10F200,P10F202
}PICTYPE;



char bulk_erase(PICFAMILY picfamily, PICTYPE pictype);

char write_code(PICFAMILY picfamily, PICTYPE pictype, unsigned long address, unsigned char* data,char blocksize,char lastblock);

char write_data(PICFAMILY picfamily, PICTYPE pictype, unsigned int address, unsigned char* data, char blocksize, char lastblock);

char write_config_bits(PICFAMILY picfamily, PICTYPE pictype, unsigned long address, unsigned char* data, char blocksize, char lastblock);

void read_code(PICFAMILY picfamily, PICTYPE pictype, unsigned long address, unsigned char* data, char blocksize, char lastblock);

void read_data(PICFAMILY picfamily, PICTYPE pictype, unsigned int address, unsigned char* data, char blocksize, char lastblock);

//delay times according to programming specification for PIC18F:
#define P9 2
#define P10 1
#define P11 10
#define P11A 5
//delay times according to programming specification for PIC16F:
#define Tera 10
#define Tprog 10
#define Tdprog 8
#endif //PROG_H
