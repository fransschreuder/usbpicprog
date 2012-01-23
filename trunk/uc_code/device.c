#include "device.h"


#define LIST(x) #x,

rom char *rom picfamilyName[] = {
#include "picfamily.h"
};

rom char *rom pictypeName[] = {
#include "pictype.h"
};
#undef LIST
