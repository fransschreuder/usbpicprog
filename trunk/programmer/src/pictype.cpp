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
			"P12F629",	//Name
			0x3FF,		//CodeSize
			0x2000,		//ConfigAddress
			0x2100,		//DataAddress in hex file
			0x80,		//Datasize
			0x0F,		//ConfigSize
			P12F629		//PicFamily
		}
		,
		{
			"P12F675",	//Name
			0x3FF,		//CodeSize
			0x2000,		//ConfigAddress
			0x2100,		//DataAddress in hex file
			0x80,		//Datasize
			0x0F,		//ConfigSize
			P12F629		//PicFamily
		}
		,
		{
			"P18F2550",	//Name
			0x3FFF,		//CodeSize
			0x300000,	//ConfigAddress
			0x200000,	//DataAddress in hex file
			0x100,		//Datasize
			0x0F,		//ConfigSize
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
