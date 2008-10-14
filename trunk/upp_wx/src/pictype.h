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
#ifndef PICTYPE_H
#define PICTYPE_H
#include <string>
#include <vector>

using namespace std;

/*PicFamily is the range of algorithms, each by a certain range of PIC types*/
typedef enum _PicFamily{P18F2XXX,P18FXX2,P16F87XA,P16F62XA,P16F62X,P12F629,P12F6XX,P16F84A}PicFamily;

/*Some generic properties of a pic.*/
typedef struct 
PIC{
	string Name;
	int CodeSize;
	int ConfigAddress;
	int DataAddress; //in hex file
	int DataSize;
	int ConfigSize;
	PicFamily picFamily;
	int DevId;
	int DevIdMask;
}Pic;

class PicType
{
	public:
/*Constructor to select a pic by string, usage:
 Pictype* picType=new PicType("P18F2550"); */        
		PicType(string picType);
 /*Constructor to select a pic by devId, usage:
 Pictype* picType=new PicType(0x1240); */		
		PicType(int devId);
/*Returns the pic that has been selected*/		
		Pic getCurrentPic(void);
/*Returns an array (vector) of strings containing all names of supported PICs*/		
		vector<string> getPicNames(void);
	private:
/*contains information about the currently selected pic*/
		Pic currentPic;
/*Contains the names of all supported PICs (initialized in constructor)*/		
		vector<string> picNames;
		
	
};
#endif  //PICTYPE_H
