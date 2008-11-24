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
#include "uppmainwindow.h"


using namespace std;



#define USB_DEBUG 2
/*The class Hardware connects to usbpicprog using libusb. The void* CB points
 to the parent UppMainWindowCallBack which is used for updating the progress
 bar. If initiated with no argument, progress is not updated*/
Hardware::Hardware(void* CB, HardwareType SetHardware)
{
	struct usb_bus *bus=NULL;
	struct usb_device *dev=NULL;
	int hwtype = SetHardware;

	_handle=NULL;

	usb_init();
	ptCallBack=CB;
	usb_find_busses();
	usb_find_devices();
#ifdef USB_DEBUG
	cout<<"USB debug enabled, remove #define USB_DEBUG 10 in hardware.cpp to disable it"<<endl;
	usb_set_debug(USB_DEBUG);
#endif

	for (bus=usb_get_busses();bus;bus=bus->next)
	{
		for (dev=bus->devices;dev;dev=dev->next)
		{


			if (hwtype == HW_UPP)
			{
				if ((dev->descriptor.idVendor == UPP_VENDOR) && (dev->descriptor.idProduct == UPP_PRODUCT) )
				{
					_handle = usb_open(dev);
					if (!_handle)continue; //failed to open this device, choose the next one
					hwtype=HW_UPP;
					break; //found usbpicprog, exit the for loop
				}
				if ((dev->descriptor.idVendor == BOOTLOADER_VENDOR) && (dev->descriptor.idProduct == BOOTLOADER_PRODUCT) )
				{
					_handle = usb_open(dev);
					if (!_handle)continue; //failed to open this device, choose the next one

					hwtype=HW_BOOTLOADER;
					break; //found bootloader , exit the for loop
				}
			}
			else
			{
				if ((dev->descriptor.idVendor == BOOTLOADER_VENDOR) && (dev->descriptor.idProduct == BOOTLOADER_PRODUCT) )
				{
					_handle = usb_open(dev);
					if (!_handle)continue; //failed to open this device, choose the next one

					hwtype=HW_BOOTLOADER;
					break; //found bootloader , exit the for loop
				}
				if ((dev->descriptor.idVendor == UPP_VENDOR) && (dev->descriptor.idProduct == UPP_PRODUCT) )
				{
					_handle = usb_open(dev);
					if (!_handle)continue; //failed to open this device, choose the next one

					hwtype=HW_UPP;
					break; //found usbpicprog, exit the for loop
				}
			}
			_handle=NULL;
		}
		if(_handle!=NULL)	//successfully initialized? don't try any other buses
			break;
	}

	if(_handle==NULL)
	{
		CurrentHardware = HW_NONE;
	}
	else
	{
		CurrentHardware = (HardwareType)hwtype;
	}
	if(_handle!=NULL)
	{

        #ifndef __WXMSW__
		if (usb_reset(_handle) < 0)
			cerr<<"Couldn't reset interface"<<endl;
		usb_close(_handle);
		_handle = usb_open(dev);
		#endif
		if(!_handle){
			CurrentHardware = HW_NONE;
			return;
		}
#if 0
		tryToDetachDriver();
#endif
		int _config=1;
		int _interface=0;
		int i;
		for (i=0; i<dev->descriptor.bNumConfigurations; i++)
			if ( _config==dev->config[i].bConfigurationValue ) break;
		if ( i==dev->descriptor.bNumConfigurations )
		{
			i = 0;
			_config = dev->config[i].bConfigurationValue;
	  	}
		const usb_config_descriptor &configd = dev->config[i];
		if ( usb_set_configuration(_handle, _config)<0 )
		{
			cerr<<"Error setting configuration"<<endl;
			CurrentHardware = HW_NONE;
			_handle=NULL;
			return;
		}

		for (i=0; i<configd.bNumInterfaces; i++)
			if ( _interface==configd.interface[i].altsetting[0].bInterfaceNumber ) break;
		if ( i==configd.bNumInterfaces )
		{
			int old = _interface;
			i = 0;
			_interface = configd.interface[i].altsetting[0].bInterfaceNumber;
			cerr<<"Interface "<<old<<" not present: using "<<_interface<<endl;
		}
		privateInterface = &(configd.interface[i].altsetting[0]);
        if ( usb_claim_interface(_handle, _interface)<0 )
        {
			cerr<<"Error claiming interface"<<endl;
			CurrentHardware = HW_NONE;
			_handle=NULL;
			return;
		}
	}
}

