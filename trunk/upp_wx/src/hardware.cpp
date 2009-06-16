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

// IMPORTANT: first include wxWidgets headers (indirectly)
#include "hardware.h"
#include "uppmainwindow.h"

// NOW we can include the <usb.h> header without compiling problems
#include <usb.h>
#include <iostream>

using namespace std;
// #define USB_DEBUG 2


// ----------------------------------------------------------------------------
// Hardware
// ----------------------------------------------------------------------------

Hardware::Hardware(UppMainWindow* CB, HardwareType hwtype)
{
    struct usb_bus *bus=NULL;
    struct usb_device *dev=NULL;
    m_abortOperations=false;
    m_handle=NULL;
    m_pCallBack=CB;

    usb_init();
    usb_find_busses();
    usb_find_devices();
#ifdef USB_DEBUG
    cout<<"USB debug enabled, remove '#define USB_DEBUG 2' in hardware.cpp to disable it"<<endl;
    usb_set_debug(USB_DEBUG);
#endif

    // search the various USB busses:
    for (bus=usb_get_busses();bus;bus=bus->next)
    {
        for (dev=bus->devices;dev;dev=dev->next)
        {
            if (hwtype == HW_UPP)
            {
                // search first the UPP and then, if no UPP is found, the bootloader

                if ((dev->descriptor.idVendor == UPP_VENDOR) && 
                    (dev->descriptor.idProduct == UPP_PRODUCT))
                {
                    m_handle = usb_open(dev);
                    if (!m_handle)
                        continue; // failed to open this device, choose the next one

                    hwtype=HW_UPP;
                    break; // found usbpicprog, exit the for loop
                }

                if ((dev->descriptor.idVendor == BOOTLOADER_VENDOR) && 
                    (dev->descriptor.idProduct == BOOTLOADER_PRODUCT))
                {
                    m_handle = usb_open(dev);
                    if (!m_handle)
                        continue; // failed to open this device, choose the next one

                    hwtype=HW_BOOTLOADER;
                    break; // found bootloader, exit the for loop
                }
            }
            else
            {
                // search first the bootloader and then, if no bootloader is found, the UPP

                if ((dev->descriptor.idVendor == BOOTLOADER_VENDOR) && 
                    (dev->descriptor.idProduct == BOOTLOADER_PRODUCT))
                {
                    m_handle = usb_open(dev);
                    if (!m_handle)
                        continue; // failed to open this device, choose the next one

                    hwtype=HW_BOOTLOADER;
                    break; // found bootloader, exit the for loop
                }
                if ((dev->descriptor.idVendor == UPP_VENDOR) && 
                    (dev->descriptor.idProduct == UPP_PRODUCT))
                {
                    m_handle = usb_open(dev);
                    if (!m_handle)
                        continue; // failed to open this device, choose the next one

                    hwtype=HW_UPP;
                    break; // found usbpicprog, exit the for loop
                }
            }

            m_handle=NULL;
        }

        if (m_handle!=NULL)	// successfully initialized? don't try any other bus
            break;
    }

    if (m_handle==NULL)
    {
        m_hwCurrent = HW_NONE;
        return;
    }

    // we've found some hardware!

    m_hwCurrent = (HardwareType)hwtype;
#ifndef __WXMSW__
    if (usb_reset(m_handle) < 0)
        wxLogError("Couldn't reset interface");

    usb_close(m_handle);
    m_handle = usb_open(dev);

    if (!m_handle)
    {
        m_hwCurrent = HW_NONE;
        return;
    }
#endif
#if 0
    tryToDetachDriver();
#endif

    // initialize the libusb members used later to communicate with
    // the hardware

    int _config=1;
    int i;

    m_nInterfaceNumber = 0;

    for (i=0; i<dev->descriptor.bNumConfigurations; i++)
        if ( _config==dev->config[i].bConfigurationValue )
            break;

    if ( i==dev->descriptor.bNumConfigurations )
    {
        i = 0;
        _config = dev->config[i].bConfigurationValue;
    }

    const usb_config_descriptor &configd = dev->config[i];
    if ( usb_set_configuration(m_handle, _config)<0 )
    {
        wxLogError("Error setting configuration");
        m_hwCurrent = HW_NONE;
        m_handle = NULL;
        return;
    }

    for (i=0; i<configd.bNumInterfaces; i++)
        if ( m_nInterfaceNumber==configd.interface[i].altsetting[0].bInterfaceNumber )
            break;

    if ( i==configd.bNumInterfaces )
    {
        int old = m_nInterfaceNumber;
        i = 0;
        m_nInterfaceNumber = configd.interface[i].altsetting[0].bInterfaceNumber;
        wxLogError("Interface %d not present: using %d", old, m_nInterfaceNumber);
    }

    m_interface = &(configd.interface[i].altsetting[0]);


    // claim the USB interface

    if ( usb_claim_interface(m_handle, m_nInterfaceNumber)<0 )
    {
        wxLogError("Error claiming interface");
        m_hwCurrent = HW_NONE;
        m_handle = NULL;
        return;
    }

    // everything completed successfully:

    wxASSERT(m_handle != NULL && m_hwCurrent != HW_NONE);
}

