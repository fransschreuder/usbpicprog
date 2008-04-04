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
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "read_hexfile.h"
#include "pictype.h"
#include <vector>

using namespace std;

ReadHexFile::ReadHexFile(PicType* picType,char* filename)
{
	int extAddress=0;
	int address;
	int byteCount;
	int checkSum;
	string tempStr;
	RecordType recordType;
	ifstream fp (filename);
	vector<int> lineData;
	if(fp==NULL) 
	{
		cerr<<"Could not open Hex file... Exiting\n\n"<<endl;
		return;
	}
	do
	{
		fp>>tempStr;
		
		
		sscanf(tempStr.c_str(),":%02X",&byteCount);
		
		if((((byteCount+5)*2)+1)!=tempStr.size())
		{
			cerr<<"Failure in hex file... Exiting"<<endl;
			return;
		}
		
		sscanf(tempStr.c_str()+3,"%04X",&address);
		sscanf(tempStr.c_str()+7,"%02X",&recordType);
		lineData.resize(byteCount);
		for(int i=0;i<byteCount;i++)
		{
			sscanf(tempStr.c_str()+9+(i*2),"%02X",&lineData[i]);	
		}
		sscanf(tempStr.c_str()+9+(byteCount*2),"%02X",&checkSum);
		if(!calcCheckSum(byteCount,address,recordType,lineData,checkSum))
		{
			cerr<<"Error in checksum... Exiting"<<endl;
			return;
		}
		switch(recordType)
		{
			case DATA:
				/**
				Is The address within the Config Memory range?
				 */
				if(((extAddress+address)>=(picType->getCurrentPic().ConfigAddress))&&
				((extAddress+address)<(picType->getCurrentPic().ConfigAddress+picType->getCurrentPic().ConfigSize)))
				   {
					   if(configMemory.size()<(picType->getCurrentPic().ConfigSize))
					   {
						   int newSize =(extAddress+address+lineData.size())-(picType->getCurrentPic().ConfigAddress);
						   if(configMemory.size()<newSize)configMemory.resize(newSize);
					   }
					   for(int i=0;i<lineData.size();i++)
					   configMemory[extAddress+address+i-picType->getCurrentPic().ConfigAddress]=lineData[i];
				   }
				/**
				Is The address within the Code Memory range?
				*/
				if((extAddress+address)<(picType->getCurrentPic().CodeSize))
				   {
					   if(codeMemory.size()<(picType->getCurrentPic().CodeSize))
					   {
						   int newSize =(extAddress+address+lineData.size());
						   if(codeMemory.size()<newSize)codeMemory.resize(newSize);
					   }
					   for(int i=0;i<lineData.size();i++)
						   codeMemory[extAddress+address+i]=lineData[i];
				   }
				/**
				Is The address within the Eeprom Data range?
				*/
				if(((extAddress+address)>=(picType->getCurrentPic().DataAddress))&&
								    ((extAddress+address)<(picType->getCurrentPic().DataAddress+picType->getCurrentPic().DataSize)))
				   {
					   if(dataMemory.size()<(picType->getCurrentPic().DataSize))
					   {
						   int newSize =(extAddress+address+lineData.size())-(picType->getCurrentPic().DataAddress);
						   if(dataMemory.size()<newSize)dataMemory.resize(newSize);
					   }
					   for(int i=0;i<lineData.size();i++)
						   dataMemory[extAddress+address+i-picType->getCurrentPic().DataAddress]=lineData[i];
				   }
				break;
			case EXTADDR:
				extAddress=(lineData[0]<<24)|(lineData[1]<<16);
				break;
			case ENDOFFILE:
				break;
			default:
				cerr<<"unknown record type: "<<recordType<<endl;
				return;
				break;
				
			
		}
	}while(recordType!=ENDOFFILE);
	fp.close();
	return;
}

bool ReadHexFile::calcCheckSum(int byteCount,int address, RecordType recordType,vector<int> &lineData, int checkSum)
{
	int check=0;
	check+=byteCount;
	check+=(address>>8)&0xFF;
	check+=(address)&0xFF;
	check+=recordType;
	
	for(int i=0;i<lineData.size();i++)
	{
		check+=lineData[i];
	}
	
	check=(0x100-check)&0xFF;
	
	if(check!=checkSum)printf("%2X %2X\n",check,checkSum);
	return (check==checkSum);
}
	
vector<int> ReadHexFile::getCodeMemory(void)
{
	return codeMemory;
}

vector<int> ReadHexFile::getDataMemory(void)
{
	return dataMemory;
}

vector<int> ReadHexFile::getConfigMemory(void)
{
	return configMemory;
}


