/**************************************************************************
 *   Copyright (C) 2012 by Mike Afheldt                                    *
 *   usbpicprog.org                                                        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 **************************************************************************/
#ifndef DEVICE_H
#define DEVICE_H

#define LIST(x) x,
typedef enum _PICFAMILY {
#include "picfamily.h"
	UPP_INVALID_PICFAMILY
}PICFAMILY;

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
		unsigned int type:7;
		unsigned int p3_3V:1;
		unsigned int family:5;
		 }flags;
} DEVICE_t;
#define is3_3V()	currDevice.flags.p3_3V||(ConfigLimitPGDPGC==1)
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
