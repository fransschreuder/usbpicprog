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

#include <wx/log.h>
#include <wx/string.h>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

#include "hexfile.h"
#include "pictype.h"

using namespace std;


// ----------------------------------------------------------------------------
// HexFile
// ----------------------------------------------------------------------------

HexFile::HexFile(PicType* picType, const char* filename)
{
    m_bModified = false;

    if (picType!=NULL && filename==NULL)
        newFile(picType);
    else if (picType!=NULL && filename!=NULL)
        open(picType,filename);
    //else: if both arguments are NULL, you need to call open() later
}

void HexFile::newFile(PicType* picType)
{
    m_codeMemory.resize(0);
    m_dataMemory.resize(0);
    m_configMemory.resize(0);
    
    m_codeMemory.resize(picType->CodeSize,0xFF);
    m_dataMemory.resize(picType->DataSize,0xFF);
    m_configMemory.resize(picType->ConfigSize,0xFF);
    calcConfigMask(picType);
    
    trimData(picType);
    
    for (unsigned int i=0;i<picType->ConfigSize;i++)
        m_configMemory[i] &= m_configMask[i];

    m_bModified = false;
    m_filename.clear();
}

bool HexFile::open(PicType* picType, const char* filename)
{
    unsigned int extAddress=0;
    unsigned int address;
    unsigned int byteCount;
    unsigned int checkSum;
    unsigned int configAddress,dataAddress;
    unsigned int newSize;
    string tempStr;
    RecordType recordType;
	ifstream fp(filename);
    vector<int> lineData;
    
    m_configMemory.resize(0);
    m_dataMemory.resize(0);
    m_codeMemory.resize(0);

    // for info about the HEX file format see:
    // http://en.wikipedia.org/wiki/Intel_HEX

    if (fp==NULL)
    {
        wxLogError("Could not open Hex file...");
        return false;
    }
            
    m_filename = wxString(filename);


    // parse the file

    do
    {
        // read a line from the file
        fp>>tempStr;

        // first 2 digits are the byte count
        sscanf(tempStr.c_str(),":%02X",&byteCount);
        if ((((byteCount+5)*2)+1)!=(signed)tempStr.size())
        {
            wxLogError("Failure in hex file...");
            return false;
        }

        sscanf(tempStr.c_str()+3,"%04X",&address);
        sscanf(tempStr.c_str()+7,"%02X",(int*)&recordType);

        // read the data from this line
        lineData.resize(byteCount);
        for (unsigned int i=0;i<byteCount;i++)
            sscanf(tempStr.c_str()+9+(i*2),"%02X",&lineData[i]);

        // last two bytes are the checksum
        sscanf(tempStr.c_str()+9+(byteCount*2),"%02X",&checkSum);
        if (!calcCheckSum(byteCount,address,recordType,lineData,checkSum))
        {
            wxLogError("Error in checksum...");
            return false;
        }

        switch (recordType)
        {
        case DATA:
            // is the address within the Config Memory range?
            configAddress=picType->ConfigAddress;
            if (picType->is14Bit())
                configAddress*=2;
            if (((extAddress+address)>=(configAddress))&&
                ((extAddress+address)<(configAddress+picType->ConfigSize)))
            {
                if (m_configMemory.size() < picType->ConfigSize)
                {
                    newSize = extAddress+address+lineData.size() - configAddress;
                    if (m_configMemory.size()<newSize)
                        m_configMemory.resize(newSize,0xFF);

                    for (unsigned int i=0;i<lineData.size();i++)
                        m_configMemory[extAddress+address+i-configAddress]=lineData[i];
                }
                else wxLogError("Data in hex file outside config memory of PIC");
            }
            
            // is the address within the Eeprom Data Memory range?
            dataAddress=picType->DataAddress;
            if (picType->is14Bit())
                dataAddress*=2;
				
            if (((extAddress+address)>=(dataAddress))&&
                ((extAddress+address)<(dataAddress+picType->DataSize)))
            {
                if (m_dataMemory.size() < picType->DataSize)
                {
                    if (picType->is16Bit()||(picType->is24Bit()))
                        newSize = extAddress+address+lineData.size() - dataAddress;
                    else		//for 14
                        newSize = extAddress+address+lineData.size()/2 - dataAddress;
                    
                    if (m_dataMemory.size()<newSize)
                        m_dataMemory.resize(newSize,0xFF);
                    if (picType->is16Bit()||picType->is24Bit())
                    {
                        for (unsigned int i=0;i<lineData.size();i++)
                            m_dataMemory[extAddress+address+i-dataAddress]=lineData[i];
                    }
                    else // PIC16 devices contain useless data after every data byte in data memory :(
                    {
                        for (unsigned int i=0;i<lineData.size();i+=2)
                            m_dataMemory[(extAddress+address+i-dataAddress)/2]=lineData[i];
                    }
                }
                else wxLogError("Data in hex file outside data memory of PIC");
            }
            
            // is the address within the Code Memory range?
            if ((extAddress+address)<(picType->CodeSize))
            {
                if (m_codeMemory.size() < picType->CodeSize)
                {
                    newSize = extAddress+address+lineData.size();
                    if (m_codeMemory.size()<newSize)
                        m_codeMemory.resize(newSize,0xFF);

                    for (unsigned int i=0;i<lineData.size();i++)
                        m_codeMemory[extAddress+address+i]=lineData[i];
                }
                else wxLogError("Data in hex file outside code memory of PIC");
            }
            break;
            
        case EXTADDR:
            extAddress=(lineData[0]<<24)|(lineData[1]<<16);
            break;
            
        case ENDOFFILE:
            break;
            
        default:
            wxLogError("unknown record type: %d", recordType);
            return false;
            break;
        }
    } while (recordType!=ENDOFFILE);

    fp.close();

    // make sure that the config memory array's size is always picType->ConfigSize
    m_configMemory.resize(picType->ConfigSize,0xFF);
    calcConfigMask(picType);    // update m_configMask
    
    trimData(picType);
    m_bModified = false;

    return true;
}

