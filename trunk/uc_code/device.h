#define LIST(x) x,
typedef enum _PICFAMILY {
#include "picfamily.h"
	UPP_INVALID_PICFAMILY
}PICFAMILY;
#undef LIST
#define LIST( x, a1, a2, a3, a4, a5, a6, a7, a8 ) x,
typedef enum _PICTYPE {
#include "pictype.h"
	UPP_INVALID_PICTYPE
}PICTYPE;
#undef LIST


extern rom char *rom pictypeName[];
extern rom char *rom picfamilyName[];