Hardware::~Hardware()
{
    if (m_handle)
    {
        usb_release_interface(m_handle, m_nInterfaceNumber);
        usb_close(m_handle);

        m_handle=NULL;
    }
        
    m_hwCurrent=HW_NONE;
}

void Hardware::statusCallBack(int value) const
{
    if (m_pCallBack)
        m_pCallBack->updateProgress(value);
}

Hardware::EndpointMode Hardware::endpointMode(int ep) const
{
    int index = ep & USB_ENDPOINT_ADDRESS_MASK;
    const usb_endpoint_descriptor *ued = m_interface->endpoint + index;
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

int Hardware::setPicType(PicType* picType)
{
    if (!picType->ok()) return -1;
    if (m_hwCurrent == HW_BOOTLOADER) return -1;
    if (m_handle == NULL) return -1;

    statusCallBack (0);

    unsigned char msg[64];
    msg[0]=CMD_SET_PICTYPE;
    msg[1]=picType->picFamily;

    // send the command to the hw
    if (writeString(msg,2) < 0)
        return -1;       // failure

    // read back the reply
    int nBytes = readString(msg,1);
    if (nBytes < 0)
    {
        wxLogError("Usb Error");
        return nBytes;
    }

    statusCallBack (100);
    return (int)msg[0];
}

int Hardware::bulkErase(PicType* picType)
{
    unsigned char msg[64];

    if (!picType->ok()) return -1;
    if (m_handle == NULL) return -1;

    statusCallBack (0);

    if (m_hwCurrent == HW_UPP)
    {
        if (m_abortOperations)
            return OPERATION_ABORTED;

        // send the command
        msg[0]=CMD_ERASE;
        if (writeString(msg,1) < 0)
            return 0;

        // read back the reply
        int nBytes = readString(msg,1);
        if (nBytes < 0)
        {
            wxLogError("Usb Error");
            return nBytes;
        }

        statusCallBack (100);
        return (int)msg[0];
    }
    else // hardware is HW_BOOTLOADER
    {
        HexFile hf(picType);
        if (write(TYPE_DATA, &hf, picType) < 0)
            return -1;

        statusCallBack(50);

        for (unsigned int address=0x800; address<picType->CodeSize; address+=64)
        {
            if (m_abortOperations)
                return OPERATION_ABORTED;

            BootloaderPackage bootloaderPackage;
            bootloaderPackage.fields.cmd=CMD_BOOT_ERASE;
            bootloaderPackage.fields.size=1; // only one block of 64 bytes supported by bootloader
            bootloaderPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
            bootloaderPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
            bootloaderPackage.fields.addrL=(unsigned char)(address&0xFF);

            if (writeString(bootloaderPackage.data,5) < 0)
                return -1;
            if (readString(msg,5) < 0)
                return -1;

            statusCallBack(100);
        }

        return 1;   // OK
    }
}

int Hardware::read(MemoryType type, HexFile *hexData, PicType *picType, int numberOfBytes, HexFile *verifyData)
{
    if (!picType->ok()) return -1;
    if (m_handle == NULL) return -1;

    if (m_abortOperations)
        return OPERATION_ABORTED;

    // which memory area are we going to read?
    unsigned int memorySize = 0;
	
    switch (type)
    {
		case TYPE_CODE: memorySize = picType->CodeSize; break;
		case TYPE_DATA: memorySize = picType->DataSize; break;
		case TYPE_CONFIG: memorySize = picType->ConfigSize; break;
    }

	if((numberOfBytes>0)&&(numberOfBytes<memorySize))memorySize=numberOfBytes;
	
    if (memorySize==0)
        return 0;       // no code/config/data to read

    // create a temporary array to store read data
    vector<int> mem;
    mem.resize(memorySize, 0xFF);

    // how big is each block?
    unsigned int blockSizeHW;
	switch(type)
	{
		case TYPE_CONFIG:
		case TYPE_CODE:
			if (m_hwCurrent == HW_BOOTLOADER)
				blockSizeHW=BLOCKSIZE_BOOTLOADER;
			else 
			{
				if(picType->is24Bit())blockSizeHW=BLOCKSIZE_CODE_DSPIC;
				else blockSizeHW=BLOCKSIZE_CODE;
			}
			break;
		case TYPE_DATA:
			if (m_hwCurrent == HW_BOOTLOADER)
				blockSizeHW=BLOCKSIZE_BOOTLOADER;
			else 
			{
				if(picType->is24Bit())blockSizeHW=BLOCKSIZE_DATA_DSPIC;
				else blockSizeHW=BLOCKSIZE_DATA;
			}
			break;
	}
			
    statusCallBack (0);

    if (type == TYPE_DATA && m_hwCurrent == HW_BOOTLOADER)
        return 0;   // TODO implement readData for bootloader
    if (type == TYPE_CONFIG && m_hwCurrent == HW_BOOTLOADER)
        return 0;   // Better write no configuration words in bootloader;
                    // it's unsafe: you might destroy the bootloader

    int nBytes=0;
    for(unsigned int blockcounter=0; blockcounter<memorySize; blockcounter+=blockSizeHW)
    {
        statusCallBack (blockcounter*100/memorySize);

        // set the type of this block
        unsigned int blocktype = BLOCKTYPE_MIDDLE;
        if (blockcounter == 0)
            blocktype |= BLOCKTYPE_FIRST;
        if (((int)memorySize-(int)blockSizeHW)<=(int)blockcounter)
            blocktype |= BLOCKTYPE_LAST;
        if (m_abortOperations)
            blocktype |= BLOCKTYPE_LAST;


		
        unsigned int currentBlockCounter = blockcounter;
        if (picType->is14Bit())
            currentBlockCounter /= 2;
        unsigned int blocksize;
        if (memorySize > (blockcounter+blockSizeHW))
            blocksize = blockSizeHW;
        else 
            blocksize = memorySize-blockcounter;

        // do read the block
        unsigned char dataBlock[BLOCKSIZE_MAXSIZE];
        nBytes += readBlock(type, dataBlock, currentBlockCounter, blocksize, blocktype);

        // move read data in the temporary array
        for (unsigned int i=0; i<blocksize; i++)
        {
            if (memorySize > (blockcounter+i))
                mem[blockcounter+i]=((int)(dataBlock[i]&0xFF));
            else
            {
                wxLogError("Trying to read memory outside allowed area: bc+i=%d; memory size=%d", 
                           blockcounter+i, memorySize);
                // return -1;
            }
			if(verifyData!=NULL)
			{
				switch(type)
				{
					case TYPE_DATA:
						if(verifyData->getMemory(type,blockcounter+i)!=mem[blockcounter+i]) m_abortOperations=true;
						break;
					case TYPE_CODE:
						if(verifyData->getMemory(type,blockcounter+i)!=mem[blockcounter+i]) 
						{
							if(m_hwCurrent==HW_BOOTLOADER&&((blockcounter+i)>0x800))m_abortOperations=true;
							if(m_hwCurrent!=HW_BOOTLOADER)m_abortOperations=true;
						}
						break;
					case TYPE_CONFIG: //just continue reading -- this is always short and complicated to verify in this step
						break;
				}
			}
			//if(blockcounter==0)cout<<std::hex<<mem[blockcounter+i]<<endl;
        }

        if (m_abortOperations&&((blocktype &BLOCKTYPE_LAST)==BLOCKTYPE_LAST))
            break;
    }

    // finally save the data we've just read into the hexfile:
    hexData->putMemory(type, mem, picType);

    return nBytes;
}

int Hardware::write(MemoryType type, HexFile *hexData, PicType *picType)
{
    if (!picType->ok()) return -1;
    if (m_handle == NULL) return -1;

    if (m_abortOperations)
        return OPERATION_ABORTED;

    // which memory area are we going to write?
    vector<int>* memory = NULL;
    
    memory = &hexData->getMemory(type);
    
    if (memory->size()==0)
	{
        return 1;       // no code/config/data to write
	}

    // how big is each block?
    unsigned int blockSizeHW;
	switch(type)
	{
		case TYPE_CODE:
			if (m_hwCurrent == HW_BOOTLOADER)
				blockSizeHW=BLOCKSIZE_BOOTLOADER;
			else 
			{
				if(picType->is24Bit())blockSizeHW=BLOCKSIZE_CODE_DSPIC;
				else blockSizeHW=BLOCKSIZE_CODE;
			}
			break;
		case TYPE_DATA:
			if (m_hwCurrent == HW_BOOTLOADER)
				blockSizeHW=BLOCKSIZE_BOOTLOADER;
			else 
			{
				if(picType->is24Bit())blockSizeHW=BLOCKSIZE_DATA_DSPIC;
				else blockSizeHW=BLOCKSIZE_DATA;
			}
			break;
		case TYPE_CONFIG:
			blockSizeHW=BLOCKSIZE_CONFIG;
			break;
	}

    statusCallBack (0);

    int nBytes=0;
    for (unsigned int blockcounter=0; blockcounter<memory->size(); blockcounter+=blockSizeHW)
    {
        statusCallBack (blockcounter*100/memory->size());

        // fill in a new datablock packet
        unsigned char dataBlock[BLOCKSIZE_MAXSIZE];
        for (unsigned int i=0; i<blockSizeHW; i++)
        {
            if (memory->size() > (blockcounter+i))
                dataBlock[i]=(*memory)[blockcounter+i];
            else
                dataBlock[i]=0;
        }

        // set the type of this block
        unsigned int blocktype = BLOCKTYPE_MIDDLE;
        if (blockcounter == 0)
            blocktype |= BLOCKTYPE_FIRST;
        if (((int)memory->size()-(int)blockSizeHW) <= (int)blockcounter)
            blocktype |= BLOCKTYPE_LAST;
        if (m_abortOperations)
            blocktype |= BLOCKTYPE_LAST;

		/*if(type==TYPE_CONFIG)
		{
			cout<<"Blocktype: "<<blocktype<<", memory size: "<<memory->size()<<", blocksizeHw: "<<blockSizeHW<<", blockcounter: "<<blockcounter<<endl;
		}*/
		   

        unsigned int currentBlockCounter=blockcounter;
        if (picType->is14Bit())
            currentBlockCounter /= 2;

        // do write the block
        int retCode = writeBlock(type, dataBlock, currentBlockCounter, blockSizeHW, blocktype);
        if (m_hwCurrent == HW_UPP)
        {
            if (retCode==3) 
                return -3;	// something not implemented in firmware :(
            if (retCode==4) 
                return -4;	// verify error
            if ((blocktype==BLOCKTYPE_MIDDLE || blocktype==BLOCKTYPE_FIRST) && retCode!=2)
                return -2;  // should ask for next block
            if (blocktype==BLOCKTYPE_LAST && retCode!=1)
                return -1;	// should say OK

            // retCode == 1 means "OK, all finished"; retCode == 2 means "OK, waiting for next block"
        }

        if (m_abortOperations)
            break;
    }

    return 1;       // OK
}

VerifyResult Hardware::verify(HexFile *hexData, PicType *picType, bool doCode, bool doConfig, bool doData)
{
    VerifyResult res;

    // create a temporary hex file to store the code/config/data bytes we're going to read
    HexFile readData;

    // read from the device

    if (doCode&&(hexData->getMemory(TYPE_CODE).size()>0))
    {
        if (read(TYPE_CODE, &readData, picType,hexData->getMemory(TYPE_CODE).size(),hexData) < 0)
        {
            res.Result=VERIFY_USB_ERROR;
            res.DataType=TYPE_CODE;
            return res;
        }
    }
    if (doData&&(hexData->getMemory(TYPE_DATA).size()>0))
    {
        if (read(TYPE_DATA, &readData, picType,hexData->getMemory(TYPE_DATA).size(),hexData) < 0)
        {
            res.Result=VERIFY_USB_ERROR;
            res.DataType=TYPE_DATA;
            return res;
        }
    }
    if (doConfig&&(hexData->getMemory(TYPE_CONFIG).size()>0))
    {
        if (read(TYPE_CONFIG, &readData, picType,hexData->getMemory(TYPE_CONFIG).size(),hexData) < 0)
        {
            res.Result=VERIFY_USB_ERROR;
            res.DataType=TYPE_CONFIG;
            return res;
        }
    }


    // compare read values and hexData

    if ((hexData->getMemory(TYPE_CODE).size()+
        hexData->getMemory(TYPE_DATA).size()+
        hexData->getMemory(TYPE_CONFIG).size())==0) 
    {
        // there should be at least some data in the file
        wxLogWarning("No data to verify");

        res.Result=VERIFY_OTHER_ERROR;
        return res;
    }

    if (doCode&&(hexData->getMemory(TYPE_CODE).size()>0))
    {
        res = readData.verify(TYPE_CODE, hexData,(m_hwCurrent==HW_BOOTLOADER));
        if (res.Result != VERIFY_SUCCESS)
            return res;
    }
    if (doData&&(hexData->getMemory(TYPE_DATA).size()>0))
    {
        res = readData.verify(TYPE_DATA, hexData,false);
        if (res.Result != VERIFY_SUCCESS)
            return res;
    }
    if (doConfig && m_hwCurrent == HW_UPP &&(hexData->getMemory(TYPE_CONFIG).size()>0)) 
        // it's no use to verify config for the bootloader
    {
        res = readData.verify(TYPE_CONFIG, hexData,false);
        if (res.Result != VERIFY_SUCCESS)
            return res;
    }

    res.Result=VERIFY_SUCCESS;
    return res;
}

VerifyResult Hardware::blankCheck(PicType *picType)
{
    VerifyResult ret;

    if (!picType->ok())
    {
        ret.Result = VERIFY_OTHER_ERROR;
        return ret;
    }

    // create a temporary blank HEX file and verify current PIC
    // memory against it
    HexFile blankHexFile(picType);
    return verify(&blankHexFile, picType);
}

int Hardware::autoDetectDevice()
{
    if (m_handle == NULL) return -1;

    if (m_hwCurrent == HW_BOOTLOADER)
        return -1;     // PIC18F2550

	PicType pic30 = PicType::FindPIC("P30F1010");
	if(setPicType(&pic30)<0)
		return -1;

	int devId=readId();

	cout<<"Devid PIC30: "<<devId<<endl;
	
	if(devId<0)
		return -1;

	PicType picType = PicType::FindPIC(0x20000|devId);

	if(picType.ok())
		return devId|0x20000; 
	else
	{
	
		// need to set hardware to PIC18, no matter which one
		PicType pic18 = PicType::FindPIC("P18F2550");
		if (setPicType(&pic18) < 0)
			return -1;

		int devId=readId();
		if (devId < 0)
			return -1;

		PicType picType = PicType::FindPIC(0x10000|devId);
		if (picType.ok())
		{
			return devId|0x10000; 
				// add an extra bit to make the difference between 16F and 18F
		}
		else
		{
			// try PIC16: the specific PIC16 device doesn't matter
			PicType pic16 = PicType::FindPIC("P16F628A");
			if (setPicType(&pic16) < 0)
				return -1;

			devId=readId();
			if (devId < 0)
				return -1;
			
			PicType picType = PicType::FindPIC(devId);
			if (picType.ok())
				return devId;
		}
	}
    return -1;
}

int Hardware::getFirmwareVersion(FirmwareVersion* firmwareVersion) const
{
	unsigned char msg[64];
    if (m_handle == NULL) return -1;

    if (m_hwCurrent == HW_UPP)
    {
        statusCallBack (0);
		firmwareVersion->isBootloader=false;
        // send the command
        msg[0]=CMD_FIRMWARE_VERSION;
        if (writeString(msg,1) < 0)
		{
			firmwareVersion->versionString="";
			firmwareVersion->major=0;
			firmwareVersion->minor=0;
			firmwareVersion->release=0;
			return -1;
		}
        // read back the reply
        int nBytes = readString(msg,64);
        if (nBytes < 0)
		{
			firmwareVersion->versionString="";
			firmwareVersion->major=0;
			firmwareVersion->minor=0;
			firmwareVersion->release=0;
			return nBytes;
		}
		firmwareVersion->versionString.assign((const char*)msg);
		wxString strippedVersion=firmwareVersion->versionString.substr(firmwareVersion->versionString.find_first_of(' ')+1);
		firmwareVersion->stableRelease=(strippedVersion.size()==5);
		if(firmwareVersion->stableRelease)
		{
			firmwareVersion->major=atoi(strippedVersion.substr(0,1).c_str());
			firmwareVersion->minor=atoi(strippedVersion.substr(2,1).c_str());
			firmwareVersion->release=atoi(strippedVersion.substr(4,1).c_str());
		}
		else
		{
			firmwareVersion->major=0;
			firmwareVersion->minor=0;
			firmwareVersion->release=atoi(strippedVersion.c_str());
		}
		//cout<<strippedVersion<<" "<<strippedVersion.size()<<firmwareVersion->major<<firmwareVersion->minor<<firmwareVersion->release<<endl;
        statusCallBack (100);
        return nBytes;
    }
    else
    {
		firmwareVersion->isBootloader=true;
        //wxLogMessage("Getting Firmware version for bootloader...");

        msg[0]=0;
        msg[1]=0;
        msg[2]=0;
        msg[3]=0;
        msg[4]=0;

        int nBytes=-1;
        statusCallBack (0);

        if (writeString(msg,5) < 0)
            return -1;

        nBytes = readString(msg,64);
        if (nBytes < 0)
            return nBytes;

        statusCallBack (100);
		firmwareVersion->major=msg[3];
		firmwareVersion->minor=msg[2];
		firmwareVersion->release=0;
        sprintf((char*)msg, "Bootloader v%d.%d", msg[3], msg[2]);
		firmwareVersion->versionString.assign((const char*)msg);
		firmwareVersion->stableRelease=true;
        return nBytes;
    }
}


// ----------------------------------------------------------------------------
// Hardware - private functions
// ----------------------------------------------------------------------------

int Hardware::readString(unsigned char* msg, int size) const
{
    if (m_handle == NULL) return -1;

    int nBytes;
    if ( endpointMode(READ_ENDPOINT)==Interrupt )  
        nBytes = usb_interrupt_read(m_handle,READ_ENDPOINT,(char*)msg,size,USB_OPERATION_TIMEOUT);
    else
        nBytes = usb_bulk_read(m_handle,READ_ENDPOINT,(char*)msg,size,USB_OPERATION_TIMEOUT);

    if (nBytes < 0)
    {
        wxLogError("Usb Error while reading: %s", usb_strerror());
        return -1;
    }

    return nBytes;
}

int Hardware::writeString(const unsigned char* msg, int size) const
{
    if (m_handle == NULL) return -1;

    int nBytes;
    if ( endpointMode(WRITE_ENDPOINT)==Interrupt )
        nBytes = usb_interrupt_write(m_handle,WRITE_ENDPOINT,(char*)msg,size,USB_OPERATION_TIMEOUT/10);
    else 
        nBytes = usb_bulk_write(m_handle,WRITE_ENDPOINT,(char*)msg,size,USB_OPERATION_TIMEOUT/10);

    if (nBytes < size)
    {
        wxLogError("Usb Error while writing to device: %d bytes, errCode: %d", size, nBytes);
        wxLogError("%s", usb_strerror());
        return -1;
    }

    return nBytes;
}

int Hardware::readId()
{
    if (m_handle == NULL) return -1;

    statusCallBack (0);

    // send to the hardware the READ-ID command
    unsigned char msg[64];
    msg[0]=CMD_READ_ID;
    if (writeString(msg,1) < 0)
        return -1;

    // read from the hardware the reply
    int nBytes = readString(msg,2);
    if (nBytes < 0)
        return -1;

    // success
    statusCallBack (100);
    return ((((int)msg[1])&0xFF)<<8)|(((int)msg[0])&0xFF);
}

int Hardware::readBlock(MemoryType type, unsigned char* msg, int address, int size, int lastblock)
{
    if (m_handle == NULL) return -1;

    int nBytes = -1;
    if (m_hwCurrent == HW_UPP)
    {
        UppPackage uppPackage;

        switch (type)
        {
        case TYPE_CODE:
        case TYPE_CONFIG:
			//cout<<"Read code or config block, address: "<<address<<",size: "<<size<<", lastblock: "<<lastblock<<endl;
            uppPackage.fields.cmd=CMD_READ_CODE;
            break;
        case TYPE_DATA:
            uppPackage.fields.cmd=CMD_READ_DATA;
            break;
        }
        uppPackage.fields.size=size;
        uppPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
        uppPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
        uppPackage.fields.addrL=(unsigned char)(address&0xFF);
        uppPackage.fields.blocktype=(unsigned char)lastblock;

        // send the command
        nBytes = writeString(uppPackage.data,6);
        if (nBytes < 0)
            return nBytes;

        // read back the bytes
        nBytes = readString(msg,size);
    }
    else
    {
        BootloaderPackage bootloaderPackage;

        switch (type)
        {
        case TYPE_CODE:
            bootloaderPackage.fields.cmd=CMD_BOOT_READ_CODE;
            break;
        case TYPE_CONFIG:
            bootloaderPackage.fields.cmd=CMD_BOOT_READ_CONFIG;
            break;
        case TYPE_DATA:
            bootloaderPackage.fields.cmd=CMD_BOOT_READ_DATA;
            break;
        }

        bootloaderPackage.fields.size=size;
        bootloaderPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
        bootloaderPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
        bootloaderPackage.fields.addrL=(unsigned char)(address&0xFF);

        // send the command
        // FIXME: define the constant 5 as the sizeof() of the relevant portion of
        //        the bootloader package instead of hardcoding it
        nBytes = writeString(bootloaderPackage.data,5);
        if (nBytes < 0)
            return nBytes;

        // read back the bytes
        unsigned char* tmpmsg = new unsigned char[size+5];
        nBytes = readString(tmpmsg,size+5) - 5;
        if (nBytes < 0)
        {
            wxLogError("Usb Error");
            delete [] tmpmsg;
            return nBytes;
        }
		/*for(int i=0;i<size+6;i++)
			cout<<std::hex<<(int)tmpmsg[i]<<" ";
		cout<<endl;*/

        memcpy(msg,tmpmsg+5,nBytes);
        delete [] tmpmsg;
    }

    if (nBytes < 0)
        wxLogError("Usb Error");
    return nBytes;
}

int Hardware::writeBlock(MemoryType type, unsigned char* msg, int address, int size, int lastblock)
{
    if (m_handle == NULL) return -1;

    unsigned char resp_msg[64];
    if (m_hwCurrent == HW_UPP)
    {
        UppPackage uppPackage;
        switch (type)
        {
        case TYPE_CODE:
			//cout<<"Write code block, address: "<<address<<",size: "<<size<<", lastblock: "<<lastblock<<endl;
            uppPackage.fields.cmd=CMD_WRITE_CODE;
            break;
        case TYPE_CONFIG:
			//cout<<"Write config block, address: "<<address<<",size: "<<size<<", lastblock: "<<lastblock<<endl;
            uppPackage.fields.cmd=CMD_WRITE_CONFIG;
            break;
        case TYPE_DATA:
			//cout<<"Write data block, address: "<<address<<",size: "<<size<<", lastblock: "<<lastblock<<endl;
            uppPackage.fields.cmd=CMD_WRITE_DATA;
            break;
        }
        uppPackage.fields.size=size;
        uppPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
        uppPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
        uppPackage.fields.addrL=(unsigned char)(address&0xFF);
        uppPackage.fields.blocktype=(unsigned char)lastblock;
        memcpy(uppPackage.fields.dataField,msg,size);

        /*if (address==0)
        {
            for (int i=0;i<size+6;i++)
                cout<<hex<<(int)uppPackage.data[i]<<" "<<dec;
            cout<<endl;
        }*/
		/*for(int i=0;i<size+6;i++)
			cout<<std::hex<<(int)uppPackage.data[i]<<" ";
		cout<<endl;*/
			
        int nBytes = writeString(uppPackage.data,size+6);
        if (nBytes < 0)
            return nBytes;

        nBytes = readString(resp_msg,1);
        if (nBytes < 0)
            wxLogError("Usb Error");
        // cout<<"Response message: "<<(int)resp_msg[0]<<endl;
        return (int)resp_msg[0];
    }
    else
    {
        if (type == TYPE_CONFIG)
        {
            if ((lastblock&BLOCKTYPE_LAST)==BLOCKTYPE_LAST)
                return 1; // say OK, but do nothing... we don't want to write config bits
            else 
                return 2; // do nothing, but ask for the the next block...
        }

        int OkOrLastblock=2; // ask for next block
        if (lastblock==BLOCKTYPE_LAST)
            OkOrLastblock=1; // ok

        if (address<0x800)
            return OkOrLastblock;	// should not write inside bootloader area

        BootloaderPackage bootloaderPackage;

        switch (type)
        {
        case TYPE_CODE:
            bootloaderPackage.fields.cmd=CMD_BOOT_WRITE_CODE;
            break;
        case TYPE_DATA:
            bootloaderPackage.fields.cmd=CMD_BOOT_WRITE_DATA;
            break;
        }

        bootloaderPackage.fields.size=size;
        bootloaderPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
        bootloaderPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
        bootloaderPackage.fields.addrL=(unsigned char)(address&0xFF);
        memcpy(bootloaderPackage.fields.dataField,msg,size);

        if (writeString(bootloaderPackage.data,5+size) < 0)
            return -1;

        if (readString(resp_msg,5+size) < 0)
            return -1;

        return OkOrLastblock;
    }
}

void Hardware::tryToDetachDriver()
{
    // try to detach an already existing driver... (linux only)
#if defined(LIBUSB_HAS_GET_DRIVER_NP) && LIBUSB_HAS_GET_DRIVER_NP
    //  log(Log::DebugLevel::Extra, "find if there is already an installed driver");
    char dname[256] = "";
    if ( usb_get_driver_np(m_handle, m_nInterfaceNumber, dname, 255)<0 ) return;
    //  log(Log::DebugLevel::Normal, QString("  a driver \"%1\" is already installed...").arg(dname));
#if defined(LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP) && LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
    usb_detach_kernel_driver_np(m_handle, m_nInterfaceNumber);
    // log(Log::DebugLevel::Normal, "  try to detach it...");
    if ( usb_get_driver_np(m_handle, m_nInterfaceNumber, dname, 255)<0 ) return;
    // log(Log::DebugLevel::Normal, "  failed to detach it");
#endif
#endif
}