Hardware::EndpointMode Hardware::endpointMode(int ep)
{
	int index = ep & USB_ENDPOINT_ADDRESS_MASK;
	const usb_endpoint_descriptor *ued = privateInterface->endpoint + index;
	switch (ued->bmAttributes & USB_ENDPOINT_TYPE_MASK)
	{
		case USB_ENDPOINT_TYPE_BULK: return Bulk;
		case USB_ENDPOINT_TYPE_INTERRUPT: return Interrupt;
		case USB_ENDPOINT_TYPE_ISOCHRONOUS: return Isochronous;
		case USB_ENDPOINT_TYPE_CONTROL: return Control;
		default: break;
	}
	return Nb_EndpointModes;
}

Hardware::~Hardware()
{
	statusCallBack (0);

	if(_handle)
	{
		usb_release_interface(_handle, bInterfaceNumber);
		usb_close(_handle);
		_handle=NULL;
	}
}

HardwareType Hardware::getHardwareType(void)
{
	return CurrentHardware;
}

void Hardware::tryToDetachDriver()
{
	// try to detach an already existing driver... (linux only)
	#if defined(LIBUSB_HAS_GET_DRIVER_NP) && LIBUSB_HAS_GET_DRIVER_NP
	//  log(Log::DebugLevel::Extra, "find if there is already an installed driver");
	char dname[256] = "";
	if ( usb_get_driver_np(_handle, bInterfaceNumber, dname, 255)<0 ) return;
	//  log(Log::DebugLevel::Normal, QString("  a driver \"%1\" is already installed...").arg(dname));
	#if defined(LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP) && LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
	usb_detach_kernel_driver_np(_handle, bInterfaceNumber);
	// log(Log::DebugLevel::Normal, "  try to detach it...");
	if ( usb_get_driver_np(_handle, bInterfaceNumber, dname, 255)<0 ) return;
	// log(Log::DebugLevel::Normal, "  failed to detach it");
	#endif
	#endif
}

