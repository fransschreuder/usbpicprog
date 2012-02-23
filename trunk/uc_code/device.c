#include "device.h"
#include "prog_lolvl.h"
#include "bulk_erase.h"
#include "read_code.h"
#include "read_data.h"
#include "write_code.h"
#include "write_data.h"
#include "write_config_bits.h"

#define LIST(x) #x,

rom char *rom picfamilyName[] =
{
#include "picfamily.h"
};
//#undef LIST

//#define LIST( x, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) #x,

rom char *rom pictypeName[] =
{
#include "pictype.h"
	 "P24FJG"
};
#undef LIST

#define LIST DEVICE_ENTRY
#pragma romdata DEVICES

DEVICE_TABLE devices[]  =
{
#include "device_table.h"
};

#undef LIST
#pragma romdata


DEVICE_t currDevice;
PICFAMILY picfamily = PIC18;
PICTYPE pictype = P18F2XXX;
