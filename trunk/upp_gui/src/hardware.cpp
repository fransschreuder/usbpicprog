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

#include "hardware.h"
#include <iostream>
#include <string.h>

using namespace std;

Hardware::Hardware(  ) 
{
	struct usb_bus *bus;
	struct usb_device *dev;

	usb_init();
	
	usb_find_busses();
	usb_find_devices();
	
	for (bus=usb_get_busses();bus;bus=bus->next)
	{
		for (dev=bus->devices;dev;dev=dev->next)
		{
			
			_handle = usb_open(dev);
			if (!_handle)
			{
				cerr<<"Warning !! cannot open device"<<endl;
				continue;
			}
			if ((dev->descriptor.idVendor == VENDOR) && (dev->descriptor.idProduct == PRODUCT) )
				break;
			usb_close(_handle);
			_handle=NULL;
		}
			if(_handle!=NULL)
				break;
	}
	if (!_handle)
		cerr<<"Warning !! Device not found"<<endl;
}


Hardware::~Hardware()
{
	if(_handle!=NULL)
	{
		usb_close(_handle);
		_handle=NULL;
	}
}

int Hardware::readCode(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	nBytes=0;
	vector<int> mem;
	mem.resize(picType->getCurrentPic().CodeSize);
	char dataBlock[BLOCKSIZE_CODE];
	int blocktype;
	for(int blockcounter=0;blockcounter<picType->getCurrentPic().CodeSize;blockcounter+=BLOCKSIZE_CODE)
	{
		blocktype=BLOCKTYPE_MIDDLE;
		if(blockcounter=0)blocktype|=BLOCKTYPE_FIRST;
		if((picType->getCurrentPic().CodeSize-BLOCKSIZE_CODE)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
		nBytes+=readCodeBlock(dataBlock,blockcounter,BLOCKSIZE_CODE,blocktype);
		for(int i=0;i<BLOCKSIZE_CODE;i++)
		{
			if(picType->getCurrentPic().CodeSize>(blockcounter+i))
			{
				mem[blockcounter+i]=dataBlock[i];
			}
			else
			{
				cerr<<"Trying to read memory outside Code area"<<endl;
				return -1;
			}
		}
			
		
	}
	hexData->putCodeMemory(mem);
	return nBytes;
}

int Hardware::writeCode(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	nBytes=0;
	char dataBlock[BLOCKSIZE_CODE];
	int blocktype;
	for(int blockcounter=0;blockcounter<hexData->getCodeMemory().size();blockcounter+=BLOCKSIZE_CODE)
	{
		for(int i=0;i<BLOCKSIZE_CODE;i++)
		{
			if(hexData->getCodeMemory().size()>(blockcounter+i))
			{
				dataBlock[i]=hexData->getCodeMemory()[blockcounter+i];
			}
			else
			{
				dataBlock[i]=0;
			}
		}
			
		blocktype=BLOCKTYPE_MIDDLE;
		if(blockcounter==0)blocktype|=BLOCKTYPE_FIRST;
		if((hexData->getCodeMemory().size()-BLOCKSIZE_CODE)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
		nBytes=writeCodeBlock(dataBlock,blockcounter,BLOCKSIZE_CODE,blocktype);
	}
	return nBytes;
}

int Hardware::readData(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	nBytes=0;
	vector<int> mem;
	mem.resize(picType->getCurrentPic().DataSize);
	char dataBlock[BLOCKSIZE_DATA];
	int blocktype;
	for(int blockcounter=0;blockcounter<picType->getCurrentPic().DataSize;blockcounter+=BLOCKSIZE_DATA)
	{
		blocktype=BLOCKTYPE_MIDDLE;
		if(blockcounter==0)blocktype|=BLOCKTYPE_FIRST;
		if((picType->getCurrentPic().DataSize-BLOCKSIZE_DATA)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
		nBytes+=readDataBlock(dataBlock,blockcounter,BLOCKSIZE_DATA,blocktype);
		for(int i=0;i<BLOCKSIZE_DATA;i++)
		{
			if(picType->getCurrentPic().DataSize>(blockcounter+i))
			{
				mem[blockcounter+i]=dataBlock[i];
			}
			else
			{
				cerr<<"Trying to read memory outside Data area"<<endl;
				return -1;
			}
		}
			
		
	}
	hexData->putDataMemory(mem);
	return nBytes;

}

int Hardware::writeData(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	nBytes=0;
	char dataBlock[BLOCKSIZE_DATA];
	int blocktype;
	for(int blockcounter=0;blockcounter<hexData->getDataMemory().size();blockcounter+=BLOCKSIZE_DATA)
	{
		for(int i=0;i<BLOCKSIZE_DATA;i++)
		{
			if(hexData->getDataMemory().size()>(blockcounter+i))
			{
				dataBlock[i]=hexData->getDataMemory()[blockcounter+i];
			}
			else
			{
				dataBlock[i]=0;
			}
		}
			
		blocktype=BLOCKTYPE_MIDDLE;
		if(blockcounter==0)blocktype|=BLOCKTYPE_FIRST;
		if((hexData->getDataMemory().size()-BLOCKSIZE_DATA)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
		nBytes+=writeDataBlock(dataBlock,blockcounter,BLOCKSIZE_DATA,blocktype);
	}
	return nBytes;
}

int Hardware::readConfig(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	nBytes=0;
	vector<int> mem;
	mem.resize(picType->getCurrentPic().ConfigSize);
	char dataBlock[BLOCKSIZE_CONFIG];
	int blocktype;
	for(int blockcounter=0;blockcounter<picType->getCurrentPic().ConfigSize;blockcounter+=BLOCKSIZE_CONFIG)
	{
		blocktype=BLOCKTYPE_MIDDLE;
		if(blockcounter==0)blocktype|=BLOCKTYPE_FIRST;
		if((picType->getCurrentPic().ConfigSize-BLOCKSIZE_CONFIG)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
		nBytes+=readCodeBlock(dataBlock,blockcounter+picType->getCurrentPic().ConfigAddress,BLOCKSIZE_CONFIG,blocktype);
		for(int i=0;i<BLOCKSIZE_CONFIG;i++)
		{
			if(picType->getCurrentPic().ConfigSize>(blockcounter+i))
			{
				mem[blockcounter+i]=dataBlock[i];
			}
			else
			{
				cerr<<"Trying to read memory outside Config area"<<endl;
				return -1;
			}
		}
			
		
	}
	hexData->putConfigMemory(mem);
	return nBytes;
}

int Hardware::writeConfig(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	nBytes=0;
	char dataBlock[BLOCKSIZE_CONFIG];
	int blocktype;
	for(int blockcounter=0;blockcounter<hexData->getConfigMemory().size();blockcounter+=BLOCKSIZE_CONFIG)
	{
		for(int i=0;i<BLOCKSIZE_CONFIG;i++)
		{
			if(hexData->getConfigMemory().size()>(blockcounter+i))
			{
				dataBlock[i]=hexData->getConfigMemory()[blockcounter+i];
			}
			else
			{
				dataBlock[i]=0;
			}
		}
			
		blocktype=BLOCKTYPE_MIDDLE;
		if(blockcounter==0)blocktype|=BLOCKTYPE_FIRST;
		if((hexData->getConfigMemory().size()-BLOCKSIZE_CONFIG)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
		nBytes+=writeConfigBlock(dataBlock,blockcounter+picType->getCurrentPic().ConfigAddress,BLOCKSIZE_CONFIG,blocktype);
	}
	return nBytes;
}


int Hardware::autoDetectDevice(void)
{
	return readId();
	
}

int Hardware::bulkErase(void)
{
	char msg[64];
	
	msg[0]=CMD_ERASE;
	int nBytes;
	if (_handle !=NULL)
	{
		if(writeString(msg,1)<0)
		{
			return 0;
		}
		nBytes = readString(msg);
			
		if (nBytes < 0 )
		{
			cerr<<"Usb Error"<<endl;
			return nBytes;
		}
		else
		{
			return (int)msg[0];
		}
	}
	
}


int Hardware::readId(void)
{
	char msg[64];
	
	msg[0]=CMD_READ_ID;
	int nBytes;
	if (_handle !=NULL)
	{
		if(writeString(msg,1)<0)
		{
			return 0;
		}
		nBytes = readString(msg);
			
		if (nBytes < 0 )
		{
			cerr<<"Usb Error"<<endl;
			return 0;
		}
		else
		{
			//cout<<"Id: "<<hex<<((int)msg[0]&0xFF)<<" "<<hex<<((int)msg[1]&0xFF)<<", "<<nBytes<<" bytes"<<endl;
			return ((((int)msg[1])&0xFF)<<8)|(((int)msg[0])&0xFF);
			
		}
	}
	
}

int Hardware::setPicType(PicType* picType)
{
	char msg[64];
	
	msg[0]=CMD_SET_PICTYPE;
	msg[1]=picType->getCurrentPic().picFamily;
	int nBytes;
	if (_handle !=NULL)
	{
		if(writeString(msg,2)<0)
		{
			return 0;
		}
		nBytes = readString(msg);
			
		if (nBytes < 0 )
		{
			cerr<<"Usb Error"<<endl;
			return 0;
		}
		else
		{
			//cout<<"Id: "<<hex<<((int)msg[0]&0xFF)<<" "<<hex<<((int)msg[1]&0xFF)<<", "<<nBytes<<" bytes"<<endl;
			return (int)msg[0];
			
		}
	}
	
}

int Hardware::readCodeBlock(char * msg,int address,int size,int lastblock)
{
	UppPackage uppPackage;
	if (_handle !=NULL)
	{
		uppPackage.fields.cmd=CMD_READ_CODE;
		uppPackage.fields.size=size;
		uppPackage.fields.addrU=(char)((address>>16)&0xFF);
		uppPackage.fields.addrH=(char)((address>>8)&0xFF);
		uppPackage.fields.addrL=(char)(address&0xFF);
		uppPackage.fields.blocktype=(char)lastblock;
		
		
	    
		int nBytes = writeString(uppPackage.data,6);
		if (nBytes < 0 )
		{
			return nBytes;
		}
			
		nBytes = readString(msg);
		if (nBytes < 0 )
			cerr<<"Usb Error"<<endl;
		return nBytes;
	}
	else return -1;
}

int Hardware::writeCodeBlock(char * msg,int address,int size,int lastblock)
{
	char resp_msg[10];
	UppPackage uppPackage;
	if (_handle !=NULL)
	{
		uppPackage.fields.cmd=CMD_WRITE_CODE;
		uppPackage.fields.size=size;
		uppPackage.fields.addrU=(char)((address>>16)&0xFF);
		uppPackage.fields.addrH=(char)((address>>8)&0xFF);
		uppPackage.fields.addrL=(char)(address&0xFF);
		uppPackage.fields.blocktype=(char)lastblock;
		strncpy(uppPackage.fields.dataField,msg,size);
		int nBytes = writeString(uppPackage.data,size+6);
		if (nBytes < 0 )
		{
			return nBytes;
		}
			
		nBytes = readString(resp_msg);
		if (nBytes < 0 )
			cerr<<"Usb Error"<<endl;
		return (int)resp_msg[0];
	}
	else return -1;
	
}

int Hardware::writeConfigBlock(char * msg,int address,int size,int lastblock)
{
	char resp_msg[10];
	UppPackage uppPackage;
	if (_handle !=NULL)
	{
		uppPackage.fields.cmd=CMD_WRITE_CONFIG;
		uppPackage.fields.size=size;
		uppPackage.fields.addrU=(char)((address>>16)&0xFF);
		uppPackage.fields.addrH=(char)((address>>8)&0xFF);
		uppPackage.fields.addrL=(char)(address&0xFF);
		uppPackage.fields.blocktype=(char)lastblock;
		strncpy(uppPackage.fields.dataField,msg,size);
		int nBytes = writeString(uppPackage.data,size+6);
		if (nBytes < 0 )
		{
			return nBytes;
		}
			
		nBytes = readString(resp_msg);
		if (nBytes < 0 )
			cerr<<"Usb Error"<<endl;
		return (int)resp_msg[0];
	}
	else return -1;
	
}



int Hardware::readDataBlock(char * msg,int address,int size,int lastblock)
{
	UppPackage uppPackage;
	if (_handle !=NULL)
	{
		uppPackage.fields.cmd=CMD_READ_DATA;
		uppPackage.fields.size=size;
		uppPackage.fields.addrU=0;
		uppPackage.fields.addrH=(char)((address>>8)&0xFF);
		uppPackage.fields.addrL=(char)(address&0xFF);
		uppPackage.fields.blocktype=(char)lastblock;
		int nBytes = writeString(uppPackage.data,6);
		if (nBytes < 0 )
		{
			return nBytes;
		}
			
		nBytes = readString(msg);
		if (nBytes < 0 )
			cerr<<"Usb Error"<<endl;
		return nBytes;
	}
	else return -1;
	
}


int Hardware::writeDataBlock(char * msg,int address,int size,int lastblock)
{
	char resp_msg[10];
	UppPackage uppPackage;
	if (_handle !=NULL)
	{
		uppPackage.fields.cmd=CMD_WRITE_DATA;
		uppPackage.fields.size=size;
		uppPackage.fields.addrU=0;
		uppPackage.fields.addrH=(char)((address>>8)&0xFF);
		uppPackage.fields.addrL=(char)(address&0xFF);
		uppPackage.fields.blocktype=(char)lastblock;
		strncpy(uppPackage.fields.dataField,msg,size);
		int nBytes = writeString(uppPackage.data,size+6);
		if (nBytes < 0 )
		{
			return nBytes;
		}
			
		nBytes = readString(resp_msg);
		if (nBytes < 0 )
			cerr<<"Usb Error"<<endl;
		return (int)resp_msg[0];
	}
	else return -1;	
}

bool Hardware::connected(void) 
{
		if (_handle == NULL)
			return 0;
		else
			return 1;
}


int Hardware::writeString(const char * msg,int size)
{
	int nBytes;
	if (_handle != NULL)
	{
		for(int i=0;i<size;i++)printf("%2X ",msg[i]&0xFF);
		cout<<endl;
		nBytes = usb_interrupt_write(_handle,1,(char*)msg,size,5000);
		if (nBytes < 0 )
			cerr<<"Usb Error while writing to device"<<endl;
		
	}
	else 
		cerr<<"Error: Not connected"<<endl;
	return nBytes;
}

int Hardware::readString(char* msg)
{
	int nBytes = usb_interrupt_read(_handle,1,(char*)msg,64,5000);
		if (nBytes < 0 )
		{
			cerr<<"Usb Error"<<endl;
		
		}
		return nBytes;
		
}

const char * Hardware::readResponse(void)
{
	
	char msg[64];
	int nBytes = readString(msg);
		if (nBytes < 0 )
		{
			cerr<<"Usb Error"<<endl;
			return NULL;
		}
			
	else
	{
		switch(msg[0])
		{
			case 1:
				return "Received: 1, Ok";
				break;
			case 2:
				return "Received: 2, asked for next block";
				break;
			case 3:
				return "Error, PIC not supported for this action";
				break;
			default:
				return "Received unexpected response";
				break;	
		}
			
	}
}

//
