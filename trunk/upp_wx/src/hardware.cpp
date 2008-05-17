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
#include "uppmainwindow_callback.h"

using namespace std;

/*The class Hardware connects to usbpicprog using libusb. The void* CB points 
 to the parent UppMainWindowCallBack which is used for updating the progress
 bar. If initiated with no argument, progress is not updated*/
Hardware::Hardware(void* CB) 
{
	struct usb_bus *bus;
	struct usb_device *dev;
	usb_init();
	ptCallBack=CB;
	usb_find_busses();
	usb_find_devices();
	
	for (bus=usb_get_busses();bus;bus=bus->next)
	{
		for (dev=bus->devices;dev;dev=dev->next)
		{
			
			_handle = usb_open(dev);
			if (!_handle)continue; //failed to open this device, choose the next one
			if ((dev->descriptor.idVendor == VENDOR) && (dev->descriptor.idProduct == PRODUCT) )
				break; //found usbpicprog, exit the for loop
			usb_close(_handle);
			_handle=NULL;
		}
			if(_handle!=NULL)	//successfully initialized? don't try any other buses
				break;
	}
	
}

Hardware::~Hardware()
{
	if(_handle!=NULL)
	{
		usb_close(_handle);
		_handle=NULL;
	}
}

/*give the hardware the command to switch to a certain pic algorithm*/
int Hardware::setPicType(PicType* picType)
{
	char msg[64];
	statusCallBack (0);
	msg[0]=CMD_SET_PICTYPE;
	msg[1]=picType->getCurrentPic().picFamily;
	int nBytes=-1;
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
			return nBytes;
		}
		else
		{
			//cout<<"Id: "<<hex<<((int)msg[0]&0xFF)<<" "<<hex<<((int)msg[1]&0xFF)<<", "<<nBytes<<" bytes"<<endl;
			return (int)msg[0];
			statusCallBack (100);

		}
	}
	return nBytes;
}

/*Erase all the contents (code, data and config) of the pic*/
int Hardware::bulkErase(void)
{
	char msg[64];

	msg[0]=CMD_ERASE;
	int nBytes=-1;
	statusCallBack (0);
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
			statusCallBack (100);
		}
	}
	return nBytes;
}

