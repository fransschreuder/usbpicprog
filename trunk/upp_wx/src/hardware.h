/***************************************************************************
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
 ***************************************************************************/
 
 #ifndef HARDWARE_H
#define HARDWARE_H
#include <usb.h>
#include <iostream>
//
#define VENDOR 0x04D8
#define PRODUCT 0x000E
#include "read_hexfile.h"
#include "pictype.h"
//

#define CMD_ERASE 0x10
#define CMD_READ_ID 0x20
#define CMD_WRITE_CODE 0x30
#define CMD_READ_CODE 0x40
#define CMD_WRITE_DATA 0x50
#define CMD_READ_DATA 0x60
#define CMD_WRITE_CONFIG 0x70
#define CMD_SET_PICTYPE 0x80
#define CMD_FIRMWARE_VERSION 0x90

#define BLOCKTYPE_MIDDLE 0
#define BLOCKTYPE_FIRST 1
#define BLOCKTYPE_LAST 2
#define BLOCKTYPE_FIRST_LAST 3

#define BLOCKSIZE_DATA 32
#define BLOCKSIZE_CONFIG 8
#define BLOCKSIZE_CODE 32

typedef union _UppPackage
{
	struct _fields
	{
		unsigned cmd:8;
		unsigned size:8;
		unsigned addrU:8;
		unsigned addrH:8;
		unsigned addrL:8;
		unsigned blocktype:8;
		char dataField[32];
	}fields;
	char data[37];
}UppPackage;

class Hardware
{
public:
	Hardware();
	~Hardware();
	int bulkErase(void);
	int setPicType(PicType* picType);
	int readCode(ReadHexFile *hexData,PicType *picType);
	int writeCode(ReadHexFile *hexData,PicType *picType);
	int readData(ReadHexFile *hexData,PicType *picType);
	int writeData(ReadHexFile *hexData,PicType *picType);
	int readConfig(ReadHexFile *hexData,PicType *picType);
	int writeConfig(ReadHexFile *hexData,PicType *picType);
	int autoDetectDevice(void);
	bool connected(void);
	
private :
	const char* readResponse(void);
	int readString(char* msg);
	int writeString(const char* msg,int size);
	int readId(void);
	int readCodeBlock(char * msg,int address,int size,int lastblock);
	int writeCodeBlock(char * msg,int address,int size,int lastblock);
	int writeConfigBlock(char * msg,int address,int size,int lastblock);
	int readDataBlock(char * msg,int address,int size,int lastblock);
	int writeDataBlock(char * msg,int address,int size,int lastblock);
	usb_dev_handle	*_handle;
	
};
#endif //HARDWARE_H
