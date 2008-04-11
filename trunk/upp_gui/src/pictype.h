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

typedef enum _PicFamily{P18F2XXX,P18FXX2,P16F87XA,P16F62XA,P16F62X,P12F629,P12F6XX}PicFamily;

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
		PicType(string picType);
		Pic getCurrentPic(void);
	private:
		Pic currentPic;
		
	
};
#endif  //PICTYPE_H
