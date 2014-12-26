/**************************************************************************
*   Copyright (C) 2008 by Frans Schreuder                                 *
*   usbpicprog.sourceforge.net                                            *
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

#ifndef PROG_H
#define PROG_H


/**
    Programming routines for PIC16 and PIC18.
**/
#include "device.h"


extern PICFAMILY picfamily;
extern PICTYPE pictype;
char bulk_erase(unsigned char doRestore);

char write_code( unsigned long address, unsigned char* data, char blocksize, char lastblock );

char write_data( unsigned long address, unsigned char* data, char blocksize, char lastblock );

char write_config_bits( unsigned long address, unsigned char* data, char blocksize, char lastblock );

char read_code( unsigned long address, unsigned char* data, char blocksize, char lastblock );

unsigned char read_data( unsigned long address, unsigned char* data, char blocksize, char lastblock );


// delay times according to programming specification for PIC18F:
#define P9 2
#define P9A 6
#define P10 1
#define P11 10
#define P11A 5
#define P11K 16

// delay times according to programming specification for PIC16F:
#define Tera 10
#define Tprog 10
#define Tdprog 8

#endif // PROG_H
