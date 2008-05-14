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
//#include <string>


using namespace std;

ReadHexFile::ReadHexFile(PicType* picType,const char* filename)
{
	
	codeMemory.resize(0);
    dataMemory.resize(0);
	configMemory.resize(0);
	if((picType==NULL)&&filename==NULL) return; //you need to call open later then
	else if((picType!=NULL)&&filename==NULL) //fill an empty hexfile with 0xFF
	{
        codeMemory.resize(picType->getCurrentPic().CodeSize,0xFF);
    	dataMemory.resize(picType->getCurrentPic().DataSize,0xFF);
	    configMemory.resize(picType->getCurrentPic().ConfigSize,0xFF);
	    trimData(picType);
	    return;
    }
	else open(picType,filename);
}

ReadHexFile::~ReadHexFile()
{
}

int ReadHexFile::open(PicType* picType,const char* filename)
{
	int extAddress=0;
	int address;
	int byteCount;
	int checkSum;
	string tempStr;
	RecordType recordType;
	ifstream fp (filename);
	vector<int> lineData;
	configMemory.resize(0);
	dataMemory.resize(0);
	codeMemory.resize(0);
	if(fp==NULL) 
	{
		//cerr<<"Could not open Hex file... Exiting\n\n"<<endl;
		return -1;
	}
	do
	{
		fp>>tempStr;
		strcpy(filenameToSave,filename);
		
		sscanf(tempStr.c_str(),":%02X",&byteCount);
		
		if((((byteCount+5)*2)+1)!=(signed)tempStr.size())
		{
			//cerr<<"Failure in hex file... Exiting"<<endl;
			return -1;
		}
		
		sscanf(tempStr.c_str()+3,"%04X",&address);
		sscanf(tempStr.c_str()+7,"%02X",(int*)&recordType);
		lineData.resize(byteCount);
		for(int i=0;i<byteCount;i++)
		{
			sscanf(tempStr.c_str()+9+(i*2),"%02X",&lineData[i]);	
		}
		sscanf(tempStr.c_str()+9+(byteCount*2),"%02X",&checkSum);
		if(!calcCheckSum(byteCount,address,recordType,lineData,checkSum))
		{
			//cerr<<"Error in checksum... Exiting"<<endl;
			return -1;
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
					   if((signed)configMemory.size()<(picType->getCurrentPic().ConfigSize))
					   {
						   int newSize =(extAddress+address+lineData.size())-(picType->getCurrentPic().ConfigAddress);
						   if((signed)configMemory.size()<newSize)configMemory.resize(newSize,0xFF);
					   }
					   for(int i=0;i<(signed)lineData.size();i++)
					   configMemory[extAddress+address+i-picType->getCurrentPic().ConfigAddress]=lineData[i];
				   }
				/**
				Is The address within the Code Memory range?
				*/
				if((extAddress+address)<(picType->getCurrentPic().CodeSize))
				   {
					   if((signed)codeMemory.size()<(picType->getCurrentPic().CodeSize))
					   {
						   int newSize =(extAddress+address+lineData.size());
						   if((signed)codeMemory.size()<newSize)codeMemory.resize(newSize,0xFF);
					   }
					   for(int i=0;i<(signed)lineData.size();i++)
						   codeMemory[extAddress+address+i]=lineData[i];
				   }
				/**
				Is The address within the Eeprom Data range?
				*/
				if(((extAddress+address)>=(picType->getCurrentPic().DataAddress))&&
								    ((extAddress+address)<(picType->getCurrentPic().DataAddress+picType->getCurrentPic().DataSize)))
				   {
					   if((signed)dataMemory.size()<(picType->getCurrentPic().DataSize))
					   {
						   int newSize =(extAddress+address+lineData.size())-(picType->getCurrentPic().DataAddress);
						   if((signed)dataMemory.size()<newSize)dataMemory.resize(newSize,0xFF);
					   }
					   for(int i=0;i<(signed)lineData.size();i++)
						   dataMemory[extAddress+address+i-picType->getCurrentPic().DataAddress]=lineData[i];
				   }
				break;
			case EXTADDR:
				extAddress=(lineData[0]<<24)|(lineData[1]<<16);
				break;
			case ENDOFFILE:
				break;
			default:
				//cerr<<"unknown record type: "<<recordType<<endl;
				return -1;
				break;
				
			
		}
	}while(recordType!=ENDOFFILE);
	fp.close();
	trimData(picType);
	return 0;
}