/*Read the code memory from the pic (starting from address 0 into *hexData*/
int Hardware::readCode(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	nBytes=-1;
	vector<int> mem;
	mem.resize(picType->getCurrentPic().CodeSize);
	char dataBlock[BLOCKSIZE_CODE];
	int blocktype;
	statusCallBack (0);
	if (_handle !=NULL)
	{
		for(int blockcounter=0;blockcounter<picType->getCurrentPic().CodeSize;blockcounter+=BLOCKSIZE_CODE)
		{
			statusCallBack ((blockcounter*100)/((signed)picType->getCurrentPic().CodeSize));
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
	}
	return nBytes;
}

/* Write the code memory area of the pic with the data in *hexData */
int Hardware::writeCode(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	nBytes=-1;
	char dataBlock[BLOCKSIZE_CODE];
	int blocktype;
	if (_handle !=NULL)
	{
		for(int blockcounter=0;blockcounter<(signed)hexData->getCodeMemory().size();blockcounter+=BLOCKSIZE_CODE)
		{
			statusCallBack ((blockcounter*100)/((signed)hexData->getCodeMemory().size()));
			for(int i=0;i<BLOCKSIZE_CODE;i++)
			{
				if((signed)hexData->getCodeMemory().size()>(blockcounter+i))
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
			if(((signed)hexData->getCodeMemory().size()-BLOCKSIZE_CODE)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
			nBytes=writeCodeBlock(dataBlock,blockcounter,BLOCKSIZE_CODE,blocktype);
		}
	}
	return nBytes;
}

/* read the Eeprom Data area of the pic into *hexData->dataMemory */
int Hardware::readData(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	nBytes=-1;
	vector<int> mem;
	mem.resize(picType->getCurrentPic().DataSize);
	char dataBlock[BLOCKSIZE_DATA];
	int blocktype;
	statusCallBack (0);
	if (_handle !=NULL)
	{
		for(int blockcounter=0;blockcounter<picType->getCurrentPic().DataSize;blockcounter+=BLOCKSIZE_DATA)
		{
			statusCallBack ((blockcounter*100)/((signed)picType->getCurrentPic().DataSize));
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
	}
	return nBytes;

}


/*write the Eeprom data from *hexData->dataMemory into the pic*/
int Hardware::writeData(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	nBytes=-1;
	char dataBlock[BLOCKSIZE_DATA];
	int blocktype;
	statusCallBack (0);
	if (_handle !=NULL)
	{
		for(int blockcounter=0;blockcounter<(signed)hexData->getDataMemory().size();blockcounter+=BLOCKSIZE_DATA)
		{
			statusCallBack ((blockcounter*100)/((signed)hexData->getDataMemory().size()));
			for(int i=0;i<BLOCKSIZE_DATA;i++)
			{
				if((signed)hexData->getDataMemory().size()>(blockcounter+i))
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
			if(((signed)hexData->getDataMemory().size()-BLOCKSIZE_DATA)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
			nBytes+=writeDataBlock(dataBlock,blockcounter,BLOCKSIZE_DATA,blocktype);
		}
	}
	return nBytes;
}

/* Read the configuration words (and user ID's for PIC16 dev's) */
int Hardware::readConfig(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	nBytes=-1;
	vector<int> mem;
	mem.resize(picType->getCurrentPic().ConfigSize);
	char dataBlock[BLOCKSIZE_CONFIG];
	int blocktype;
	statusCallBack (0);
	if (_handle !=NULL)
	{
		for(int blockcounter=0;blockcounter<picType->getCurrentPic().ConfigSize;blockcounter+=BLOCKSIZE_CONFIG)
		{
			statusCallBack ((blockcounter*100)/((signed)picType->getCurrentPic().ConfigSize));
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
	}
	return nBytes;
}

/*Writes the configuration words (and user ID's for PIC16 dev's)*/
int Hardware::writeConfig(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	nBytes=-1;
	char dataBlock[BLOCKSIZE_CONFIG];
	int blocktype;
	statusCallBack (0);
	if (_handle !=NULL)
	{
		for(int blockcounter=0;blockcounter<(signed)hexData->getConfigMemory().size();blockcounter+=BLOCKSIZE_CONFIG)
		{
			statusCallBack ((blockcounter*100)/((signed)hexData->getConfigMemory().size()));
			for(int i=0;i<BLOCKSIZE_CONFIG;i++)
			{
				if((signed)hexData->getConfigMemory().size()>(blockcounter+i))
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
			if(((signed)hexData->getConfigMemory().size()-BLOCKSIZE_CONFIG)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
			nBytes+=writeConfigBlock(dataBlock,blockcounter+picType->getCurrentPic().ConfigAddress,BLOCKSIZE_CONFIG,blocktype);
		}
	}
	return nBytes;
}

/*Reads the whole PIC and checks if the data matches hexData*/
VerifyResult Hardware::verify(ReadHexFile *hexData, PicType *picType)
{
    VerifyResult res;
    ReadHexFile *verifyHexFile = new ReadHexFile;
    if(readCode(verifyHexFile,picType)<0)
	{
        res.Result=VERIFY_USB_ERROR;
        res.DataType=TYPE_CODE;
        return res;
    }
	if(readData(verifyHexFile,picType)<0)
	{
        res.Result=VERIFY_USB_ERROR;
        res.DataType=TYPE_DATA;
        return res;
    }
	if(readConfig(verifyHexFile,picType)<0)
	{
        res.Result=VERIFY_USB_ERROR;
        res.DataType=TYPE_CONFIG;
        return res;
    }
    if ((hexData->getCodeMemory().size()+
        hexData->getDataMemory().size()+
        hexData->getConfigMemory().size())>0) //there should be at least some data in the file
    {
        for(int i=0;i<(signed)hexData->getCodeMemory().size();i++)
        {
            if((signed)verifyHexFile->getCodeMemory().size()<(i+1))
            {
                res.Result=VERIFY_OTHER_ERROR;
                return res;
            }
            if(verifyHexFile->getCodeMemory()[i]!=hexData->getCodeMemory()[i])
            {
                res.Result=VERIFY_MISMATCH;
                res.DataType=TYPE_CODE;
                res.Address=i;
                res.Read=verifyHexFile->getCodeMemory()[i];
                res.Expected=hexData->getCodeMemory()[i];
                return res;
            }

        }
        for(int i=0;i<(signed)hexData->getDataMemory().size();i++)
        {
            if((signed)verifyHexFile->getDataMemory().size()<(i+1))
            {
                res.Result=VERIFY_OTHER_ERROR;              
                return res;
            }
            if(verifyHexFile->getDataMemory()[i]!=hexData->getDataMemory()[i])
            {
                res.Result=VERIFY_MISMATCH;
                res.DataType=TYPE_DATA;
                res.Address=i;
                res.Read=verifyHexFile->getDataMemory()[i];
                res.Expected=hexData->getDataMemory()[i];
                return res;
            }

        }
        for(int i=0;i<(signed)hexData->getConfigMemory().size();i++)
        {
            if((signed)verifyHexFile->getConfigMemory().size()<(i+1))
            {
                res.Result=VERIFY_OTHER_ERROR;                
                return res;
            }
            if(verifyHexFile->getConfigMemory()[i]!=hexData->getConfigMemory()[i])
            {
                res.Result=VERIFY_MISMATCH;
                res.DataType=TYPE_CONFIG;
                res.Address=i;
                res.Read=verifyHexFile->getConfigMemory()[i];
                res.Expected=hexData->getConfigMemory()[i];
                return res;
            }

        }
        res.Result=VERIFY_SUCCESS;
    }
    else
    {
      res.Result=VERIFY_OTHER_ERROR;
    }
    return res;
}

/*Reads the whole PIC and checks if it is blank*/
VerifyResult Hardware::blankCheck(PicType *picType)
{
    ReadHexFile* blankHexFile=new ReadHexFile(picType);
    return verify(blankHexFile,picType);
}


/*This function does nothing but reading the devid from the PIC, call it the following way:
 
	 Hardware* hardware=new Hardware();
	 int devId=hardware->autoDetectDevice();
	 PicType* picType=new PicType(devId);
	 hardware->setPicType(picType);
	 
 */
int Hardware::autoDetectDevice(void)
{
	return readId();
}

/*check if usbpicprog is successfully connected to the usb bus and initialized*/
bool Hardware::connected(void)
{
		if (_handle == NULL)
			return 0;
		else
			return 1;
}

/*Return a string containing the firmware version of usbpicprog*/
int Hardware::getFirmwareVersion(char* msg)
{
	msg[0]=CMD_FIRMWARE_VERSION;
	int nBytes=-1;
	statusCallBack (0);
	if (_handle !=NULL)
	{
		if(writeString(msg,1)<0)
		{
			return -1;
		}
		nBytes = readString(msg);

		if (nBytes < 0 )
		{
			return nBytes;
		}
		else
		{
			statusCallBack (100);
		}
	}
	return nBytes;
}

/*read a string of data from usbpicprog (through interrupt_read)*/
int Hardware::readString(char* msg)
{
	int nBytes = usb_interrupt_read(_handle,READ_ENDPOINT,(char*)msg,64,5000);
		if (nBytes < 0 )
		{
			cerr<<"Usb Error"<<endl;
			return -1;
		}
		return nBytes;

}

/*Send a string of data to usbpicprog (through interrupt write)*/
int Hardware::writeString(const char * msg,int size)
{
	int nBytes=0;
	if (_handle != NULL)
	{
		//for(int i=0;i<size;i++)printf("%2X ",msg[i]&0xFF);
		//cout<<endl;

		nBytes = usb_interrupt_write(_handle,WRITE_ENDPOINT,(char*)msg,size,5000);
		
		if (nBytes < size )
			cerr<<"Usb Error while writing to device: "<<nBytes<<" bytes"<<endl;

	}
	else
	{
		cerr<<"Error: Not connected"<<endl;
		return -1;
	}
	return nBytes;
}

/*Private function called by autoDetectDevice */
int Hardware::readId(void)
{
	char msg[64];
	
	msg[0]=CMD_READ_ID;
	int nBytes=-1;
	statusCallBack (0);
	if (_handle !=NULL)
	{
		if(writeString(msg,1)<0)
		{
			return -1;
		}
		nBytes = readString(msg);
			
		if (nBytes < 0 )
		{
			return -1;
		}
		else
		{
   			statusCallBack (100);
			return ((((int)msg[1])&0xFF)<<8)|(((int)msg[0])&0xFF);
		}
	}
	return nBytes;
}



/*private function to read one block of code memory*/
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

/*private function to write one block of code memory*/
int Hardware::writeCodeBlock(char * msg,int address,int size,int lastblock)
{
	char resp_msg[64];
	UppPackage uppPackage;
	if (_handle !=NULL)
	{
		uppPackage.fields.cmd=CMD_WRITE_CODE;
		uppPackage.fields.size=size;
		uppPackage.fields.addrU=(char)((address>>16)&0xFF);
		uppPackage.fields.addrH=(char)((address>>8)&0xFF);
		uppPackage.fields.addrL=(char)(address&0xFF);
		uppPackage.fields.blocktype=(char)lastblock;
		memcpy(uppPackage.fields.dataField,msg,size);
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

/*private function to write one block of config memory*/
int Hardware::writeConfigBlock(char * msg,int address,int size,int lastblock)
{
	char resp_msg[64];
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


/*private function to read one block of data memory*/
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

/*private function to write one block of data memory*/
int Hardware::writeDataBlock(char * msg,int address,int size,int lastblock)
{
	char resp_msg[64];
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

/*When Hardware is constructed, ptCallBack is initiated by a pointer
 to UppMainWindowCallBack, this function calls the callback function
 to update the progress bar*/
void Hardware::statusCallBack(int value)
{
	if(ptCallBack!=NULL)
	{
		UppMainWindowCallBack* CB=(UppMainWindowCallBack*)ptCallBack;
		CB->updateProgress(value);
	}
}


