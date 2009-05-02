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

// NOTE: to avoid lots of warnings with MSVC 2008 about deprecated CRT functions
//       it's important to include wx/defs.h before STL headers
#include <wx/defs.h>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string.h>

#include "hexfile.h"
#include "pictype.h"

using namespace std;


// ----------------------------------------------------------------------------
// HexFile
// ----------------------------------------------------------------------------

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

int HexFile::newFile(PicType* picType)
{
    m_codeMemory.resize(0);
    m_dataMemory.resize(0);
    m_configMemory.resize(0);
    m_codeMemory.resize(picType->getCurrentPic().CodeSize,0xFF);
    m_dataMemory.resize(picType->getCurrentPic().DataSize,0xFF);
    m_configMemory.resize(picType->getCurrentPic().ConfigSize,0xFF);
    
    trimData(picType);
    
    for (unsigned int i=0;i<picType->getCurrentPic().ConfigSize;i++)
        m_configMemory[i] &= picType->getCurrentPic().ConfigMask[i];

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
    m_configMemory.resize(0);
    m_dataMemory.resize(0);
    m_codeMemory.resize(0);

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
        
        m_filename = string(filename);

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
                if(!picType->getCurrentPic().is16Bit())
                    configAddress*=2;
                if(((extAddress+address)>=(configAddress))&&
                   ((extAddress+address)<(configAddress+picType->getCurrentPic().ConfigSize)))
                {
                    if((signed)m_configMemory.size()<(picType->getCurrentPic().ConfigSize))
                    {
                        newSize =(extAddress+address+lineData.size())-(configAddress);
                        if((signed)m_configMemory.size()<newSize)m_configMemory.resize(newSize,0xFF);
                        for(int i=0;i<(signed)lineData.size();i++)
                            m_configMemory[extAddress+address+i-configAddress]=lineData[i];
                    }
                    else cerr<<"Data in hex file outside config memory of PIC"<<endl;
                }
                /**
                Is The address within the Code Memory range?
                */
                if((extAddress+address)<(picType->getCurrentPic().CodeSize))
                {
                    if((signed)m_codeMemory.size()<(picType->getCurrentPic().CodeSize))
                    {
                        newSize =(extAddress+address+lineData.size());
                        if((signed)m_codeMemory.size()<newSize)m_codeMemory.resize(newSize,0xFF);
                        for(int i=0;i<(signed)lineData.size();i++)
                            m_codeMemory[extAddress+address+i]=lineData[i];
                    }
                    else cerr<<"Data in hex file outside code memory of PIC"<<endl;
                }
                /**
                Is The address within the Eeprom Data range?
                */
                dataAddress=picType->getCurrentPic().DataAddress;
                if(!picType->getCurrentPic().is16Bit())dataAddress*=2;
                if(((extAddress+address)>=(dataAddress))&&
                                    ((extAddress+address)<(dataAddress+picType->getCurrentPic().DataSize)))
                {
                    if((signed)m_dataMemory.size()<(picType->getCurrentPic().DataSize))
                    {
                        if(picType->getCurrentPic().is16Bit())
                            newSize =(extAddress+address+lineData.size())-(dataAddress);
                        else
                            newSize =(extAddress+address+(lineData.size()/2))-(dataAddress);
                        if((signed)m_dataMemory.size()<newSize)m_dataMemory.resize(newSize,0xFF);
                        if(picType->getCurrentPic().is16Bit())
                        {
                            for(int i=0;i<(signed)lineData.size();i++)
                                m_dataMemory[extAddress+address+i-dataAddress]=lineData[i];
                        }
                        else //PIC16 devices contain useless data after every data byte in data memory :(
                        {
                            for(int i=0;i<(signed)lineData.size();i+=2)
                                m_dataMemory[(extAddress+address+i-dataAddress)/2]=lineData[i];
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
    if(!picType->getCurrentPic().is16Bit())
    {
        for(unsigned int i=1;i<m_codeMemory.size();i+=2)
            m_codeMemory[i]&=0x3F;
        for(unsigned int i=1;i<m_configMemory.size();i+=2)
            m_configMemory[i]&=0x3F;
    }
    else
    {
        for(unsigned int i=1;i<m_codeMemory.size();i++)
            m_codeMemory[i]&=0xFF;
        for(unsigned int i=1;i<m_configMemory.size();i++)
            m_configMemory[i]&=0xFF;
    }
    
    for(unsigned int i=1;i<m_dataMemory.size();i++)
        m_dataMemory[i]&=0xFF;

    m_bModified = true;
}

int HexFile::reload(PicType* picType)
{
    return open(picType, m_filename.c_str());
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

    m_filename = string(filename);

    if(m_codeMemory.size()>0)
    {
        lineData.resize(2);
        //Put address 0x0000 in lineData
        lineData[0]=0x00;
        lineData[1]=0x00;
        makeLine(0,EXTADDR,lineData,txt);
        fp<<txt<<endl;
        for(int i=0;i<(signed)m_codeMemory.size();i+=16)
        {
            if(i+16<(signed)m_codeMemory.size())
            {
                lineSize=16;
            }
            else
            {
                lineSize=m_codeMemory.size()-i;
            }
            lineData.resize(lineSize);
            for(int j=0;j<lineSize;j++)
            {
                lineData[j]=m_codeMemory[i+j];
            }
            makeLine(i,DATA,lineData,txt);
            fp<<txt<<endl;

        }
    }
    if(m_dataMemory.size()>0)
    {
        lineData.resize(2);
        //Put address DataAddress in lineData
        address=picType->getCurrentPic().DataAddress;
        if(!picType->getCurrentPic().is16Bit())address*=2;
        lineData[0]=(address>>24)&0xFF;
        lineData[1]=(address>>16)&0xFF;
        makeLine(0,EXTADDR,lineData,txt);
        fp<<txt<<endl;
        for(int i=0;i<(signed)m_dataMemory.size();i+=16)
        {
            if(i+16<(signed)m_dataMemory.size())
            {
                lineSize=16;
            }
            else
            {
                lineSize=m_dataMemory.size()-i;
            }
            lineData.resize(lineSize);
            for(int j=0;j<lineSize;j++)
            {
                lineData[j]=m_dataMemory[i+j];
            }
            makeLine(i+(address&0xFFFF),DATA,lineData,txt);
            fp<<txt<<endl;
        }
    }
    if(m_configMemory.size()>0)
    {
        lineData.resize(2);
        //Put address DataAddress in lineData
        address=picType->getCurrentPic().ConfigAddress;
        if(!picType->getCurrentPic().is16Bit())address*=2;
        lineData[0]=(address>>24)&0xFF;
        lineData[1]=(address>>16)&0xFF;
        makeLine(0,EXTADDR,lineData,txt);
        fp<<txt<<endl;
        for(int i=0;i<(signed)m_configMemory.size();i+=16)
        {
            if(i+16<(signed)m_configMemory.size())
            {
                lineSize=16;
            }
            else
            {
                lineSize=m_configMemory.size()-i;
            }
            lineData.resize(lineSize);
            for(int j=0;j<lineSize;j++)
            {
                lineData[j]=m_configMemory[i+j];
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
    return saveAs(picType, m_filename.c_str());
}

void HexFile::putCodeMemory(vector<int> &mem)
{
    m_codeMemory=mem;
    m_bModified = true;
}

void HexFile::putCodeMemory(int address, int mem)
{
    if(m_codeMemory.size()>(unsigned)address)
    {
        m_codeMemory[address]=mem;
        m_bModified = true;
    }
}

void HexFile::putDataMemory(vector<int> &mem)
{
    m_dataMemory=mem;
    m_bModified = true;
}

void HexFile::putDataMemory(int address, int mem)
{
    if(m_dataMemory.size()>(unsigned)address)
    {
        m_dataMemory[address]=mem;
        m_bModified = true;
    }
}

void HexFile::putConfigMemory(vector<int> &mem)
{
    m_configMemory=mem;
    m_bModified = true;
}

void HexFile::putConfigMemory(int address, int mem)
{
	if(m_configMemory.size()<=(unsigned)address) 
		m_configMemory.resize(address+1);
    if(m_configMemory.size()>(unsigned)address)
    {
        m_configMemory[address]=mem;
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
    return m_codeMemory;
}

int HexFile::getCodeMemory(int address)
{
    if((unsigned)address<m_codeMemory.size())
        return m_codeMemory[address];
    else
        return 0;
}

vector<int> &HexFile::getDataMemory(void)
{
    return m_dataMemory;
}

int HexFile::getDataMemory(int address)
{
    if((unsigned)address<m_dataMemory.size())
        return m_dataMemory[address];
    else
        return 0;
}

vector<int> &HexFile::getConfigMemory(void)
{
    return m_configMemory;
}

int HexFile::getConfigMemory(int address)
{
    if((unsigned)address<m_configMemory.size())
        return m_configMemory[address];
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