void ReadHexFile::trimData(PicType* picType)
{
    if((picType->getCurrentPic().Name.find("P10")==0)|
        (picType->getCurrentPic().Name.find("P12")==0)|
        (picType->getCurrentPic().Name.find("P16")==0))
    {
        for(int i=1;i<(signed)codeMemory.size();i+=2)
        {
            codeMemory[i]&=0x3F;
        }
        for(int i=1;i<(signed)configMemory.size();i+=2)
        {
            configMemory[i]&=0x3F;
        }
    }
}

int ReadHexFile::reload(PicType* picType)
{
	return open(picType,filenameToSave);
}

int ReadHexFile::saveAs(PicType* picType,const char* filename)
{
	ofstream fp (filename);
	int lineSize;
	vector<int> lineData;
	char txt[256];
	int address;

	if(fp==NULL)
	{
		//cerr<<"Could not open Hex file for writing... Exiting\n\n"<<endl;
		return -1;
	}
	if(codeMemory.size()>0)
	{
    	lineData.resize(2);
        //Put address 0x0000 in lineData
        lineData[0]=0x00;
        lineData[1]=0x00;
        makeLine(0,EXTADDR,lineData,txt);
    	fp<<txt<<endl;
    	for(int i=0;i<(signed)codeMemory.size();i+=16)
    	{
    		if(i+16<(signed)codeMemory.size())
    		{
    			lineSize=16;
    		}
    		else
    		{
    			lineSize=codeMemory.size()-i;
    		}
    		lineData.resize(lineSize);
    		for(int j=0;j<lineSize;j++)
    		{
    			lineData[j]=codeMemory[i+j];
    		}
    		makeLine(i,DATA,lineData,txt);
    		fp<<txt<<endl;
    		
    	}
    }
	if(dataMemory.size()>0)
	{
    	lineData.resize(2);
        //Put address DataAddress in lineData
        address=picType->getCurrentPic().DataAddress;
        lineData[0]=(address>>24)&0xFF;
        lineData[1]=(address>>16)&0xFF;
        makeLine(0,EXTADDR,lineData,txt);
    	fp<<txt<<endl;
    	for(int i=0;i<(signed)dataMemory.size();i+=16)
    	{
    		if(i+16<(signed)dataMemory.size())
    		{
    			lineSize=16;
    		}
    		else
    		{
    			lineSize=dataMemory.size()-i;
    		}
    		lineData.resize(lineSize);
    		for(int j=0;j<lineSize;j++)
    		{
    			lineData[j]=dataMemory[i+j];
    		}
    		makeLine(i+(address&0xFFFF),DATA,lineData,txt);
    		fp<<txt<<endl;
    	}
    }
    if(configMemory.size()>0)
	{
    	lineData.resize(2);
        //Put address DataAddress in lineData
        address=picType->getCurrentPic().ConfigAddress;
        lineData[0]=(address>>24)&0xFF;
        lineData[1]=(address>>16)&0xFF;
        makeLine(0,EXTADDR,lineData,txt);
    	fp<<txt<<endl;
    	for(int i=0;i<(signed)configMemory.size();i+=16)
    	{
    		if(i+16<(signed)configMemory.size())
    		{
    			lineSize=16;
    		}
    		else
    		{
    			lineSize=configMemory.size()-i;
    		}
    		lineData.resize(lineSize);
    		for(int j=0;j<lineSize;j++)
    		{
    			lineData[j]=configMemory[i+j];
    		}
    		makeLine(i+(address&0xFFFF),DATA,lineData,txt);
    		fp<<txt<<endl;
    	}
    }
    lineData.resize(0);
    makeLine(0,ENDOFFILE,lineData,txt);
    fp<<txt<<endl;
	return 0;
	
}


