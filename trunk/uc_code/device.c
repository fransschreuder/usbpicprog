#include "device.h"
#include "prog_lolvl.h"
#include "bulk_erase.h"
#include "read_code.h"
#include "read_data.h"
#include "write_code.h"
#include "write_data.h"
#include "write_config_bits.h"

#define LIST(x) #x,

rom char *rom picfamilyName[] = {
#include "picfamily.h"
};
#undef LIST

#define LIST( x, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) #x,

rom char *rom pictypeName[] = {
#include "pictype.h"
};
#undef LIST

#define enter_ISCP_XXXX 0
#define bulk_erase_XXXX 0
#define read_code_XXXX 0
#define read_data_XXXX 0
#define write_code_XXXX 0
#define write_data_XXXX 0
#define write_config_bits_XXXX 0

#define enter_ISCP_none 0
#define bulk_erase_none 0
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

#define LIST( x, a0, a1, a2, a3, a4, a5, a6, a7, a8 ) \
{ enter_ISCP_##a2, bulk_erase_##a3, read_code_##a4, read_data_##a5, write_code_##a6, write_data_##a7, write_config_bits_##a8, p##a1, a0 },

rom DEVICE devices[] = {
#include "pictype.h"
};
#undef LIST
DEVICE currDevice;
PICFAMILY picfamily=PIC18;
PICTYPE pictype=P18F2XXX;
