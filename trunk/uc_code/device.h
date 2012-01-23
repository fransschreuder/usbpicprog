#define LIST(x) x,
typedef enum _PICFAMILY {
#include "picfamily.h"
	UPP_INVALID_PICFAMILY
}PICFAMILY;
typedef enum _PICTYPE {
#include "pictype.h"
	UPP_INVALID_PICTYPE
}PICTYPE;
#undef LIST


extern rom char *rom pictypeName[];
extern rom char *rom picfamilyName[];
