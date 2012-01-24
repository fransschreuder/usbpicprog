#include "device.h"


#define LIST(x) #x,

rom char *rom picfamilyName[] = {
#include "picfamily.h"
};
#undef LIST

#define LIST( x, a1, a2, a3, a4, a5, a6, a7, a8 ) #x,

rom char *rom pictypeName[] = {
#include "pictype.h"
};
#undef LIST

PICFAMILY picfamily=PIC18;
PICTYPE pictype=P18F2XXX;
