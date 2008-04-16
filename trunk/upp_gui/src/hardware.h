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
//
#define VENDOR 0x04D8
#define PRODUCT 0x000E
#include "read_hexfile.h"
#include "pictype.h"
//
class Hardware
{
public:
	Hardware();
	~Hardware();

	int readCode(ReadHexFile *hexData,PicType *picType);
	int writeCode(ReadHexFile *hexData,PicType *picType);
	int readData(ReadHexFile *hexData,PicType *picType);
	int writeData(ReadHexFile *hexData,PicType *picType);
	int readConfig(ReadHexFile *hexData,PicType *picType);
	int writeConfig(ReadHexFile *hexData,PicType *picType);
	

private :
	bool connected(void);
	const char* readResponse(void);
	int readString(char* msg);
	void writeString(const char* msg,int size);
	int readId(void);
	int readCodeBlock(char * msg,int address,int size,int lastblock);
	int readDataBlock(char * msg,int address,int size,int lastblock);
	usb_dev_handle	*_handle;
	
};
#endif //HARDWARE_H
