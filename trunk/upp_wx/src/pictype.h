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
typedef enum _PicFamily{
    P18F2XXX,P18FXX2,P16F87XA,P16F62XA,P16F62X,P12F629,P12F6XX,P16F84A,P16F81X,
    P16F7X,P16F7X7,P16F87X,P16F72,P16F87,P16F54,P16F57,P16F785,P16F59,P16F91X,P16F88X,
    P16C6XX,P16C55X,P16C7XX,P16C64x,P14000,P16C50X,P16C432,P17CXX,P17C7XX,
    P18FXX39,P18F6X2X,P18FXX80,P18F8410,P18F6XJXX,P18F45J10,P18F97J60,P18F1X30,P18FXX23,P18FXXK20,
    P24FJXXXGA0XX,P24FJXXXGA1,dsP30F,dsP33F,P24H,P10F200,P10F202
}PicFamily;

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
    int ConfigMask[16];
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

    bool matchWasFound(void) const {return matchFound;}

private:
/*contains information about the currently selected pic*/
    Pic currentPic;
/*Contains the names of all supported PICs (initialized in constructor)*/
    vector<string> picNames;
    bool matchFound;
};

#endif  //PICTYPE_H