int ReadHexFile::save(PicType* picType)
{
	return saveAs(picType,filenameToSave);
}

void ReadHexFile::putCodeMemory(vector<int> &mem)
{
	codeMemory=mem;
}

void ReadHexFile::putDataMemory(vector<int> &mem)
{
	dataMemory=mem;
}

void ReadHexFile::putConfigMemory(vector<int> &mem)
{
	configMemory=mem;
}

bool ReadHexFile::calcCheckSum(int byteCount,int address, RecordType recordType,vector<int> &lineData, int checkSum)
{
	int check=0;
	check+=byteCount;
	check+=(address>>8)&0xFF;
	check+=(address)&0xFF;
	check+=recordType;
	
	for(int i=0;i<(signed)lineData.size();i++)
	{
		check+=lineData[i];
	}
	
	check=(0x100-check)&0xFF;
	
	if(check!=checkSum)printf("%2X %2X\n",check,checkSum);
	return (check==checkSum);
}

void ReadHexFile::makeLine(int address, RecordType recordType, vector<int> &lineData, char* output_line)
{
    int check=0;
    int byteCount=lineData.size();
	check+=byteCount;
	check+=(address>>8)&0xFF;
	check+=(address)&0xFF;
	check+=recordType;
    sprintf(output_line,":%02X%04X%02X",byteCount,address,recordType);
	for(int i=0;i<(signed)lineData.size();i++)
	{
        sprintf(output_line+9+(i*2),"%02X",lineData[i]);
        check+=(lineData[i]&0xFF);
	}

	check=(0x100-check)&0xFF;

    sprintf(output_line+9+(lineData.size()*2),"%02X",check);
    

}
	
vector<int> &ReadHexFile::getCodeMemory(void)
{
	return codeMemory;
}

vector<int> &ReadHexFile::getDataMemory(void)
{
	return dataMemory;
}

vector<int> &ReadHexFile::getConfigMemory(void)
{
	return configMemory;
}

void ReadHexFile::print(string* output,PicType *picType)
{
    int lineSize;
	char txt[256];
	output->append("Code Memory\n");
	for(int i=0;i<(signed)getCodeMemory().size();i+=16)
	{
		sprintf(txt,"%08X::",i);
		output->append(txt);
		if(i+16<(signed)getCodeMemory().size())
		{
			lineSize=16;
		}
		else
		{
			lineSize=getCodeMemory().size()-i;
		}
		for(int j=0;j<lineSize;j++)
		{
			sprintf(txt,"%02X",getCodeMemory()[i+j]);
			output->append(txt);
		}
		output->append("\n");
	}
	output->append("\nConfig Memory\n");
	for(int i=0;i<(signed)getConfigMemory().size();i+=16)
	{
		sprintf(txt,"%08X::",i+picType->getCurrentPic().ConfigAddress);
		output->append(txt);
		if(i+16<(signed)getConfigMemory().size())
		{
			lineSize=16;
		}
		else
		{
			lineSize=getConfigMemory().size()-i;
		}
		for(int j=0;j<lineSize;j++)
		{
			sprintf(txt,"%02X",getConfigMemory()[i+j]);
			output->append(txt);
		}
		output->append("\n");
	}
	output->append("\nData Memory\n");
	for(int i=0;i<(signed)getDataMemory().size();i+=16)
	{
		sprintf(txt,"%08X::",i);
		output->append(txt);
		if(i+16<(signed)getDataMemory().size())
		{
			lineSize=16;
		}
		else
		{
			lineSize=getDataMemory().size()-i;
		}
		for(int j=0;j<lineSize;j++)
		{
			sprintf(txt,"%02X",getDataMemory()[i+j]);
			output->append(txt);
		}
		output->append("\n");
	}


}


