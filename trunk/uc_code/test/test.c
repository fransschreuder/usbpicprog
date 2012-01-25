#ifdef TEST
#include "prog.h"
#include "test.h"
#include <string.h>

extern PICFAMILY picfamily;
extern PICTYPE pictype;

unsigned char set_pictype( PICTYPE pictype );
void test( PICTYPE pictype );
int main( int argc, char *argv[] )
{
	// very dumb interface if there are arguments then pure numbers are interpreted as pictype enums otherwise as a string pictype
	// no arguments do all pictypes

	if( argc <= 1 )
		for( pictype = 0; pictype < UPP_INVALID_PICTYPE; ++pictype )
			test( pictype );
	else
	{
		while( ++argv, --argc > 0 )
		{
			if( '0' <= argv[0][0] && argv[0][0] <= '9' )
				pictype = (PICTYPE) atoi( argv[0] );
			else
			{
				for( pictype = 0; pictype != UPP_INVALID_PICTYPE; ++pictype )
					if( strcasecmp( pictypeName[pictype], argv[0] ) == 0 )
						break;
				if( pictype == UPP_INVALID_PICTYPE ) {
					fprintf( stderr, "Unknown pictype %s\n", argv[0] );
					continue;
				}
			}
			test( pictype );
		}
	}
}
#define FIRST 1
#define LAST 2

void test( PICTYPE pictype )
{
	int i;
	unsigned char result;
	unsigned long address;
	unsigned char data[1024];
	char blocksize;

	if( pictype >= UPP_INVALID_PICTYPE )
	{
		fprintf( stderr, "Unknown pictype %d\n", pictype );
		return;
	}
	currDevice = devices[pictype];

	address = 0x40;
	blocksize = 1;

	memset( data, 0, sizeof( data ) );

	set_pictype(pictype);
	printf( " Type %s, Family %s\n", pictypeName[pictype], picfamilyName[picfamily] );
	printf( ">>>>>> set_vdd_vpp\n" );
	set_vdd_vpp( pictype, picfamily, 1 );

	printf( ">>>>>> bulk_erase( %s, %s, 0 )\n", picfamilyName[picfamily], pictypeName[pictype] );
	result = bulk_erase( picfamily, pictype, 0 );
	printf( " returns( %d )\n", result );

	printf( ">>>>>> write_code( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 0L, 32, FIRST );
	result = write_code( picfamily, pictype, 0x00, data, 32, FIRST );
	printf( " returns( %d )\n", result );
	printf( ">>>>>> write_code( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 0x20L, 32, 0 );
	result = write_code( picfamily, pictype, 0x20, data, 32, 0 );
	printf( " returns( %d )\n", result );


	printf( ">>>>>> write_data( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 0L, 9, FIRST );
	result = write_data( picfamily, pictype, address, data, 9, FIRST );
	printf( " returns( %d )\n", result );

	printf( ">>>>>> write_config_bits( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 0x2000L, 3, FIRST );
	result = write_config_bits( picfamily, pictype, 0x2000, data, 3, FIRST );
	printf( " returns( %d )\n", result );

	printf( ">>>>>> read_code( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 0L, 3, FIRST );
	result = read_code( picfamily, pictype, 0, data, 3, FIRST );
	printf( " returns( %d )\n", result );
	printf( ">>>>>> read_code( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 0xF80000L, 3, 0 );
	result = read_code( picfamily, pictype, 0xF80000, data, 3, 0 );
	printf( " returns( %d )\n", result );

	printf( ">>>>>>  read_data( %s, %s, %04lX, data, %02X, %X )\n", picfamilyName[picfamily], pictypeName[pictype], 0x02L, 9, FIRST );
	result = read_data( picfamily, pictype, 2, data, 9, FIRST );
	printf( " returns( %d )\n", result );

	printf("done\n");
	printf("\n\n\n\n\n\n\n\n\n\n\n");
}


unsigned char set_pictype( PICTYPE pictype )
{
	switch(pictype)
	{
	        case P12F508:
		case P10F200:
		case P10F202:
		case P16F54:
		case P16F57:
		case P16F59:picfamily=PIC10;break;
		case P16F87XA:
		case P16F62XA:
		case P16F62X:
		case P12F629:
		case P12F6XX:
		case P12F61X:
		case P16F84A:
		case P16F81X:
		case P16F7X:
		case P16F7X7:
		case P16F87X:
		case P16F72:
		case P16F87:
		case P16F785:
		case P16F91X:
		case P16F88X:
		case P16C6XX:
		case P16C55X:
		case P16C7XX:
		case P16C64X:
		case P14000:
		case P16C50X:
		case P16C432:
		case P17CXX:
		case P16F716:
		case P16F182X:	// added
		case P17C7XX:picfamily=PIC16;break;
		case P18F2XXX:
		case P18FXXK20:			
		case P18F4XK22:
		case P18LF4XK22:
		case P18FXX2:
		case P18FXX39:
		case P18F6X2X:
		case P18FXX80:
		case P18F8410:
		case P18F1X30:
		case P18FXX23:
		case P18F13K22:
		case P18F14K22:
		case P18F872X:	// added
		case P18FXX31:	// added
		case P18FX220: 	// added
		case P18LF13K22:
		case P18LF14K22:picfamily=PIC18;break;
		case P18F97J60:
		case P18F6XJXX:
		case P18F45J10:picfamily=PIC18J;break;
		case P18F6XKXX:
		case P18F67KXX:picfamily=PIC18K;break;
		case P24FJXXXGA0XX:
		case P24H:
		case P24FJXXXGA1:
		case P24FXXKAXXX:picfamily=PIC24;break;
		case dsP30F:
		case dsP30F_LV:picfamily=dsPIC30;break;
		case dsP33F:picfamily=dsPIC33;break;
		case I2C_EE_1:
		case I2C_EE_2:picfamily=I2C;break;
   		default: pictype=P18F2XXX;picfamily=PIC18;return 3;break;
	}
	return 1;
}
#else
// needs at least 1 line for MCC18
extern unsigned char timerRunning;
#endif
