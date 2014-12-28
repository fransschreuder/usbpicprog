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

/*
 * covers
 *
 * • PIC24FJ32GA102	• PIC24FJ64GA102
 * • PIC24FJ32GA104	• PIC24FJ64GA104
 * • PIC24FJ32GB002	• PIC24FJ64GB002
 * • PIC24FJ32GB004	• PIC24FJ64GB004
 * as defined in PIC24FJ64GA1/GB0 Microchip Doc# DS39934B
 *
 * • PIC24FJ256GA106	• PIC24FJ256GB106
 * • PIC24FJ256GA108	• PIC24FJ256GB108
 * • PIC24FJ256GA110	• PIC24FJ256GB110
 * • PIC24FJ192GA106	• PIC24FJ192GB106
 * • PIC24FJ192GA108	• PIC24FJ192GB108
 * • PIC24FJ192GA110	• PIC24FJ192GB110
 * • PIC24FJ128GA106	• PIC24FJ128GB106
 * • PIC24FJ128GA108	• PIC24FJ128GB108
 * • PIC24FJ128GA110	• PIC24FJ128GB110
 * • PIC24FJ64GB106	• PIC24FJ64GB110
 * • PIC24FJ64GB106	• PIC24FJ64GB108
 * • PIC24FJ64GB110
 * as defined in PIC24FJ64GA1/GB0 Microchip Doc# DS39934B
 *
 * • PIC24FJ16GA002	• PIC24FJ96GA006
 * • PIC24FJ16GA004	• PIC24FJ96GA008
 * • PIC24FJ32GA002	• PIC24FJ96GA010
 * • PIC24FJ32GA004	• PIC24FJ128GA006
 * • PIC24FJ48GA002	• PIC24FJ128GA008
 * • PIC24FJ48GA004
 * • PIC24FJ128GA010
 * • PIC24FJ64GA002
 * • PIC24FJ64GA004
 * • PIC24FJ64GA006
 * • PIC24FJ64GA008
 * • PIC24FJ64GA010
 * as defined in PIC24FJXXXGA0XX Microchip Doc# DS39934B
 *
 *
 * as defined in dsPIC33F/PIC24H Microchip Doc# DS70152E
 *
 * • PIC24F04KA200	• PIC24F04KA201
 * as defined in PIC24FXXKA2XX Microchip Doc# DS39991A
 *
 * • PIC24F08KA101	• PIC24F32KA301
 * • PIC24F16KA101	• PIC24F16KA302
 * • PIC24F08KA102	• PIC24F32KA302
 * • PIC24F16KA102	• PIC24F16KA304
 * • PIC24F16KA301	• PIC24F32KA304
 * as defined in PIC24FXXKA1XX Microchip Doc# DS39919B
 *
 */

#ifndef PIC24_H
#define PIC24_H
void bulk_erase_PIC24( unsigned int nv );
void bulk_erase_P24KA( unsigned char doRestore );
void bulk_erase_P24FJ( unsigned char doRestore );
void bulk_erase_PIC24E( unsigned int nv );
void bulk_erase_P24EP( unsigned char doRestore );

void write_code_PIC24( unsigned long address, unsigned char* data, char blocksize, char lastblock, int nv, unsigned char write_size );
void write_code_P24KA( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void write_code_P24FJ( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void write_code_PIC24E( unsigned long address, unsigned char* data, char blocksize, char lastblock, int nv );
void write_code_P24EP( unsigned long address, unsigned char* data, char blocksize, char lastblock );

void write_config_bits_PIC24( unsigned long address, unsigned char* data, char blocksize, unsigned int nv );
void write_config_bits_P24KA( unsigned long address, unsigned char* data, char blocksize, char lastblock );

void write_config_bits_P24FJ( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void write_config_bits_P24H( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void write_config_bits_P24EP( unsigned long address, unsigned char* data, char blocksize, char lastblock );

void write_data_P24KA( unsigned long address, unsigned char* data, char blocksize, char lastblock );

void read_code_PIC24( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void read_code_PIC24E( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void read_code_PIC24E( unsigned long address, unsigned char* data, char blocksize, char lastblock );

void read_data_P24KA( unsigned long address, unsigned char* data, char blocksize, char lastblock );


#endif //PIC24_H
