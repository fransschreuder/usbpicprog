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
#include "pictype.h"
#include <iostream>
#include <fstream>
#include <cstdlib>


PicType::PicType(string picTypeStr)
{
	const Pic picTypes[]={
		{
			"PIC12F629",	//Name
			0x3FF,		//CodeSize
			0x2000,		//ConfigAddress
			0x2100,		//DataAddress in hex file
			0x80,		//Datasize
			0x0F,		//ConfigSize
			P12F629		//PicFamily
					//UserId
					//UserIdMask
		},
		{
			"P12F675",	//Name
			0x3FF,		//CodeSize
			0x2000,		//ConfigAddress
			0x2100,		//DataAddress in hex file
			0x80,		//Datasize
			0x0F,		//ConfigSize
			P12F629		//PicFamily
		},
		
		{
			"P12F675",	//Name
			0x3FF,		//CodeSize
			0x2000,		//ConfigAddress
			0x2100,		//DataAddress in hex file
			0x80,		//Datasize
			0x0F,		//ConfigSize
			P12F629		//PicFamily
		},
		//P16F630 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
		"P12F675",	//Name
		0x3FF,		//CodeSize
		0x2000,		//ConfigAddress
		0x2100,		//DataAddress in hex file
		0x80,		//Datasize
		0x0F,		//ConfigSize
		P12F629		//PicFamily
		},
		//PIC16F676 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		//PIC16F627 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		//PIC16F628 	√ 	½ 	√ 	√ 	√ 	√ 	√ 	√ 	√
		//PIC16F627A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		//PIC16F628A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		//PIC16F648A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		//PIC16F873A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		//PIC16F874A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		//PIC16F875A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		//PIC16F877A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		//PIC18F242 	½ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F242",	//Name
			0x3FFF,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18FXX2	//PicFamily
		},
		//PIC18F248 	½ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F248",	//Name
			0x3FFF,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18FXX2	//PicFamily
		},
		//PIC18F252 	½ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F253",	//Name
			0x3FFF,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18FXX2	//PicFamily
		},
		//PIC18F258 	½ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F258",	//Name
			0x3FFF,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18FXX2	//PicFamily
		},
		//PIC18F442 	½ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F442",	//Name
			0x3FFF,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18FXX2	//PicFamily
		},
		//PIC18F448 	½ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F448",	//Name
			0x3FFF,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18FXX2	//PicFamily
		},
		//PIC18F452 	½ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F452",	//Name
			0x3FFF,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18FXX2	//PicFamily
		},
		//PIC18F458 	½ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F458",	//Name
			0x3FFF,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18FXX2	//PicFamily
		},
		//PIC18F2221 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2221",	//Name
			0x1000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2321 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2321",	//Name
			0x2000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2410 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2410",	//Name
			0x4000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2420 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2420",	//Name
			0x4000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2423 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2423",	//Name
			0x4000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2450 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2450",	//Name
			0x4000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2455 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2455",	//Name
			0x6000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2458 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2458",	//Name
			0x6000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2480 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2480",	//Name
			0x4000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2510 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2510",	//Name
			0x8000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2515 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2515",	//Name
			0xC000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2520 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2520",	//Name
			0x8000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2523 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2523",	//Name
			0x8000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2525 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2525",	//Name
			0xC000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2550 	√ 	√ 	√ 	√ 	√ 	√ 	√ 	√ 	√
		{
			"P18F2550",	//Name
			0x8000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2553 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2553",	//Name
			0x8000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2580 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2580",	//Name
			0x8000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2585 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2585",	//Name
			0xC000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2610 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2610",	//Name
			0x10000,	//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2620 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2620",	//Name
			0x10000,	//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2680 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2680",	//Name
			0x10000,	//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2682 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2682",	//Name
			0x14000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F2685 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F2685",	//Name
			0x18000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4221 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4221",	//Name
			0x1000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4321 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4321",	//Name
			0x2000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4410 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4410",	//Name
			0x4000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4420 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4420",	//Name
			0x4000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4423 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4423",	//Name
			0x4000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4450 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4450",	//Name
			0x4000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4455 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4455",	//Name
			0x6000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4458 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4458",	//Name
			0x6000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4480 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4480",	//Name
			0x4000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4510 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4510",	//Name
			0x8000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4515 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4515",	//Name
			0xC000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4520 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4520",	//Name
			0x8000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4523 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4523",	//Name
			0x8000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4525 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4525",	//Name
			0xC000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4550 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4550",	//Name
			0x8000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4553 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4553",	//Name
			0x8000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4580 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4580",	//Name
			0x8000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4585 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4585",	//Name
			0xC000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4610 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4610",	//Name
			0x10000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4620 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4620",	//Name
			0x10000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4680 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4680",	//Name
			0x10000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4682 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F46820",	//Name
			0x14000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		},
		//PIC18F4685 	√ 	U 	U 	U 	U 	U 	U 	U 	U
		{
			"P18F4685",	//Name
			0x18000,		//CodeSize
			0x300000,	//ConfigAddress
			0xF00000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0E,		//ConfigSize
			P18F2XXX	//PicFamily
		}
	};
	const int numberOfSupportedDevices=sizeof(picTypes)/sizeof(Pic);
	for(int i=0;i<numberOfSupportedDevices;i++)
	{
		if(picTypeStr.compare(picTypes[i].Name)==0)
		{
			currentPic=picTypes[i];
		}
	}
	if(currentPic.Name.compare("")==0)
	{
		
		cerr<<"Not Found: "<<picTypeStr<<endl;
	}
	
}

Pic PicType::getCurrentPic(void)
{
	return currentPic;
}
