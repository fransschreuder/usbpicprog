#include "device.h"


#define LIST(x) #x,

rom char *rom picfamilyName[] = {
#include "picfamily.h"
};

rom char *rom pictypeName[] = {
#include "pictype.h"
};
#undef LIST

PICFAMILY picfamily=PIC18;
PICTYPE pictype=P18F2XXX;
