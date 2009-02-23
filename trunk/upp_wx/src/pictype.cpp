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

/*if no pic is selected, or something goes wrong in detection, this PIC is selected*/
const Pic defaultPic=
{
    "P18F2550",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11240,		//DevId
    0x1FFE0,		//DevIdMask
    {0x3F, 0xCF, 0x3F, 0x1F, 0x00, 0x87, 0xE5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40}
};

/*All the properties of supported PIC devices...*/
const Pic picTypes[]={
{
    "P10F200",	//Name
    0xFE*2,	//CodeSize
    0x1FF,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x0,		//Datasize
    0x2,		//ConfigSize
    P10F200,	//PicFamily
    0xF0000,		//DevId -> this is not implemented in 10F devices, no autodetection possible
    0xFFFFF,		//DevIdMask
    {0x1C,0x00}

},
{
    "P10F202",	//Name
    0x1FE*2,	//CodeSize
    0x3FF,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x0,		//Datasize
    0x2,		//ConfigSize
    P10F202,	//PicFamily
    0xF0000,		//DevId -> this is not implemented in 10F devices, no autodetection possible
    0xFFFFF,		//DevIdMask
    {0x1C,0x00}
},
{
    "P10F204",	//Name
    0xFE*2,	//CodeSize
    0x1FF,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x0,		//Datasize
    0x2,		//ConfigSize
    P10F200,	//PicFamily
    0xF0000,		//DevId -> this is not implemented in 10F devices, no autodetection possible
    0xFFFFF,		//DevIdMask
    {0x1C,0x00}
},
{
    "P10F206",	//Name
    0x1FE*2,	//CodeSize
    0x3FF,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x0,		//Datasize
    0x2,		//ConfigSize
    P10F202,	//PicFamily
    0xF0000,		//DevId -> this is not implemented in 10F devices, no autodetection possible
    0xFFFFF,		//DevIdMask
    {0x1C,0x00}
},
{
    "P16F84A",	//Name
    0x1FF*2,	//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x40,		//Datasize
    0x2,		//ConfigSize
    P16F84A,	//PicFamily
    0x0560,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3F}
},
{
    "P12F629",	//Name
    0x3FF*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P12F629,	//PicFamily
    0x0F80,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x01}
},
{
    "P12F675",	//Name
    0x3FF*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P12F629,	//PicFamily
    0x0FC0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x01}
},
//P16F630 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F630",	//Name
    0x3FF*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P12F629,	//PicFamily
    0x10C0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x31}
},
//PIC16F676 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F676",	//Name
    0x3FF*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P12F629,	//PicFamily
    0x10E0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x31}
},
//PIC12F635
{
    "P12F635",	//Name
    0x400*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P12F6XX,	//PicFamily
    0x0FA0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x1F}
},
//PIC16F684
{
    "P16F684",	//Name
    0x800*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P12F6XX,	//PicFamily
    0x1080,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x0F}
},
//PIC12F683
{
    "P16F683",	//Name
    0x800*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P12F6XX,	//PicFamily
    0x0460,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x0F}
},
//PIC16F685
{
    "P16F685",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P12F6XX,	//PicFamily
    0x04A0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x0F}
},
//PIC16F631
{
    "P16F631",	//Name
    0x400*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P12F6XX,	//PicFamily
    0x1420,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x0F}
},
//PIC16F687
{
    "P16F687",	//Name
    0x800*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P12F6XX,	//PicFamily
    0x1320,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x0F}
},
//PIC16F636
{
    "P16F636",	//Name
    0x800*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P12F6XX,	//PicFamily
    0x10A0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x1F}
},
//PIC16F688
{
    "P16F688",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P12F6XX,	//PicFamily
    0x1080,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x0F}
},
//PIC16F639
{
    "P16F639",	//Name
    0x800*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P12F6XX,	//PicFamily
    0x10A0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x1F}
},
//PIC16F689
{
    "P16F689",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P12F6XX,	//PicFamily
    0x1340,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x0F}
},
//PIC16F677
{
    "P16F677",	//Name
    0x800*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P12F6XX,	//PicFamily
    0x1440,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x0F}
},
//PIC16F690
{
    "P16F690",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P12F6XX,	//PicFamily
    0x1400,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x0F}
},
//PIC16F627 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F627",	//Name
    0x400*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P16F62X,	//PicFamily
    0x07A0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3D}
},
//PIC16F628 	√ 	½ 	√ 	√ 	√ 	√ 	√ 	√ 	√
{
    "P16F628",	//Name
    0x800*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P16F62X,	//PicFamily
    0x07C0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3D}
},
//PIC16F627A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F627A",	//Name
    0x400*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P16F62XA,	//PicFamily
    0x1040,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x21}
},
//PIC16F628A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F628A",	//Name
    0x800*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P16F62XA,	//PicFamily
    0x1060,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x21}
},
//PIC16F648A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F648A",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P16F62XA,	//PicFamily
    0x1100,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x21}
},
//PIC16F72
{
    "P16F72",	//Name
    0x800*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x0,		//Datasize
    0x2,		//ConfigSize
    P16F72,	//PicFamily
    0x0090,		//DevId
    0x3FE0,		//DevIdMask
    {0x5F,0x00} //ConfigMask
},
//PIC16F73
{
    "P16F73",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x0,		//Datasize
    0x2,		//ConfigSize
    P16F7X,	//PicFamily
    0x0600,		//DevId
    0x3FE0,		//DevIdMask
    {0x5F,0x00} //ConfigMask
},
//PIC16F737
{
    "P16F737",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x0,		//Datasize
    0x4,		//ConfigSize
    P16F7X7,	//PicFamily
    0x0BA0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x39,0x43,0x00} //ConfigMask
},
//PIC16F74
{
    "P16F74",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x0,		//Datasize
    0x2,		//ConfigSize
    P16F7X,	//PicFamily
    0x0620,		//DevId
    0x3FE0,		//DevIdMask
    {0x5F,0x00} //ConfigMask
},
//PIC16F747
{
    "P16F747",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x0,		//Datasize
    0x4,		//ConfigSize
    P16F7X7,	//PicFamily
    0x0BE0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x39,0x43,0x00} //ConfigMask
},
//PIC16F76
{
    "P16F76",	//Name
    0x2000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x0,		//Datasize
    0x2,		//ConfigSize
    P16F7X,	//PicFamily
    0x0640,		//DevId
    0x3FE0,		//DevIdMask
    {0x5F,0x00} //ConfigMask
},
//PIC16F767
{
    "P16F767",	//Name
    0x2000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x0,		//Datasize
    0x4,		//ConfigSize
    P16F7X7,	//PicFamily
    0x0EA0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x39,0x43,0x00} //ConfigMask
},
//PIC16F77
{
    "P16F77",	//Name
    0x2000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x0,		//Datasize
    0x2,		//ConfigSize
    P16F7X,	//PicFamily
    0x0660,		//DevId
    0x3FE0,		//DevIdMask
    {0x5F,0x00} //ConfigMask
},
//PIC16F777
{
    "P16F777",	//Name
    0x2000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x0,		//Datasize
    0x4,		//ConfigSize
    P16F7X7,	//PicFamily
    0x0DE0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x39,0x43,0x00} //ConfigMask
},
//PIC16F818
{
    "P16F818",	//Name
    0x400*2,	//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P16F81X,	//PicFamily
    0x04C0,		//DevId
    0x3FF0,		//DevIdMask
    {0xFF,0x3F}
},
//PIC16F819
{
    "P16F819",	//Name
    0x800*2,	//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P16F81X,	//PicFamily
    0x04E0,		//DevId
    0x3FF0,		//DevIdMask
    {0xFF,0x3F}
},
//PIC16F870	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F870",	//Name
    0x400*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x40,		//Datasize
    0x2,		//ConfigSize
    P16F87X,	//PicFamily
    0x0D00,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3B}
},
//PIC16F871	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F871",	//Name
    0x400*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x40,		//Datasize
    0x2,		//ConfigSize
    P16F87X,	//PicFamily
    0x0D20,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3B}
},
//PIC16F872	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F872",	//Name
    0x400*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x40,		//Datasize
    0x2,		//ConfigSize
    P16F87X,	//PicFamily
    0x08E0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3B}
},
//PIC16F873	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F873",	//Name
    0x800*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P16F87X,	//PicFamily
    0x0960,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3B}
},
//PIC16F874	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F874",	//Name
    0x800*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P16F87X,	//PicFamily
    0x0920,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3B}
},
//PIC16F876	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F876",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P16F87X,	//PicFamily
    0x09E0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3B}
},
//PIC16F877	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F877",	//Name
    0x1000*2,	//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P16F87X,	//PicFamily
    0x09A0,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3B}
},
//PIC16F873A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F873A",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P16F87XA,	//PicFamily
    0x0E40,		//DevId
    0x3FF0,		//DevIdMask
    {0xCF,0x2F}
},
//PIC16F874A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F874A",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x2,		//ConfigSize
    P16F87XA,	//PicFamily
    0x0E60,		//DevId
    0x3FF0,		//DevIdMask
    {0xCF,0x2F}
},
//PIC16F876A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F876A",	//Name
    0x2000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P16F87XA,	//PicFamily
    0x0E00,		//DevId
    0x3FF0,		//DevIdMask
    {0xCF,0x2F}
},
//PIC16F877A 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P16F877A",	//Name
    0x2000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P16F87XA,	//PicFamily
    0x0E20,		//DevId
    0x3FF0,		//DevIdMask
    {0xCF,0x2F}
},
//PIC16F882
{
    "P16F882",	//Name
    0x800*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x80,		//Datasize
    0x4,		//ConfigSize
    P16F88X,	//PicFamily
    0x2000,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3F,0x00,0x07}
},
//PIC16F883
{
    "P16F883",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x4,		//ConfigSize
    P16F88X,	//PicFamily
    0x2020,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3F,0x00,0x07}
},
//PIC16F884
{
    "P16F884",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x4,		//ConfigSize
    P16F88X,	//PicFamily
    0x2040,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3F,0x00,0x07}
},
//PIC16F886
{
    "P16F886",	//Name
    0x2000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x4,		//ConfigSize
    P16F88X,	//PicFamily
    0x2060,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3F,0x00,0x07}
},
//PIC16F887
{
    "P16F887",	//Name
    0x2000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x4,		//ConfigSize
    P16F88X,	//PicFamily
    0x2080,		//DevId
    0x3FE0,		//DevIdMask
    {0xFF,0x3F,0x00,0x07}
},
//PIC16F913
{
    "P16F913",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P16F91X,	//PicFamily
    0x13E0,		//DevId
    0x3FF0,		//DevIdMask
    {0xFF,0x2F}
},
//PIC16F914
{
    "P16F914",	//Name
    0x1000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P16F91X,	//PicFamily
    0x13C0,		//DevId
    0x3FF0,		//DevIdMask
    {0xFF,0x2F}
},
//PIC16F916
{
    "P16F917",	//Name
    0x2000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P16F91X,	//PicFamily
    0x13A0,		//DevId
    0x3FF0,		//DevIdMask
    {0xFF,0x2F}
},
//PIC16F917
{
    "P16F917",	//Name
    0x2000*2,		//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P16F91X,	//PicFamily
    0x1380,		//DevId
    0x3FF0,		//DevIdMask
    {0xFF,0x2F}
},
//PIC16F946
{
    "P16F946",	//Name
    0x2000*2,	//CodeSize
    0x2007,		//ConfigAddress
    0x2100,		//DataAddress in hex file
    0x100,		//Datasize
    0x2,		//ConfigSize
    P16F91X,	//PicFamily
    0x1460,		//DevId
    0x3FF0,		//DevIdMask
    {0xFF,0x2F}
},
//PIC18F242 	½ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F242",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18FXX2,	//PicFamily
    0x10480,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00,0x27,0x0F,0x0F,0x00,0x01,0x85,0x00,0x0F,0xC0,0x0F,0xE0,0x0F,0x40} //ConfigMask
},
//PIC18F248 	½ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F248",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18FXX2,	//PicFamily
    0x10800,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00,0x27,0x0F,0x0F,0x00,0x01,0x85,0x00,0x0F,0xC0,0x0F,0xE0,0x0F,0x40} //ConfigMask
},
//PIC18F252 	½ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F252",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18FXX2,	//PicFamily
    0x10400,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00,0x27,0x0F,0x0F,0x00,0x01,0x85,0x00,0x0F,0xC0,0x0F,0xE0,0x0F,0x40} //ConfigMask
},
//PIC18F258 	½ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F258",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18FXX2,	//PicFamily
    0x10840,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00,0x27,0x0F,0x0F,0x00,0x01,0x85,0x00,0x0F,0xC0,0x0F,0xE0,0x0F,0x40} //ConfigMask
},
//PIC18F442 	½ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F442",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18FXX2,	//PicFamily
    0x10490,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00,0x27,0x0F,0x0F,0x00,0x01,0x85,0x00,0x0F,0xC0,0x0F,0xE0,0x0F,0x40} //ConfigMask
},
//PIC18F448 	½ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F448",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18FXX2,	//PicFamily
    0x10820,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00,0x27,0x0F,0x0F,0x00,0x01,0x85,0x00,0x0F,0xC0,0x0F,0xE0,0x0F,0x40} //ConfigMask
},
//PIC18F452 	½ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F452",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18FXX2,	//PicFamily
    0x10420,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00,0x27,0x0F,0x0F,0x00,0x01,0x85,0x00,0x0F,0xC0,0x0F,0xE0,0x0F,0x40} //ConfigMask
},
//PIC18F458 	½ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F458",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18FXX2	,	//PicFamily
    0x10860,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00,0x27,0x0F,0x0F,0x00,0x01,0x85,0x00,0x0F,0xC0,0x0F,0xE0,0x0F,0x40} //ConfigMask
},
//PIC18F2221 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2221",	//Name
    0x1000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12160,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xF5, 0x00, 0x03, 0xC0, 0x03, 0xE0, 0x03, 0x40}

},
//PIC18F2321 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2321",	//Name
    0x2000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12120,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xF5, 0x00, 0x03, 0xC0, 0x03, 0xE0, 0x03, 0x40} //ConfigMask
},
//PIC18F2410 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2410",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x0,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11160,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x03, 0xC0, 0x03, 0xE0, 0x03, 0x40} //ConfigMask
},
//PIC18F2420 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2420",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11140,		//DevId
    0x1FFF0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x03, 0xC0, 0x03, 0xE0, 0x03, 0x40} //ConfigMask
},
//PIC18F2423 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2423",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11150,		//DevId
    0x1FFF0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x03, 0xC0, 0x03, 0xE0, 0x03, 0x40} //ConfigMask
},
//PIC18F2439
{
    "P18F2439",	//Name
    0x3000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0E,		//ConfigSize
    P18FXX39,	//PicFamily
    0x10480,		//DevId
    0x1FFE0,		//DevIdMask
	{0x00, 0x17, 0x0F, 0x0F, 0x00, 0x01, 0x85, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2450 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2450",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x0,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12420,		//DevId
    0x1FFE0,		//DevIdMask
    {0x3F, 0xCF, 0x3F, 0x1F, 0x00, 0x86, 0xED, 0x00, 0x03, 0x40, 0x03, 0x60, 0x03, 0x40} //ConfigMask
},
//PIC18F2455 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2455",	//Name
    0x6000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11260,		//DevId
    0x1FFE0,		//DevIdMask
    {0x3F, 0xCF, 0x3F, 0x1F, 0x00, 0x87, 0xE5, 0x00, 0x07, 0xC0, 0x07, 0xE0, 0x07, 0x40} //ConfigMask
},
//PIC18F2458 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2458",	//Name
    0x6000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12A60,		//DevId
    0x1FFE0,		//DevIdMask
    {0x3F, 0xCF, 0x3F, 0x1F, 0x00, 0x87, 0xE5, 0x00, 0x07, 0xC0, 0x07, 0xE0, 0x07, 0x40} //ConfigMask
},
//PIC18F2480 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2480",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11AE0,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x86, 0xE5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2510 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2510",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x0,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11120,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0x1F, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2515 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2515",	//Name
    0xC000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x0,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x10CE0,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2520 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2520",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11100,		//DevId
    0x1FFF0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2523 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2523",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11110,		//DevId
    0x1FFF0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2525 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2525",	//Name
    0xC000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x10CC0,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2539
{
    "P18F2539",	//Name
    0x6000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0E,		//ConfigSize
    P18FXX39,	//PicFamily
    0x10400,		//DevId
    0x1FFE0,		//DevIdMask
	{0x00, 0x17, 0x0F, 0x0F, 0x00, 0x01, 0x85, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2550 	√ 	√ 	√ 	√ 	√ 	√ 	√ 	√ 	√
{
    "P18F2550",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11240,		//UserId
    0x1FFE0,		//DevIdMask
    {0x3F, 0xCF, 0x3F, 0x1F, 0x00, 0x87, 0xE5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2553 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2553",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12A40,		//DevId
    0x1FFE0,		//DevIdMask
    {0x3F, 0xCF, 0x3F, 0x1F, 0x00, 0x87, 0xE5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2580 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2580",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11AC0,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x86, 0xE5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2585 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2585",	//Name
    0xC000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x10EE0,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x86, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2610 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2610",	//Name
    0x10000,	//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x0,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x10C90,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2620 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2620",	//Name
    0x10000,	//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x10C80,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2680 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2680",	//Name
    0x10000,	//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x10EC0,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x86, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F2682 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2682",	//Name
    0x14000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12700,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x86, 0xC5, 0x00, 0x3F, 0xC0, 0x3F, 0xE0, 0x3F, 0x40} //ConfigMask
},
//PIC18F2685 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F2685",	//Name
    0x18000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12720,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x86, 0xC5, 0x00, 0x3F, 0xC0, 0x3F, 0xE0, 0x3F, 0x40} //ConfigMask
},
//PIC18F4221 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4221",	//Name
    0x1000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12140,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xF5, 0x00, 0x03, 0xC0, 0x03, 0xE0, 0x03, 0x40} //ConfigMask
},
//PIC18F4321 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4321",	//Name
    0x2000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12100,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xF5, 0x00, 0x03, 0xC0, 0x03, 0xE0, 0x03, 0x40} //ConfigMask
},
//PIC18F4410 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4410",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x0,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x110E0,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x03, 0xC0, 0x03, 0xE0, 0x03, 0x40} //ConfigMask
},
//PIC18F4420 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4420",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x110C0,		//DevId
    0x1FFF0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x03, 0xC0, 0x03, 0xE0, 0x03, 0x40} //ConfigMask
},
//PIC18F4423 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4423",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x110D0,		//DevId
    0x1FFF0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x03, 0xC0, 0x03, 0xE0, 0x03, 0x40} //ConfigMask
},
//PIC18F4439
{
    "P18F4439",	//Name
    0x3000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0E,		//ConfigSize
    P18FXX39,	//PicFamily
    0x104A0,		//DevId
    0x1FFE0,		//DevIdMask
	{0x00, 0x17, 0x0F, 0x0F, 0x00, 0x01, 0x85, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4450 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4450",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x0,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12400,		//DevId
    0x1FFE0,		//DevIdMask
    {0x3F, 0xCF, 0x3F, 0x1F, 0x00, 0x86, 0xED, 0x00, 0x03, 0x40, 0x03, 0x60, 0x03, 0x40} //ConfigMask
},
//PIC18F4455 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4455",	//Name
    0x6000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11220,		//DevId
    0x1FFE0,		//DevIdMask
    {0x3F, 0xCF, 0x3F, 0x1F, 0x00, 0x87, 0xE5, 0x00, 0x07, 0xC0, 0x07, 0xE0, 0x07, 0x40} //ConfigMask
},
//PIC18F4458 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4458",	//Name
    0x6000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12A20,		//DevId
    0x1FFE0,		//DevIdMask
    {0x3F, 0xCF, 0x3F, 0x1F, 0x00, 0x87, 0xE5, 0x00, 0x07, 0xC0, 0x07, 0xE0, 0x07, 0x40} //ConfigMask
},
//PIC18F4480 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4480",	//Name
    0x4000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11AA0,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x86, 0xE5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4510 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4510",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x0,		//Datasize
    0x0F,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x110A0,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4515 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4515",	//Name
    0xC000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x0,		//Datasize
    0x0E,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x10C60,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4520 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4520",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0E,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11080,		//DevId
    0x1FFF0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4523 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4523",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0E,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11090,		//DevId
    0x1FFF0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4525 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4525",	//Name
    0xC000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0E,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x10C40,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4539
{
    "P18F4539",	//Name
    0x6000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0E,		//ConfigSize
    P18FXX39,	//PicFamily
    0x10420,		//DevId
    0x1FFE0,		//DevIdMask
	{0x00, 0x17, 0x0F, 0x0F, 0x00, 0x01, 0x85, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4550 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4550",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0E,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11200,		//DevId
    0x1FFE0,	//DevIdMask
    {0x3F, 0xCF, 0x3F, 0x1F, 0x00, 0x87, 0xE5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4553 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4553",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0E,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12A00,		//DevId
    0x1FFE0,		//DevIdMask
    {0x3F, 0xCF, 0x3F, 0x1F, 0x00, 0x87, 0xE5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4580 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4580",	//Name
    0x8000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x100,		//Datasize
    0x0E,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x11A80,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x86, 0xE5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4585 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4585",	//Name
    0xC000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0E,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x10EA0,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x86, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4610 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4610",	//Name
    0x10000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x0,		//Datasize
    0x0E,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x10C20,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4620 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4620",	//Name
    0x10000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0E,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x10C00,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x87, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4680 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4680",	//Name
    0x10000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0E,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x10E80,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x86, 0xC5, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F4682 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F46820",	//Name
    0x14000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0E,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12740,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x86, 0xC5, 0x00, 0x3F, 0xC0, 0x3F, 0xE0, 0x3F, 0x40} //ConfigMask
},
//PIC18F4685 	√ 	U 	U 	U 	U 	U 	U 	U 	U
{
    "P18F4685",	//Name
    0x18000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0E,		//ConfigSize
    P18F2XXX,	//PicFamily
    0x12760,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0xCF, 0x1F, 0x1F, 0x00, 0x86, 0xC5, 0x00, 0x3F, 0xC0, 0x3F, 0xE0, 0x3F, 0x40} //ConfigMask
},
//PIC18F6525
{
    "P18F6525",	//Name
    0x0C000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0E,		//ConfigSize
    P18F6X2X,	//PicFamily
    0x10AE0,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0x2F, 0x0F, 0x1F, 0x83, 0x83, 0x86, 0x00, 0x07, 0xC0, 0x07, 0xE0, 0x07, 0x40} //ConfigMask
},
//PIC18F6621
{
    "P18F6621",	//Name
    0x10000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0E,		//ConfigSize
    P18F6X2X,	//PicFamily
    0x10AA0,		//DevId
    0x1FFE0,		//DevIdMask
	{0x00, 0x2F, 0x0F, 0x1F, 0x00, 0x81, 0x86, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
},
//PIC18F8525 10AC0
{
    "P18F8525",	//Name
    0x0C000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0E,		//ConfigSize
    P18F6X2X,	//PicFamily
    0x10AC0,		//DevId
    0x1FFE0,		//DevIdMask
    {0x00, 0x2F, 0x0F, 0x1F, 0x83, 0x83, 0x86, 0x00, 0x07, 0xC0, 0x07, 0xE0, 0x07, 0x40} //ConfigMask
},
//PIC18F8621 10A80
{
    "P18F8621",	//Name
    0x10000,		//CodeSize
    0x300000,	//ConfigAddress
    0xF00000,	//DataAddress in hex file
    0x400,		//Datasize
    0x0E,		//ConfigSize
    P18F6X2X,	//PicFamily
    0x10A80,		//DevId
    0x1FFE0,		//DevIdMask
	{0x00, 0x2F, 0x0F, 0x1F, 0x00, 0x81, 0x86, 0x00, 0x0F, 0xC0, 0x0F, 0xE0, 0x0F, 0x40} //ConfigMask
}
};