void HexFile::trimData(PicType* picType)
{
    if (picType->is14Bit())
    {
        for (unsigned int i=1;i<m_codeMemory.size();i+=2)
            m_codeMemory[i]&=0x3F;
        for (unsigned int i=1;i<m_configMemory.size();i+=2)
            m_configMemory[i]&=0x3F;
		for (unsigned int i=0;i<m_codeMemory.size();i+=2)
            m_codeMemory[i]&=0xFF;
        for (unsigned int i=0;i<m_configMemory.size();i+=2)
            m_configMemory[i]&=0xFF;
    }
    else
    {
        for (unsigned int i=0;i<m_codeMemory.size();i++)
            m_codeMemory[i]&=0xFF;
        for (unsigned int i=0;i<m_configMemory.size();i++)
            m_configMemory[i]&=0xFF;
    }
    
    for (unsigned int i=1;i<m_dataMemory.size();i++)
        m_dataMemory[i]&=0xFF;

    m_bModified = true;
}

bool HexFile::reload(PicType* picType)
{
    return open(picType, m_filename.c_str());
}

bool HexFile::saveAs(PicType* picType, const char* filename)
{
    ofstream fp (filename);
    int lineSize;
    vector<int> lineData;
    char txt[256];
    int address;

    if (fp==NULL)
    {
        wxLogError("Could not open Hex file for writing...");
        return false;
    }

    m_filename = wxString(filename);

    if (m_codeMemory.size()>0)
    {
        lineData.resize(2);
        //Put address 0x0000 in lineData
        lineData[0]=0x00;
        lineData[1]=0x00;
        makeLine(0,EXTADDR,lineData,txt);
        fp<<txt<<endl;
        for (unsigned int i=0;i<m_codeMemory.size();i+=16)
        {
            if (i+16<m_codeMemory.size())
            {
                lineSize=16;
            }
            else
            {
                lineSize=m_codeMemory.size()-i;
            }
            lineData.resize(lineSize);
            for (int j=0;j<lineSize;j++)
            {
                lineData[j]=m_codeMemory[i+j];
            }
            makeLine(i,DATA,lineData,txt);
            fp<<txt<<endl;
        }
    }
    
    if (m_dataMemory.size()>0)
    {
        lineData.resize(2);
        //Put address DataAddress in lineData
        address=picType->DataAddress;
        if (picType->is14Bit())address*=2;
        lineData[0]=(address>>24)&0xFF;
        lineData[1]=(address>>16)&0xFF;
        makeLine(0,EXTADDR,lineData,txt);
        fp<<txt<<endl;
        for (unsigned int i=0;i<m_dataMemory.size();i+=16)
        {
            if (i+16<m_dataMemory.size())
            {
                lineSize=16;
            }
            else
            {
                lineSize=m_dataMemory.size()-i;
            }
            lineData.resize(lineSize);
            for (int j=0;j<lineSize;j++)
            {
                lineData[j]=m_dataMemory[i+j];
            }
            makeLine(i+(address&0xFFFF),DATA,lineData,txt);
            fp<<txt<<endl;
        }
    }
    
    if (m_configMemory.size()>0)
    {
        lineData.resize(2);
        //Put address DataAddress in lineData
        address=picType->ConfigAddress;
        if (picType->is14Bit())address*=2;
        lineData[0]=(address>>24)&0xFF;
        lineData[1]=(address>>16)&0xFF;
        makeLine(0,EXTADDR,lineData,txt);
        fp<<txt<<endl;
        for (unsigned int i=0;i<m_configMemory.size();i+=16)
        {
            if (i+16<m_configMemory.size())
            {
                lineSize=16;
            }
            else
            {
                lineSize=m_configMemory.size()-i;
            }
            lineData.resize(lineSize);
            for (int j=0;j<lineSize;j++)
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

    return true;
}

bool HexFile::save(PicType* picType)
{
    return saveAs(picType, m_filename.c_str());
}

void HexFile::putMemory(MemoryType type, const vector<int>& mem, const PicType* picType)
{
	switch(type)
	{
		case TYPE_CODE: m_codeMemory=mem;break;
		case TYPE_CONFIG: m_configMemory=mem; calcConfigMask(picType);break;
		case TYPE_DATA: m_dataMemory=mem;break;
	}
	m_bModified = true;
}

void HexFile::putMemory(MemoryType type, int address, int mem, const PicType* picType)
{
	switch(type)
	{
		case TYPE_CODE:
			if (m_codeMemory.size()>(unsigned)address)
			{
				m_codeMemory[address]=mem;
				m_bModified = true;
			}
			break;
		case TYPE_CONFIG:
		    if (m_configMemory.size()<=(unsigned)address) 
			{
				m_configMemory.resize(address+1);
				calcConfigMask(picType);
				m_bModified = true;
			}
			if (m_configMemory.size()>(unsigned)address)
			{
				m_configMemory[address]=mem;
				m_bModified = true;
			}
			break;
		case TYPE_DATA:
			if (m_dataMemory.size()>(unsigned)address)
			{
				m_dataMemory[address]=mem;
				m_bModified = true;
			}
			break;
	}
}

VerifyResult HexFile::verify(MemoryType type, const HexFile* other,bool skipBootSection) const
{
    // NOTE: this function assumes that *this instance is the one containing
    //       data read from the device under test, and the *other instance
    //       is the one containing the expected data

    VerifyResult res;
    res.Result=VERIFY_MISMATCH;
    res.DataType=type;
	unsigned int start=0;

    const vector<int>* arrThis;
    const vector<int>* arrOther;
    switch (type)
    {
    case TYPE_CODE: 
        arrThis=&m_codeMemory; 
        arrOther=&other->m_codeMemory; 
		if(skipBootSection)start=0x800; //the bootloader section ends at 0x800
        break;
    case TYPE_DATA: 
        arrThis=&m_dataMemory; 
        arrOther=&other->m_dataMemory; 
        break;
    case TYPE_CONFIG: 
        arrThis=&m_configMemory; 
        arrOther=&other->m_configMemory; 
        break;
    }

    // ensure that in the loop below we won't exceed any array bound
    for (unsigned int i=start; i<min(arrThis->size(), arrOther->size()); i++)
    {
		switch(type)
		{
			case TYPE_DATA:
			case TYPE_CODE:
        		if ((*arrThis)[i] != (*arrOther)[i])
				{
					res.Address=i;
					res.Read=(*arrThis)[i];
					res.Expected=(*arrOther)[i];
					return res;
				}
				break;
			case TYPE_CONFIG:
				if (((*arrThis)[i]&m_configMask[i]) != ((*arrOther)[i]&m_configMask[i]))
				{
					res.Address=i;
					res.Read=(*arrThis)[i];
					res.Expected=(*arrOther)[i];
					return res;
				}
				break;
		}
    }
    /*
    // It doesn't make sense to compare a larger hex file with zeroes! let's just not compare it with anything...
    if (arrThis->size() > arrOther->size())
    {
        for (unsigned int i=arrOther->size(); i<arrThis->size(); i++)
        {
            if ((*arrThis)[i] != 0)
            {
                res.Address=i;
                res.Read=(*arrThis)[i];
                res.Expected=0;
                return res;
            }
        }
    }
    else if (arrThis->size() < arrOther->size())
    {
        for (unsigned int i=arrThis->size(); i<arrOther->size(); i++)
        {
            if ((*arrOther)[i] != 0)
            {
                res.Address=i;
                res.Read=0;
                res.Expected=(*arrOther)[i];
                return res;
            }
        }
    }*/
    
    res.Result = VERIFY_SUCCESS;
    return res;
}

vector<int>& HexFile::getMemory(MemoryType type)
{
	switch(type)
	{
		case TYPE_CODE:return m_codeMemory;
		case TYPE_DATA:return m_dataMemory;
		case TYPE_CONFIG:return m_configMemory;
	}
}

int HexFile::getMemory(MemoryType type, int address) const
{
	switch(type)
	{
		case TYPE_CODE:
			if ((unsigned)address<m_codeMemory.size())
				return m_codeMemory[address];
			else
				return 0;
		case TYPE_DATA:
			if ((unsigned)address<m_dataMemory.size())
				return m_dataMemory[address];
			else
				return 0;
		case TYPE_CONFIG:
			 if ((unsigned)address<m_configMemory.size())
				return m_configMemory[address];
			else
				return 0;
	}			
}

void HexFile::print(wxString* output,PicType *picType)
{
    int lineSize;
    char txt[256];
    output->append("Code Memory\n");
    for (int i=0;i<(signed)getMemory(TYPE_CODE).size();i+=16)
    {
        sprintf(txt,"%08X::",i);
        output->append(txt);
        if (i+16<(signed)getMemory(TYPE_CODE).size())
        {
            lineSize=16;
        }
        else
        {
            lineSize=getMemory(TYPE_CODE).size()-i;
        }
        for (int j=0;j<lineSize;j++)
        {
            sprintf(txt,"%02X",getMemory(TYPE_CODE)[i+j]);
            output->append(txt);
        }
        output->append("\n");
    }
    output->append("\nConfig Memory\n");
    for (int i=0;i<(signed)getMemory(TYPE_CONFIG).size();i+=16)
    {
        sprintf(txt,"%08X::",i+picType->ConfigAddress);
        output->append(txt);
        if (i+16<(signed)getMemory(TYPE_CONFIG).size())
        {
            lineSize=16;
        }
        else
        {
            lineSize=getMemory(TYPE_CONFIG).size()-i;
        }
        for (int j=0;j<lineSize;j++)
        {
            sprintf(txt,"%02X",getMemory(TYPE_CONFIG)[i+j]);
            output->append(txt);
        }
        output->append("\n");
    }
    output->append("\nData Memory\n");
    for (int i=0;i<(signed)getMemory(TYPE_DATA).size();i+=16)
    {
        sprintf(txt,"%08X::",i);
        output->append(txt);
        if (i+16<(signed)getMemory(TYPE_DATA).size())
        {
            lineSize=16;
        }
        else
        {
            lineSize=getMemory(TYPE_DATA).size()-i;
        }
        for (int j=0;j<lineSize;j++)
        {
            sprintf(txt,"%02X",getMemory(TYPE_DATA)[i+j]);
            output->append(txt);
        }
        output->append("\n");
    }
}


// ----------------------------------------------------------------------------
// HexFile - private functions
// ----------------------------------------------------------------------------

void HexFile::calcConfigMask(const PicType* pic)
{
    m_configMask.resize(m_configMemory.size());
    if (m_configMask.size() == 0)
        return;

    // make sure that later in the for() loop we won't access invalid elements
    // of m_configMask array:
    if (pic->is14Bit())
        //wxASSERT(pic->ConfigWords.size() == m_configMask.size(),
        //         "invalid number of config words for " + pic->Name);
    //else
        wxASSERT_MSG(pic->ConfigWords.size()*2 == m_configMask.size(),
                     "invalid number of config words for " + pic->Name);
    
    // reset m_configMask contents
    for (unsigned int i=0; i<pic->ConfigWords.size(); i++)
    {
        unsigned int mask = pic->ConfigWords[i].GetMask();
        
        if (pic->is16Bit())
        {
            m_configMask[i] = mask;
        }
        else 
        {
            // for 14 and 24 bit devices, each mask need to be saved in two different 
            // elements of the mask array:
            m_configMask[i*2] = mask & 0xFF;
            m_configMask[i*2+1] = (mask>>8) & 0xFF;
        }
    }
}

bool HexFile::calcCheckSum(int byteCount, int address, RecordType recordType,
                           const vector<int>& lineData, int checkSum)
{
    int check=0;
    check+=byteCount;
    check+=(address>>8)&0xFF;
    check+=(address)&0xFF;
    check+=recordType;

    for (unsigned int i=0;i<lineData.size();i++)
        check+=lineData[i];

    check=(0x100-check)&0xFF;

    if (check!=checkSum)
        printf("%2X %2X\n",check,checkSum);
    return (check==checkSum);
}

void HexFile::makeLine(int address, RecordType recordType, const vector<int>& lineData, char* output_line)
{
    int check=0;
    int byteCount=lineData.size();
    check+=byteCount;
    check+=(address>>8)&0xFF;
    check+=(address)&0xFF;
    check+=recordType;
    sprintf(output_line,":%02X%04X%02X",byteCount,address,recordType);
    for (unsigned int i=0;i<lineData.size();i++)
    {
        sprintf(output_line+9+(i*2),"%02X",lineData[i]);
        check+=(lineData[i]&0xFF);
    }

    check=(0x100-check)&0xFF;

    sprintf(output_line+9+(lineData.size()*2),"%02X",check);
}



