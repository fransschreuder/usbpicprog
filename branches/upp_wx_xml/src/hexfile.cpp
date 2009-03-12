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
#include <vector>
#include <string.h>

#include "hexfile.h"
#include "pictype.h"

using namespace std;


HexFile::HexFile(PicType* picType,const char* filename)
{
    m_filename[0] = '\0';
    m_bModified = false;

    if((picType!=NULL)&&(filename==NULL))
        newFile(picType);
    else if((picType!=NULL)&&(filename!=NULL))
        open(picType,filename);
    //else: If both arguments are NULL, you need to call open later
}

HexFile::~HexFile()
{
}

/*fill an empty hexfile with 0xFF*/
int HexFile::newFile(PicType* picType)
{
    codeMemory.resize(0);
    dataMemory.resize(0);
    configMemory.resize(0);
    codeMemory.resize(picType->getCurrentPic().CodeSize,0xFF);
    dataMemory.resize(picType->getCurrentPic().DataSize,0xFF);
    configMemory.resize(picType->getCurrentPic().ConfigSize,0xFF);
    trimData(picType);
    for(unsigned int i=0;i<picType->getCurrentPic().ConfigSize;i++)
        configMemory[i]&=picType->getCurrentPic().ConfigMask[i];

    m_bModified = false;
    m_filename[0] = '\0';

    return 0;
}

int HexFile::open(PicType* picType,const char* filename)
{
    unsigned int extAddress=0;
    unsigned int address;
    unsigned int byteCount;
    unsigned int checkSum;
    unsigned int configAddress,dataAddress;
    unsigned int newSize;
    string tempStr;
    RecordType recordType;
    ifstream fp (filename);
    vector<int> lineData;
    configMemory.resize(0);
    dataMemory.resize(0);
    codeMemory.resize(0);

    // for info about the HEX file format see:
    // http://en.wikipedia.org/wiki/Intel_HEX

    if(fp==NULL)
    {
        cerr<<"Could not open Hex file... Exiting\n\n"<<endl;
        return -1;
    }

    do
    {
        fp>>tempStr;
        strcpy(m_filename,filename);

        sscanf(tempStr.c_str(),":%02X",&byteCount);

        if((((byteCount+5)*2)+1)!=(signed)tempStr.size())
        {
            cerr<<"Failure in hex file... Exiting"<<endl;
            return -1;
        }

        sscanf(tempStr.c_str()+3,"%04X",&address);
        sscanf(tempStr.c_str()+7,"%02X",(int*)&recordType);
        lineData.resize(byteCount);
        for(unsigned int i=0;i<byteCount;i++)
        {
            sscanf(tempStr.c_str()+9+(i*2),"%02X",&lineData[i]);
        }
        sscanf(tempStr.c_str()+9+(byteCount*2),"%02X",&checkSum);
        if(!calcCheckSum(byteCount,address,recordType,lineData,checkSum))
        {
            cerr<<"Error in checksum... Exiting"<<endl;
            return -1;
        }

        switch(recordType)
        {
            case DATA:
                /**
                Is The address within the Config Memory range?
                */
                configAddress=picType->getCurrentPic().ConfigAddress;
                if(picType->getCurrentPic().Name.find("P18F")!=0)
                    configAddress*=2;
                if(((extAddress+address)>=(configAddress))&&
                   ((extAddress+address)<(configAddress+picType->getCurrentPic().ConfigSize)))
                {
                    if((signed)configMemory.size()<(picType->getCurrentPic().ConfigSize))
                    {
                        newSize =(extAddress+address+lineData.size())-(configAddress);
                        if((signed)configMemory.size()<newSize)configMemory.resize(newSize,0xFF);
                        for(int i=0;i<(signed)lineData.size();i++)
                            configMemory[extAddress+address+i-configAddress]=lineData[i];
                    }
                    else cerr<<"Data in hex file outside config memory of PIC"<<endl;
                }
                /**
                Is The address within the Code Memory range?
                */
                if((extAddress+address)<(picType->getCurrentPic().CodeSize))
                {
                    if((signed)codeMemory.size()<(picType->getCurrentPic().CodeSize))
                    {
                        newSize =(extAddress+address+lineData.size());
                        if((signed)codeMemory.size()<newSize)codeMemory.resize(newSize,0xFF);
                        for(int i=0;i<(signed)lineData.size();i++)
                            codeMemory[extAddress+address+i]=lineData[i];
                    }
                    else cerr<<"Data in hex file outside code memory of PIC"<<endl;
                }
                /**
                Is The address within the Eeprom Data range?
                */
                dataAddress=picType->getCurrentPic().DataAddress;
                if(picType->getCurrentPic().Name.find("P18F")!=0)dataAddress*=2;
                if(((extAddress+address)>=(dataAddress))&&
                                    ((extAddress+address)<(dataAddress+picType->getCurrentPic().DataSize)))
                {
                    if((signed)dataMemory.size()<(picType->getCurrentPic().DataSize))
                    {
                        if(picType->getCurrentPic().Name.find("P18F")==0)
                            newSize =(extAddress+address+lineData.size())-(dataAddress);
                        else
                            newSize =(extAddress+address+(lineData.size()/2))-(dataAddress);
                        if((signed)dataMemory.size()<newSize)dataMemory.resize(newSize,0xFF);
                        if(picType->getCurrentPic().Name.find("P18F")==0)
                        {
                            for(int i=0;i<(signed)lineData.size();i++)
                                dataMemory[extAddress+address+i-dataAddress]=lineData[i];
                        }
                        else //PIC16 devices contain useless data after every data byte in data memory :(
                        {
                            for(int i=0;i<(signed)lineData.size();i+=2)
                                dataMemory[(extAddress+address+i-dataAddress)/2]=lineData[i];
                        }
                    }
                    else cerr<<"Data in hex file outside data memory of PIC"<<endl;

                }
                break;
            case EXTADDR:
                extAddress=(lineData[0]<<24)|(lineData[1]<<16);
                break;
            case ENDOFFILE:
                break;
            default:
                cerr<<"unknown record type: "<<recordType<<endl;
                return -1;
                break;
        }
    }while(recordType!=ENDOFFILE);

    fp.close();
    trimData(picType);

    m_bModified = false;

    return 0;
}