/*give the hardware the command to switch to a certain pic algorithm*/
int Hardware::setPicType(PicType* picType)
{
	char msg[64];

	if (CurrentHardware == HW_BOOTLOADER) return 0;

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

		nBytes = readString(msg,1);

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
int Hardware::bulkErase(PicType* picType)
{
	char msg[64];
	int nBytes=-1;
	statusCallBack (0);
	if (_handle !=NULL)
	{
        if (CurrentHardware == HW_UPP)
        {
          	msg[0]=CMD_ERASE;
    		if(writeString(msg,1)<0)
    		{
    			return 0;
    		}
    		nBytes = readString(msg,1);

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
        else //hardware is HW_BOOTLOADER
        {
            ReadHexFile * hf = new ReadHexFile(picType);
            if(writeData(hf,picType)<0)return -1;
            statusCallBack(50);
            delete hf;
            for (int address=0x800; address<picType->getCurrentPic().CodeSize; address+=64)
            {
    			BootloaderPackage bootloaderPackage;
    			bootloaderPackage.fields.cmd=CMD_BOOT_ERASE;
    			bootloaderPackage.fields.size=1; //only one block of 64 bytes supported by bootloader
    			bootloaderPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
    			bootloaderPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
    			bootloaderPackage.fields.addrL=(unsigned char)(address&0xFF);
    			if (writeString(bootloaderPackage.data,5) < 0 )
    			{
    				return -1;
    			}
    			if(readString(msg,5)<0)return -1;
                statusCallBack(100);
            }
            return 1;
        }
	}
	return nBytes;
}

/*Read the code memory from the pic (starting from address 0 into *hexData*/
int Hardware::readCode(ReadHexFile *hexData,PicType *picType)
{
	int nBytes,blocksize,BLOCKSIZE_HW;
	nBytes=-1;
	vector<int> mem;
	mem.resize(picType->getCurrentPic().CodeSize, 0xFF);
	char dataBlock[BLOCKSIZE_CODE];
	int blocktype;
	if (CurrentHardware == HW_BOOTLOADER)BLOCKSIZE_HW=BLOCKSIZE_BOOTLOADER;
	else BLOCKSIZE_HW=BLOCKSIZE_CODE;
	statusCallBack (0);
	if (_handle !=NULL)
	{
		nBytes=0;
		for(int blockcounter=0;blockcounter<picType->getCurrentPic().CodeSize;blockcounter+=BLOCKSIZE_HW)
		{
			statusCallBack ((blockcounter*100)/((signed)picType->getCurrentPic().CodeSize));
			blocktype=BLOCKTYPE_MIDDLE;
			if(blockcounter==0)blocktype|=BLOCKTYPE_FIRST;
			if((picType->getCurrentPic().CodeSize-BLOCKSIZE_HW)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
			int	currentBlockCounter=blockcounter;
			if(picType->getCurrentPic().Name.find("P18F")!=0)currentBlockCounter/=2;
			if(picType->getCurrentPic().CodeSize>(blockcounter+BLOCKSIZE_HW))blocksize=BLOCKSIZE_HW;
			   else blocksize=picType->getCurrentPic().CodeSize-blockcounter;
			nBytes+=readCodeBlock(dataBlock,currentBlockCounter,blocksize,blocktype);
			for(int i=0;i<blocksize;i++)
			{
				if(picType->getCurrentPic().CodeSize>(blockcounter+i))
				{
					/*if (blockcounter+i >= 0x800 && blockcounter+i <= 0xA00)
					{
						cerr<<(int)(unsigned char)dataBlock[i]<<endl;
					}*/
					mem[blockcounter+i]=((unsigned char)(dataBlock[i]&0xFF));
				}
				else
				{
					cerr<<"Trying to read memory outside Code area"<<endl;
					//return -1;
				}
			}

			/*if (dataBlock[BLOCKSIZE_HW-1] == 0)
			{
				blockcounter-=BLOCKSIZE_HW-1;
			}*/
		}
		hexData->putCodeMemory(mem);
	}
	return nBytes;
}

/* Write the code memory area of the pic with the data in *hexData */
int Hardware::writeCode(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	int BLOCKSIZE_HW;
	unsigned char dataBlock[BLOCKSIZE_CODE];
	int blocktype;
	if (CurrentHardware == HW_BOOTLOADER)BLOCKSIZE_HW=BLOCKSIZE_BOOTLOADER;
	else BLOCKSIZE_HW=BLOCKSIZE_CODE;
	if (_handle !=NULL)
	{
		nBytes=0;
		for(int blockcounter=0;blockcounter<(signed)hexData->getCodeMemory().size();blockcounter+=BLOCKSIZE_HW)
		{
			statusCallBack ((blockcounter*100)/((signed)hexData->getCodeMemory().size()));
			for(int i=0;i<BLOCKSIZE_HW;i++)
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
			if(((signed)hexData->getCodeMemory().size()-BLOCKSIZE_HW)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
			int	currentBlockCounter=blockcounter;
			if(picType->getCurrentPic().Name.find("P18F")!=0)currentBlockCounter/=2;
			nBytes=writeCodeBlock(dataBlock,currentBlockCounter,BLOCKSIZE_HW,blocktype);
			if (CurrentHardware == HW_UPP)
			{
				if(nBytes==3) return -3;	//something not implemented in firmware :(
				if(nBytes==4) return -4;	//verify error
				if(((blocktype==BLOCKTYPE_MIDDLE)||(blocktype==BLOCKTYPE_FIRST))&&(nBytes!=2))return -2; //should ask for next block
				if((blocktype==BLOCKTYPE_LAST)&&(nBytes!=1))return -1;	//should say OK
			}
		}
	}
	else return -5;
	return 0;
}

/* read the Eeprom Data area of the pic into *hexData->dataMemory */
int Hardware::readData(ReadHexFile *hexData,PicType *picType)
{
	int nBytes,blocksize;
	nBytes=-1;
	vector<int> mem;
	mem.resize(picType->getCurrentPic().DataSize);
	char dataBlock[BLOCKSIZE_DATA];
	int blocktype;
	statusCallBack (0);
	if (CurrentHardware == HW_BOOTLOADER)return 0; //TODO implement readData for bootloader
	if(picType->getCurrentPic().DataSize==0)return 0;//no data to read
	if (_handle !=NULL)
	{
		for(int blockcounter=0;blockcounter<picType->getCurrentPic().DataSize;blockcounter+=BLOCKSIZE_DATA)
		{
			statusCallBack ((blockcounter*100)/((signed)picType->getCurrentPic().DataSize));
			blocktype=BLOCKTYPE_MIDDLE;
			if(blockcounter==0)blocktype|=BLOCKTYPE_FIRST;
			if((picType->getCurrentPic().DataSize-BLOCKSIZE_DATA)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
			if(picType->getCurrentPic().DataSize>(blockcounter+BLOCKSIZE_DATA))blocksize=BLOCKSIZE_DATA;
			   else blocksize=picType->getCurrentPic().DataSize-blockcounter;

			nBytes+=readDataBlock(dataBlock,blockcounter,blocksize,blocktype);
			for(int i=0;i<blocksize;i++)
			{
				if(picType->getCurrentPic().DataSize>(blockcounter+i))
				{
					mem[blockcounter+i]=(unsigned char)(dataBlock[i]&0xFF);
				}
				else
				{
					cerr<<"Trying to read memory outside Data area"<<endl;
	//				return -1;
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
	unsigned char dataBlock[BLOCKSIZE_DATA];
	int blocktype;
	statusCallBack (0);
	if (CurrentHardware == HW_BOOTLOADER)return 0; //TODO implement writeData for bootloader
	if (_handle !=NULL)
	{
		nBytes=0;
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
			nBytes=writeDataBlock(dataBlock,blockcounter,BLOCKSIZE_DATA,blocktype);
			if(nBytes==3) return -3;	//something not implemented in firmware :(
			if(((blocktype==BLOCKTYPE_MIDDLE)||(blocktype==BLOCKTYPE_FIRST))&&(nBytes!=2))return -2; //should ask for next block
			if((blocktype==BLOCKTYPE_LAST)&&(nBytes!=1))return -1;	//should say OK
		}
	}
	else return -4;
	return 0;
}

/* Read the configuration words (and user ID's for PIC16 dev's) */
int Hardware::readConfig(ReadHexFile *hexData,PicType *picType)
{
	int nBytes,blocksize;
	nBytes=-1;

	vector<int> mem;
	mem.resize(picType->getCurrentPic().ConfigSize);
	char dataBlock[BLOCKSIZE_CONFIG];
	int blocktype;
	statusCallBack (0);
	if (CurrentHardware == HW_BOOTLOADER)return 0; //TODO implement readConfig for bootloader
	if (_handle !=NULL)
	{
		for(int blockcounter=0;blockcounter<picType->getCurrentPic().ConfigSize;blockcounter+=BLOCKSIZE_CONFIG)
		{
			statusCallBack ((blockcounter*100)/((signed)picType->getCurrentPic().ConfigSize));
			blocktype=BLOCKTYPE_MIDDLE;
			if(blockcounter==0)blocktype|=BLOCKTYPE_FIRST;
			if((picType->getCurrentPic().ConfigSize-BLOCKSIZE_CONFIG)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
			int	currentBlockCounter=blockcounter;
			if(picType->getCurrentPic().Name.find("P18F")!=0)currentBlockCounter/=2;
			if(picType->getCurrentPic().ConfigSize>(blockcounter+BLOCKSIZE_CONFIG))blocksize=BLOCKSIZE_CONFIG;
			   else blocksize=picType->getCurrentPic().ConfigSize-blockcounter;
			nBytes+=readConfigBlock(dataBlock,currentBlockCounter+picType->getCurrentPic().ConfigAddress,blocksize,blocktype);
			for(int i=0;i<blocksize;i++)
			{
				if(picType->getCurrentPic().ConfigSize>(blockcounter+i))
				{
					mem[blockcounter+i]=(unsigned char)(dataBlock[i]&0xFF);
//					cerr<<hex<<(int)dataBlock[i]<<" "<<dec;
				}
				else
				{
					cerr<<"Trying to read memory outside Config area: bc+i="<<blockcounter+i<<"Configsize="<<picType->getCurrentPic().ConfigSize<<endl;
//					return -1;
				}
			}
		}
/*		for(int i=0;i<picType->getCurrentPic().ConfigSize;i++)
			mem[i]&=picType->getCurrentPic().ConfigMask[i];*/
		hexData->putConfigMemory(mem);
	}
	cerr<<endl;
	return nBytes;
}

/*Writes the configuration words (and user ID's for PIC16 dev's)*/
int Hardware::writeConfig(ReadHexFile *hexData,PicType *picType)
{
	int nBytes;
	unsigned char dataBlock[BLOCKSIZE_CONFIG];
	int blocksize;
	int blocktype;
	statusCallBack (0);
	if (CurrentHardware == HW_BOOTLOADER)return 0; //TODO implement writeConfig for bootloader
	if (_handle !=NULL)
	{
		nBytes=0;
		for(int blockcounter=0;blockcounter<(signed)hexData->getConfigMemory().size();blockcounter+=BLOCKSIZE_CONFIG)
		{
			statusCallBack ((blockcounter*100)/((signed)hexData->getConfigMemory().size()));
			blocksize=0;
			for(int i=0;i<BLOCKSIZE_CONFIG;i++)
			{
				if((signed)hexData->getConfigMemory().size()>(blockcounter+i))
				{
					dataBlock[i]=hexData->getConfigMemory()[blockcounter+i];
					blocksize++;
				}
				else
				{
					dataBlock[i]=0;
				}
			}

			blocktype=BLOCKTYPE_MIDDLE;
			if(blockcounter==0)blocktype|=BLOCKTYPE_FIRST;
			if(((signed)hexData->getConfigMemory().size()-BLOCKSIZE_CONFIG)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
			int	currentBlockCounter=blockcounter;
			if(picType->getCurrentPic().Name.find("P18F")!=0)currentBlockCounter/=2;
			nBytes=writeConfigBlock(dataBlock,currentBlockCounter+picType->getCurrentPic().ConfigAddress,blocksize,blocktype);
			if(nBytes==3) return -3;	//something not implemented in firmware :(
			if(((blocktype==BLOCKTYPE_MIDDLE)||(blocktype==BLOCKTYPE_FIRST))&&(nBytes!=2))return -2; //should ask for next block
			if((blocktype==BLOCKTYPE_LAST)&&(nBytes!=1))return -1;	//should say OK
		}
	}
	else return -4;
	return 0;
}

/*Reads the whole PIC and checks if the data matches hexData*/
VerifyResult Hardware::verify(ReadHexFile *hexData, PicType *picType, bool doCode, bool doConfig, bool doData)
{
    VerifyResult res;
    ReadHexFile *verifyHexFile = new ReadHexFile;
    if(doCode)
    {
        if(readCode(verifyHexFile,picType)<0)
    	{
            res.Result=VERIFY_USB_ERROR;
            res.DataType=TYPE_CODE;
            return res;
        }
    }
    if(doData)
    {
    	if(readData(verifyHexFile,picType)<0)
    	{
            res.Result=VERIFY_USB_ERROR;
            res.DataType=TYPE_DATA;
            return res;
        }
    }
    if(doConfig)
    {
    	if(readConfig(verifyHexFile,picType)<0)
    	{
            res.Result=VERIFY_USB_ERROR;
            res.DataType=TYPE_CONFIG;
            return res;
        }
    }
    if ((hexData->getCodeMemory().size()+
        hexData->getDataMemory().size())>0) //there should be at least some data in the file
    {
        if(doCode)
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
        }
        if(doData)
        {
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
        }
        if(doConfig)
        {
            for(int i=0;i<(signed)hexData->getConfigMemory().size();i++)
            {
                if((signed)verifyHexFile->getConfigMemory().size()<(i+1))
                {
                    res.Result=VERIFY_OTHER_ERROR;
                    return res;
                }
                if((verifyHexFile->getConfigMemory()[i]&picType->getCurrentPic().ConfigMask[i])!=(hexData->getConfigMemory()[i]&picType->getCurrentPic().ConfigMask[i]))
                {
                    res.Result=VERIFY_MISMATCH;
                    res.DataType=TYPE_CONFIG;
                    res.Address=i;
                    res.Read=verifyHexFile->getConfigMemory()[i];
                    res.Expected=hexData->getConfigMemory()[i];
                    return res;
                }

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
	int devId=0;
	if (CurrentHardware == HW_BOOTLOADER) return 0x11240;	//PIC18F2550
	setPicType (new PicType("P18F2550"));	//need to set hardware to PIC18, no matter which one
	devId=readId();
	PicType *picType = new PicType(0x10000|devId);
	if (picType->matchWasFound())return devId|0x10000; //add an extra bit to make the difference between 16F and 18F
	delete picType;
	//if(((devId&0xFF00)!=0xFF00)&&(devId!=0xFEFE)&&(devId!=0x00FE)&&(devId>1))
	setPicType(new PicType("P16F628A"));	//and now try PIC16*/

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
	if (CurrentHardware == HW_UPP)
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
			nBytes = readString(msg,64);

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
	else
	{
		cout<<"Get Firmware version for bootloader..."<<endl;
		msg[0]=0;
		msg[1]=0;
		msg[2]=0;
		msg[3]=0;
		msg[4]=0;

		int nBytes=-1;
		statusCallBack (0);
		if (_handle !=NULL)
		{
			cout<<"writeString"<<endl;
			if(writeString(msg,5)<0)
			{
				return -1;
			}
			cout<<"readString"<<endl;
			nBytes = readString(msg,64);

			if (nBytes < 0 )
			{
				return nBytes;
			}
			else
			{
				statusCallBack (100);
				sprintf((char*)msg, "Bootloader v%d.%d", msg[3], msg[2]);

				return nBytes;
			}
		}
	}
	return -1;
}

/*read a string of data from usbpicprog (through interrupt_read)*/
int Hardware::readString(char* msg,int size)
{
	if (_handle == NULL) return -1;
	EndpointMode mode = endpointMode(WRITE_ENDPOINT);
	int nBytes;
	if ( mode==Interrupt )nBytes = usb_interrupt_read(_handle,READ_ENDPOINT,(char*)msg,size,5000);
	else nBytes = usb_bulk_read(_handle,READ_ENDPOINT,(char*)msg,size,5000);
		if (nBytes < 0 )
		{
			cerr<<"Usb Error while reading: "<<nBytes<<endl;
			cerr<<usb_strerror()<<endl;
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
		EndpointMode mode = endpointMode(WRITE_ENDPOINT);
		if ( mode==Interrupt )nBytes = usb_interrupt_write(_handle,WRITE_ENDPOINT,(char*)msg,size,500);
		else nBytes = usb_bulk_write(_handle,WRITE_ENDPOINT,(char*)msg,size,500);
		if (nBytes < size )
		{
			cerr<<"Usb Error while writing to device: "<<size<<" bytes, errCode: "<<nBytes<<endl;
			cerr<<usb_strerror()<<endl;
		}
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
		nBytes = readString(msg,2);

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

int Hardware::readConfigBlock(char * msg, int address, int size, int lastblock)
{
	int nBytes = -1;

	if (_handle !=NULL)
	{
		if (CurrentHardware == HW_UPP)
		{
			return readCodeBlock(msg, address, size, lastblock);
		}
		else
		{
			BootloaderPackage bootloaderPackage;

			bootloaderPackage.fields.cmd=CMD_BOOT_READ_CONFIG;
			bootloaderPackage.fields.size=size;
			bootloaderPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
			bootloaderPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
			bootloaderPackage.fields.addrL=(unsigned char)(address&0xFF);

			nBytes = writeString(bootloaderPackage.data,5);

			if (nBytes < 0 )
			{
				return nBytes;
			}

			char tmpmsg[size+5];

			nBytes = readString(tmpmsg,size+5) - 5;
			if(nBytes<0)return nBytes;
			memcpy(msg,tmpmsg+5,nBytes);
		}


		if (nBytes < 0 )
			cerr<<"Usb Error"<<endl;
		return nBytes;
	}
	else return -1;
}


/*private function to read one block of code memory*/
int Hardware::readCodeBlock(char * msg,int address,int size,int lastblock)
{
	int nBytes = -1;
	if (_handle !=NULL)
	{
		if (CurrentHardware == HW_UPP)
		{
			UppPackage uppPackage;

			uppPackage.fields.cmd=CMD_READ_CODE;
			uppPackage.fields.size=size;
			uppPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
			uppPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
			uppPackage.fields.addrL=(unsigned char)(address&0xFF);
			uppPackage.fields.blocktype=(unsigned char)lastblock;
			nBytes = writeString(uppPackage.data,6);

			if (nBytes < 0 )
			{
				return nBytes;
			}

			nBytes = readString(msg,size);
		}
		else
		{
			BootloaderPackage bootloaderPackage;

			bootloaderPackage.fields.cmd=CMD_BOOT_READ_CODE;
			bootloaderPackage.fields.size=size;
			bootloaderPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
			bootloaderPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
			bootloaderPackage.fields.addrL=(unsigned char)(address&0xFF);

			nBytes = writeString(bootloaderPackage.data,5);

			if (nBytes < 0 )
			{
				return nBytes;
			}

			char tmpmsg[size+5];

			nBytes = readString(tmpmsg,size+5) - 5;

			memcpy(msg,tmpmsg+5,nBytes);
		}


		if (nBytes < 0 )
			cerr<<"Usb Error"<<endl;
		return nBytes;
	}
	else return -1;
}

/*private function to write one block of code memory*/
int Hardware::writeCodeBlock(unsigned char * msg,int address,int size,int lastblock)
{
	char resp_msg[64];
	if (_handle !=NULL)
	{
		if (CurrentHardware == HW_UPP)
		{
			UppPackage uppPackage;
			uppPackage.fields.cmd=CMD_WRITE_CODE;
			uppPackage.fields.size=size;
			uppPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
			uppPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
			uppPackage.fields.addrL=(unsigned char)(address&0xFF);
			uppPackage.fields.blocktype=(unsigned char)lastblock;
			memcpy(uppPackage.fields.dataField,msg,size);
			if(address==0)
			{
				for(int i=0;i<size+6;i++)
					cout<<hex<<(int)uppPackage.data[i]<<" "<<dec;
				cout<<endl;
			}
			int nBytes = writeString(uppPackage.data,size+6);
			if (nBytes < 0 )
			{
				return nBytes;
			}
			nBytes = readString(resp_msg,1);
			if (nBytes < 0 )
				cerr<<"Usb Error"<<endl;
			cout<<"Resp message: "<<(int)resp_msg[0]<<endl;
			return (int)resp_msg[0];
		}
		else
		{
			int OkOrLastblock=2; //Ask for next block
			if (lastblock==BLOCKTYPE_LAST)OkOrLastblock=1; //Ok
			if(address<0x800)return OkOrLastblock;	//should not write inside bootloader area
			BootloaderPackage bootloaderPackage;

			bootloaderPackage.fields.cmd=CMD_BOOT_WRITE_CODE;
			bootloaderPackage.fields.size=size;
			bootloaderPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
			bootloaderPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
			bootloaderPackage.fields.addrL=(unsigned char)(address&0xFF);
			memcpy(bootloaderPackage.fields.dataField,msg,size);
			if (writeString(bootloaderPackage.data,5+size) < 0 )
			{
				return -1;
			}
			readString(resp_msg,5+size);
			return OkOrLastblock;
		}
	}

	else return -1;

}

/*private function to write one block of config memory*/
int Hardware::writeConfigBlock(unsigned char * msg,int address,int size,int lastblock)
{
	char resp_msg[64];
	UppPackage uppPackage;
	if (CurrentHardware == HW_BOOTLOADER)return 1;//say OK, but do nothing... we don't want to write config bits
	if (_handle !=NULL)
	{
		uppPackage.fields.cmd=CMD_WRITE_CONFIG;
		uppPackage.fields.size=size;
		uppPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
		uppPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
		uppPackage.fields.addrL=(unsigned char)(address&0xFF);
		uppPackage.fields.blocktype=(unsigned char)lastblock;
		memcpy(uppPackage.fields.dataField,msg,size);
		for(int i=0;i<size+6;i++)
			cout<<hex<<(int)uppPackage.data[i]<<" "<<dec;
		cout<<endl;
		int nBytes = writeString(uppPackage.data,size+6);
		if (nBytes < 0 )
		{
			return nBytes;
		}

		nBytes = readString(resp_msg,1);
		if (nBytes < 0 )
			cerr<<"Usb Error"<<endl;
		return (int)resp_msg[0];
	}
	else return -1;

}


/*private function to read one block of data memory*/
int Hardware::readDataBlock(char * msg,int address,int size,int lastblock)
{
	int nBytes = -1;

	if (_handle !=NULL)
	{
		if (CurrentHardware == HW_UPP)
		{
			UppPackage uppPackage;

			uppPackage.fields.cmd=CMD_READ_DATA;
			uppPackage.fields.size=size;
			uppPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
			uppPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
			uppPackage.fields.addrL=(unsigned char)(address&0xFF);
			uppPackage.fields.blocktype=(unsigned char)lastblock;
			nBytes = writeString(uppPackage.data,6);

			if (nBytes < 0 )
			{
				return nBytes;
			}

			nBytes = readString(msg,size);
		}
		else
		{
			BootloaderPackage bootloaderPackage;

			bootloaderPackage.fields.cmd=CMD_BOOT_READ_DATA;
			bootloaderPackage.fields.size=size;
			bootloaderPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
			bootloaderPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
			bootloaderPackage.fields.addrL=(unsigned char)(address&0xFF);

			nBytes = writeString(bootloaderPackage.data,5);

			if (nBytes < 0 )
			{
				return nBytes;
			}

			char tmpmsg[size+5];

			nBytes = readString(tmpmsg,size+5) - 5;

			memcpy(msg,tmpmsg+5,nBytes);
		}


		if (nBytes < 0 )
			cerr<<"Usb Error"<<endl;
		return nBytes;
	}
	else return -1;
}

/*private function to write one block of data memory*/
int Hardware::writeDataBlock(unsigned char * msg,int address,int size,int lastblock)
{
	char resp_msg[64];
	UppPackage uppPackage;
	if (_handle !=NULL)
	{
        if (CurrentHardware == HW_UPP)
		{
    		uppPackage.fields.cmd=CMD_WRITE_DATA;
    		uppPackage.fields.size=size;
    		uppPackage.fields.addrU=0;
    		uppPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
    		uppPackage.fields.addrL=(unsigned char)(address&0xFF);
    		uppPackage.fields.blocktype=(unsigned char)lastblock;
    		memcpy(uppPackage.fields.dataField,msg,size);
    		int nBytes = writeString(uppPackage.data,size+6);
    		if (nBytes < 0 )
    		{
    			return nBytes;
    		}

    		nBytes = readString(resp_msg,1);
    		if (nBytes < 0 )
    			cerr<<"Usb Error"<<endl;
    		return (int)resp_msg[0];
        }
        else
		{
			int OkOrLastblock=2; //Ask for next block
			if (lastblock==BLOCKTYPE_LAST)OkOrLastblock=1; //Ok
			BootloaderPackage bootloaderPackage;
			bootloaderPackage.fields.cmd=CMD_BOOT_WRITE_DATA;
			bootloaderPackage.fields.size=size;
			bootloaderPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
			bootloaderPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
			bootloaderPackage.fields.addrL=(unsigned char)(address&0xFF);
			memcpy(bootloaderPackage.fields.dataField,msg,size);
			if (writeString(bootloaderPackage.data,5+size) < 0 )
			{
				return -1;
			}
			if(readString(resp_msg,5+size)<0)return -1;
			return OkOrLastblock;
		}
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
		UppMainWindow* CB=(UppMainWindow*)ptCallBack;
		CB->updateProgress(value);
	}
}