/*Constructor to select a pic by string, usage:
Pictype* picType=new PicType("P18F2550"); */
PicType::PicType(string picTypeStr)
{
    const int numberOfSupportedDevices=sizeof(picTypes)/sizeof(Pic);
    for(int i=0;i<numberOfSupportedDevices;i++)
    {
        if(picTypeStr.compare(picTypes[i].Name)==0)
        {
            currentPic=picTypes[i];
            matchFound=true;
        }
    }
    if(currentPic.Name.compare("")==0)
    {
        //cerr<<"Not Found: "<<picTypeStr<<endl;
        currentPic=defaultPic;
        matchFound=false;
        //cerr<<"Setting the PIC to default: "<<currentPic.Name<<endl;
    }
    picNames.resize(sizeof(picTypes)/sizeof(Pic));
    for(int i=0;i<(signed)picNames.size();i++)
            picNames[i]=picTypes[i].Name;
}

/*Constructor to select a pic by devId, usage:
Pictype* picType=new PicType(0x11240); */
PicType::PicType(int devId)
{
    const int numberOfSupportedDevices=sizeof(picTypes)/sizeof(Pic);
    for(int i=0;i<numberOfSupportedDevices;i++)
    {
        if((devId&picTypes[i].DevIdMask)==(picTypes[i].DevId))
        {
            currentPic=picTypes[i];
            matchFound=true;
            //cout<<"Matchfound true"<<endl;
        }
    }
    if(currentPic.Name.compare("")==0)
    {
        //cerr<<"Not Found: "<<hex<<devId<<endl;
        currentPic=defaultPic;
        matchFound=false;
        //cout<<"Matchfound false"<<endl;
        //cerr<<"Setting the PIC to default: "<<defaultPic.Name<<endl;
    }
}

/*Returns the pic that has been selected*/
Pic PicType::getCurrentPic(void)
{
    return currentPic;
}

/*Returns an array (vector) of strings containing all names of supported PICs*/
vector<string> PicType::getPicNames(void)
{
    picNames.resize(sizeof(picTypes)/sizeof(Pic));
    for(int i=0;i<(signed)picNames.size();i++)
        picNames[i]=picTypes[i].Name;
    return picNames;
}

