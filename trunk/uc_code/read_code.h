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

void read_code_I2C_EE_1( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void read_code_I2C_EE_2( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void read_code_dsPIC30( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void read_code_PIC18( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void read_code_P16F18XX( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void read_code_PIC16( unsigned long address, unsigned char* data, char blocksize, char lastblock );
void read_code_PIC10( unsigned long address, unsigned char* data, char blocksize, char lastblock );