void HexFile::trimData(PicType* picType)
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
    else
    {
        for(int i=1;i<(signed)codeMemory.size();i++)
        {
            codeMemory[i]&=0xFF;
        }
        for(int i=1;i<(signed)configMemory.size();i++)
        {
            configMemory[i]&=0xFF;
        }
    }
    for(int i=1;i<(signed)dataMemory.size();i++)
    {
        dataMemory[i]&=0xFF;
    }

    m_bModified = true;
}

int HexFile::reload(PicType* picType)
{
    return open(picType,m_filename);
}

int HexFile::saveAs(PicType* picType,const char* filename)
{
    ofstream fp (filename);
    int lineSize;
    vector<int> lineData;
    char txt[256];
    int address;

    if(fp==NULL)
    {
        cerr<<"Could not open Hex file for writing... Exiting\n\n"<<endl;
        return -1;
    }

    strcpy(m_filename,filename);

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
        if(picType->getCurrentPic().Name.find("P18F")!=0)address*=2;
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
        if(picType->getCurrentPic().Name.find("P18F")!=0)address*=2;
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

    m_bModified = false;

    return 0;
}

int HexFile::save(PicType* picType)
{
    return saveAs(picType,m_filename);
}

void HexFile::putCodeMemory(vector<int> &mem)
{
    codeMemory=mem;
    m_bModified = true;
}

void HexFile::putCodeMemory(int address, int mem)
{
    if(codeMemory.size()>(unsigned)address)
    {
        codeMemory[address]=mem;
        m_bModified = true;
    }
}

void HexFile::putDataMemory(vector<int> &mem)
{
    dataMemory=mem;
    m_bModified = true;
}

void HexFile::putDataMemory(int address, int mem)
{
    if(dataMemory.size()>(unsigned)address)
    {
        dataMemory[address]=mem;
        m_bModified = true;
    }
}

void HexFile::putConfigMemory(vector<int> &mem)
{
    configMemory=mem;
    m_bModified = true;
}

void HexFile::putConfigMemory(int address, int mem)
{
    if(configMemory.size()>(unsigned)address)
    {
        configMemory[address]=mem;
        m_bModified = true;
    }
}

bool HexFile::calcCheckSum(int byteCount,int address, RecordType recordType,
                           vector<int> &lineData, int checkSum)
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

void HexFile::makeLine(int address, RecordType recordType, vector<int> &lineData, char* output_line)
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

vector<int> &HexFile::getCodeMemory(void)
{
    return codeMemory;
}

int HexFile::getCodeMemory(int address)
{
    if((unsigned)address<codeMemory.size())
        return codeMemory[address];
    else
        return 0;
}

vector<int> &HexFile::getDataMemory(void)
{
    return dataMemory;
}

int HexFile::getDataMemory(int address)
{
    if((unsigned)address<dataMemory.size())
        return dataMemory[address];
    else
        return 0;
}

vector<int> &HexFile::getConfigMemory(void)
{
    return configMemory;
}

int HexFile::getConfigMemory(int address)
{
    if((unsigned)address<configMemory.size())
        return configMemory[address];
    else
        return 0;
}

void HexFile::print(string* output,PicType *picType)
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


