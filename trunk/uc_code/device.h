#ifndef DEVICE_H
#define DEVICE_H

#define LIST(x) x,
typedef enum _PICFAMILY {
#include "picfamily.h"
	UPP_INVALID_PICFAMILY
}PICFAMILY;
#undef LIST
#define LIST( x, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) x,
typedef enum _PICTYPE {
#include "pictype.h"
	UPP_INVALID_PICTYPE
}PICTYPE;
#undef LIST

typedef struct {
	void (*enter_ISCP)( void );
	void (*bulk_erase)(unsigned char doRestore);
	void (*read_code)( unsigned long address, unsigned char* data, char blocksize, char lastblock );
	void (*read_data)( unsigned long address, unsigned char* data, char blocksize, char lastblock );
	void (*write_code)(unsigned long address, unsigned char* data,char blocksize, char lastblock);
	void (*write_data)( unsigned long address, unsigned char* data, char blocksize, char lastblock );
	void (*write_config_bits)( unsigned long address, unsigned char* data, char blocksize, char lastblock );
	struct {
		unsigned p3_3V:1;
		PICFAMILY family:5;
		 }flags;
} DEVICE;
#define is3_3V()	currDevice.flags.p3_3V

extern rom DEVICE devices[];
extern DEVICE currDevice;
extern rom char *rom pictypeName[];
extern rom char *rom picfamilyName[];
#endif
