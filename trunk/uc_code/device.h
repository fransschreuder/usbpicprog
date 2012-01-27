#ifndef DEVICE_H
#define DEVICE_H

#define LIST(x) x,
typedef enum _PICFAMILY {
#include "picfamily.h"
	UPP_INVALID_PICFAMILY
}PICFAMILY;
#undef LIST
//#define LIST( x, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) x,
#define LIST( x ) x,
#define ENUM
typedef enum _PICTYPE {
#include "pictype.h"
	P24FJG,
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
		PICTYPE type:7;
		unsigned p3_3V:1;
		PICFAMILY family:5;
		 }flags;
} DEVICE_t;
#define is3_3V()	currDevice.flags.p3_3V
extern rom DEVICE_t devices[];
extern DEVICE_t currDevice;
extern rom char *rom pictypeName[];
extern rom char *rom picfamilyName[];

// device table defines

#define enter_ISCP_XXXX 0
#define bulk_erase_XXXX 0
#define read_code_XXXX 0
#define read_data_XXXX 0
#define write_code_XXXX 0
#define write_data_XXXX 0
#define write_config_bits_XXXX 0

#define enter_ISCP_none 0
//#define bulk_erase_none 0
#define read_code_none 0
#define read_data_none 0
#define write_code_none 0
#define write_data_none 0
#define write_config_bits_none 0

#define enter_ISCP_simpleX enter_ISCP_simple
#define enter_ISCP_simpleL enter_ISCP_simple
#define bulk_erase_P18F67KXX 0
#define XXXXX UPP_INVALID_PICFAMILY

#define p3V 1
#define p5V 0

#define DEVICE_ENTRY( x, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) \
{ enter_ISCP_##a2, bulk_erase_##a3, read_code_##a4, read_data_##a5, write_code_##a6, write_data_##a7, write_config_bits_##a8, x, p##a1, a0 },
#ifdef __18CXX
#pragma romdata DEVICES
#define DEVICE_TABLE rom DEVICE_t
#else
#define DEVICE_TABLE DEVICE_t  __attribute__ ((section ("DEVICES")))
#endif

#endif
