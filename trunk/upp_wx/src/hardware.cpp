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

// IMPORTANT: first include wxWidgets headers (indirectly)
#include <wx/log.h>
#include "hardware.h"
#include "uppmainwindow.h"

// NOW we can include the <usb.h> header without compiling problems
#include <libusb.h>
#include <iostream>

#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // useful to catch memory leaks when compiling under MSVC 
#endif

using namespace std;
//#define USB_DEBUG 3

#ifndef __WXMSW__
const char* libusb_strerror(enum libusb_error errcode)
{
    switch (errcode)
    {
        case LIBUSB_SUCCESS:
            return "Success";
        case LIBUSB_ERROR_IO:
            return "Input/output error";
        case LIBUSB_ERROR_INVALID_PARAM:
            return "Invalid parameter";
        case LIBUSB_ERROR_ACCESS:
            return "Access denied (insufficient permissions)";
        case LIBUSB_ERROR_NO_DEVICE:
            return "No such device (it may have been disconnected)";
        case LIBUSB_ERROR_NOT_FOUND:
            return "Entity not found";
        case LIBUSB_ERROR_BUSY:
            return "Resource busy";
        case LIBUSB_ERROR_TIMEOUT:
            return "Operation timed out";
        case LIBUSB_ERROR_OVERFLOW:
            return "Overflow";
        case LIBUSB_ERROR_PIPE:
            return "Pipe error";
        case LIBUSB_ERROR_INTERRUPTED:
            return "System call interrupted (perhaps due to signal)";
        case LIBUSB_ERROR_NO_MEM:
            return "Insufficient memory";
        case LIBUSB_ERROR_NOT_SUPPORTED:
            return "Operation not supported or unimplemented on this platform";
        case LIBUSB_ERROR_OTHER:
            return "Other error";

        default:
            return "Unknown error";
    }
} 
#endif

// ----------------------------------------------------------------------------
// Hardware
// ----------------------------------------------------------------------------

Hardware::Hardware()
{
#ifdef USB_DEBUG
    cout<<"USB debug enabled, remove the '#define USB_DEBUG 3' line in hardware.cpp to disable it"<<endl;
    libusb_set_debug(NULL, USB_DEBUG);
#endif

    // init all internal variables
    m_handle = NULL;
    m_pCallBack = NULL;
    m_hwCurrent = HW_NONE;
    m_abortOperations = false;
    m_modeReadEndpoint = m_modeWriteEndpoint = LIBUSB_TRANSFER_TYPE_INTERRUPT;
}

Hardware::~Hardware()
{
    disconnect();
}

bool Hardware::connect(UppMainWindow* CB, HardwareType hwtype)
{
    int retcode;

    m_abortOperations=false;
    m_handle=NULL;
    m_pCallBack=CB;

    if (hwtype == HW_UPP)
    {
        // search first the UPP and then, if no UPP is found, the bootloader

        m_handle = libusb_open_device_with_vid_pid(NULL, UPP_VENDOR, UPP_PRODUCT);
        if (m_handle)
        {
            // found the UPP programmer
            hwtype=HW_UPP;
        }
        else
        {
            // failed to open this device, choose the next one
            m_handle = libusb_open_device_with_vid_pid(NULL, BOOTLOADER_VENDOR, BOOTLOADER_PRODUCT);
            if (m_handle)
                hwtype=HW_BOOTLOADER;       // found the UPP bootloader
        }
    }
    else
    {
        // search first the bootloader and then, if no bootloader is found, the UPP

        m_handle = libusb_open_device_with_vid_pid(NULL, BOOTLOADER_VENDOR, BOOTLOADER_PRODUCT);
        if (m_handle)
        {
            // found the UPP bootloader
            hwtype=HW_BOOTLOADER;
        }
        else
        {
            // failed to open this device, choose the next one
            m_handle = libusb_open_device_with_vid_pid(NULL, UPP_VENDOR, UPP_PRODUCT);
            if (m_handle)
                hwtype=HW_UPP;       // found the UPP
        }
    }

    if (m_handle==NULL)
    {
        m_hwCurrent = HW_NONE;
        return false;
    }

    // we've found some hardware!

    m_hwCurrent = hwtype;
#if 0                    // FIXME: why is this needed??
    if ( (retcode=libusb_reset_device(m_handle)) != LIBUSB_SUCCESS )
    {
        wxLogError(_("Couldn't reset the USB device interface: %s"), libusb_strerror((libusb_error)retcode));
        m_hwCurrent = HW_NONE;
        m_handle = NULL;
        return false;
    }

    libusb_close(m_handle);
    if (hwtype == HW_UPP)
        m_handle = libusb_open_device_with_vid_pid(NULL, UPP_VENDOR, UPP_PRODUCT);
    else
        m_handle = libusb_open_device_with_vid_pid(NULL, BOOTLOADER_VENDOR, BOOTLOADER_PRODUCT);

    if (!m_handle)
    {
        m_hwCurrent = HW_NONE;
        return false;
    }
#endif
    
    // NOTE: the firmware in the UPP has a single configuration with index #1;
    //       see the CFG01.bCfgValue in uc_code/usbdsc.c
    if ( (retcode=libusb_set_configuration(m_handle, 1)) != LIBUSB_SUCCESS )
    {
        wxLogError(_("Error setting configuration of the USB device: %s"), libusb_strerror((libusb_error)retcode));
        m_hwCurrent = HW_NONE;
        m_handle = NULL;
        return false;
    }

    // claim the USB interface
    // NOTE: the firmware in the UPP has a single interface with index #0:
    //       see the CFG01.bInterfaceNumber in uc_code/usbdsc.c
    if ( (retcode=libusb_claim_interface(m_handle, 0)) != LIBUSB_SUCCESS )
    {
        wxLogError(_("Error claiming the USB device interface: %s"), libusb_strerror((libusb_error)retcode));
        m_hwCurrent = HW_NONE;
        m_handle = NULL;
        return false;
    }
    
    // TODO: as libusb-1.0 docs in the "caveats" page say we should check here that the interface we claimed
    //       still has the same configuration we set previously...
	// 		 but anyway, usbpicprog will never switch 

    // now save the endpoint mode for both the READ_ENDPOINT and the WRITE_ENDPOINT
#if 0
    if ( (retcode=libusb_get_descriptor(m_handle, LIBUSB_DT_ENDPOINT, ENDPOINT_INDEX, (unsigned char*)&ed, sizeof(ed))) != LIBUSB_SUCCESS )
    {
        wxLogError(_("Error getting USB endpoint descriptor: %s"), libusb_strerror((libusb_error)retcode));
        m_hwCurrent = HW_NONE;
        m_handle = NULL;
        return false;
    }
    
    m_modeReadEndpoint = ed.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK;
    
    if ( (retcode=libusb_get_descriptor(m_handle, LIBUSB_DT_ENDPOINT, WRITE_ENDPOINT, (unsigned char*)&ed, sizeof(ed))) != LIBUSB_SUCCESS )
    {
        wxLogError(_("Error getting USB endpoint descriptor: %s"), libusb_strerror((libusb_error)retcode));
        m_hwCurrent = HW_NONE;
        m_handle = NULL;
        return false;
    }
    
    m_modeWriteEndpoint = ed.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK;
#else
    // IMPORTANT: currently the firmware does not support retrieving the endpoint descriptors so we have to hardcode
    //            the type of the endpoints here:
	if(m_hwCurrent==HW_UPP)
	    m_modeReadEndpoint = m_modeWriteEndpoint = LIBUSB_TRANSFER_TYPE_INTERRUPT;
	else 
		m_modeReadEndpoint = m_modeWriteEndpoint = LIBUSB_TRANSFER_TYPE_BULK;
#endif    
    
    // everything completed successfully:

    wxASSERT(m_handle != NULL && m_hwCurrent != HW_NONE);

    return true;
}

bool Hardware::disconnect()
{
    bool success = true;

    if (m_handle)
    {
        success &= libusb_release_interface(m_handle, 0) == LIBUSB_SUCCESS;
        libusb_close(m_handle);
        m_handle = NULL;
    }
    m_hwCurrent = HW_NONE;

    return success;
}

void Hardware::statusCallBack(int value) const
{
    if (m_pCallBack)
        m_pCallBack->updateProgress(value);
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
        return nBytes;

    statusCallBack (100);
    return (int)msg[0];
}

int Hardware::bulkErase(PicType* picType, bool doRestoreCalRegs)
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
        if(doRestoreCalRegs)
            msg[1]=0x00;
        else 
            msg[1]=0xAA;
        
        if (writeString(msg,2) < 0)
            return 0;

        // read back the reply
        int nBytes = readString(msg,1);
        if (nBytes < 0)
            return nBytes;

        //backupOscCalBandGap(picType);
        statusCallBack (100);
        return (int)msg[0];
    }
    else // hardware is HW_BOOTLOADER
    {
        if (!picType->ok()) return -1;
        HexFile hf(picType);
        if (write(TYPE_DATA, &hf, picType) < 0)
            return -1;

        statusCallBack(50);

        for (unsigned int address=0x800; address<picType->CodeSize; address+=64)
        {
            if (m_abortOperations)
                return OPERATION_ABORTED;

            BootloaderPackage bootloaderPackage;
            bootloaderPackage.data[bp_cmd]=CMD_BOOT_ERASE;
            bootloaderPackage.data[bp_size]=1; // only one block of 64 bytes supported by bootloader
            bootloaderPackage.data[bp_addrU]=(unsigned char)((address>>16)&0xFF);
            bootloaderPackage.data[bp_addrH]=(unsigned char)((address>>8)&0xFF);
            bootloaderPackage.data[bp_addrL]=(unsigned char)(address&0xFF);

            if (writeString(bootloaderPackage.data,5) < 0)
                return -1;
            if (readString(msg,5) < 0)
                return -1;

            statusCallBack(100);
        }

        return 1;   // OK
    }
}

int Hardware::backupOscCalBandGap(PicType *picType)
{
	unsigned char msg[64]={0,0};
    if((picType->picFamily!=P12F629)&&(picType->picFamily!=P12F508))	//back up osccal and bandgap registers for those devices
        return -1;

	int OscCalAddress=0x3FF;
	
	if(picType->Name.Contains("12F508"))OscCalAddress=0x204;
	if(picType->Name.Contains("12F509"))OscCalAddress=0x404;
	if(picType->picFamily==P12F629)OscCalAddress=0x3FF;
	if (readBlock(TYPE_CODE, msg , OscCalAddress, 2, 3) <= 0)
		return -1;

	picType->OscCal = (((unsigned int)msg[0]&0xFF)|((((unsigned int)msg[1])<<8)&0x3F00));
    
	if(picType->picFamily==P12F629)
	{
		if (readBlock(TYPE_CONFIG, msg , 0x2007, 2, 3 ) <= 0)
			return -1;

		picType->BandGap = ((((unsigned int)msg[1])<<8)&0x3000);
	}
	else picType->BandGap = 0;
    return 1;
}

int Hardware::restoreOscCalBandGap(PicType *picType, int OscCal, int BandGap)
{
    unsigned char msg[64];
    if((picType->picFamily!=P12F629)&&(picType->picFamily!=P12F508))	//back up osccal and bandgap registers for those devices
        return -1;
	
	int address=0;
	if(picType->Name.Contains("12F508"))address = 0x1ff;
	else if(picType->Name.Contains("12F509"))address = 0x3ff;
	else if(picType->picFamily==P12F629)address = 0x3ff;


	msg[0]=(unsigned char)(OscCal&0xFF);
	msg[1]=(unsigned char)((OscCal>>8)&0xFF);
	if (writeBlock(TYPE_CODE, msg, address, 2, 3) <= 0)
		return -1;
	if(picType->picFamily==P12F629)
	{
		msg[0]=0xFF;
		msg[1]=(unsigned char)((BandGap&0x03)<<4)|0x0F;
	    
		if (writeBlock(TYPE_CONFIG, msg, 0x2007, 2, 3) <= 0)
			return -1;
	}
	return 1;
}

int Hardware::read(MemoryType type, HexFile *hexData, PicType *picType, unsigned int numberOfBytes, HexFile *verifyData)
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

    if (numberOfBytes<memorySize)
        memorySize=numberOfBytes;
    
    if (memorySize==0)
	{
        return 0;       // no code/config/data to read
	}
    // create a temporary array to store read data
    vector<int> mem;
    mem.resize(memorySize, 0xFF);

    // how big is each block?
    unsigned int blockSizeHW;
    switch(type)
    {
        case TYPE_CONFIG:
            if(picType->bitsPerWord()==24)
            {
                blockSizeHW=BLOCKSIZE_CONFIG_DSPIC_READ;
                break;
            }
        case TYPE_CODE:
            if (m_hwCurrent == HW_BOOTLOADER)
                blockSizeHW=BLOCKSIZE_BOOTLOADER;
            else 
            {
                switch(picType->bitsPerWord())
                {
                    case 24:
                        blockSizeHW=BLOCKSIZE_CODE_DSPIC_READ;
                        break;
                    default:
                        blockSizeHW=BLOCKSIZE_CODE;
                        break;
                }
            }
            break;
        case TYPE_DATA:
            if (m_hwCurrent == HW_BOOTLOADER)
                blockSizeHW=BLOCKSIZE_BOOTLOADER;
            else 
            {
                switch(picType->bitsPerWord())
                {
                    case 24:
                        blockSizeHW=BLOCKSIZE_DATA_DSPIC_READ;
                        break;
                    default:
                        blockSizeHW=BLOCKSIZE_DATA;
                        break;
                }
            }
            break;
    }
    statusCallBack (0);
    if (type == TYPE_DATA && m_hwCurrent == HW_BOOTLOADER)
        return 0;   // TODO implement readData for bootloader
    if (type == TYPE_CONFIG && m_hwCurrent == HW_BOOTLOADER)
        return 0;   // Better write no configuration words in bootloader;
                    // it's unsafe: you might destroy the bootloader

    int nBytes=0, ret;
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
        if (picType->bitsPerWord()==14)
            currentBlockCounter /= 2;
        if (type==TYPE_CONFIG)
            currentBlockCounter+=picType->ConfigAddress;
        unsigned int blocksize;
        if (memorySize > (blockcounter+blockSizeHW))
            blocksize = blockSizeHW;
        else 
            blocksize = memorySize-blockcounter;

        // do read the block
        /*if(type==TYPE_CONFIG)
            cout<<"CurrentBlockCounter: "<<std::hex<<currentBlockCounter<<endl;*/
        unsigned char dataBlock[BLOCKSIZE_MAXSIZE];
        if ((ret = readBlock(type, dataBlock, currentBlockCounter, blocksize, blocktype)) <= 0)
		{
			return -1;      // failed reading this block; stop here
		}
        nBytes += ret;

        // move read data in the temporary array
        for (unsigned int i=0; i<blocksize; i++)
        {
            if (memorySize > (blockcounter+i))
                mem[blockcounter+i]=((int)(dataBlock[i]&0xFF));
            else
            {
                wxLogError(_("Trying to read memory outside allowed area: bc+i=%d; memory size=%d"), 
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
                if(picType->Name.IsSameAs("18F2450")||
                    picType->Name.IsSameAs("18F4450")) blockSizeHW=BLOCKSIZE_CODE_PIC18F2450;
                else if(picType->Name.IsSameAs("18F2221")||
                    picType->Name.IsSameAs("18F2321")||
                    picType->Name.IsSameAs("18F4221")||
                    picType->Name.IsSameAs("18F4321")) blockSizeHW=BLOCKSIZE_CODE_PIC18F2221;
                switch(picType->bitsPerWord())
                {
                    case 24:
                        blockSizeHW=BLOCKSIZE_CODE_DSPIC;
                        break;
                    default:
                        blockSizeHW=BLOCKSIZE_CODE;
                        break;
                }
            }
            break;
        case TYPE_DATA:
            if (m_hwCurrent == HW_BOOTLOADER)
                blockSizeHW=BLOCKSIZE_BOOTLOADER;
            else 
            {
                switch(picType->bitsPerWord())
                {
                    case 24:
                        blockSizeHW=BLOCKSIZE_DATA_DSPIC;
                        break;
                    default:
                        blockSizeHW=BLOCKSIZE_DATA;
                        break;
                }
            }
            break;
        case TYPE_CONFIG:
            switch(picType->bitsPerWord())
            {
                case 24:
                    blockSizeHW=BLOCKSIZE_CONFIG_DSPIC;
                    break;
                default:
                    blockSizeHW=BLOCKSIZE_CONFIG;
                    break;
            }
            break;
    }

    statusCallBack (0);
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
                dataBlock[i]=0xFF;
        }

        // set the type of this block
        unsigned int blocktype = BLOCKTYPE_MIDDLE;
        if (blockcounter == 0)
            blocktype |= BLOCKTYPE_FIRST;
        if (((int)memory->size()-(int)blockSizeHW) <= (int)blockcounter)
            blocktype |= BLOCKTYPE_LAST;
        if (m_abortOperations)
            blocktype |= BLOCKTYPE_LAST;

        

        unsigned int currentBlockCounter=blockcounter;
        if (picType->bitsPerWord()==14)
            currentBlockCounter /= 2;
        if(type==TYPE_CONFIG)
            currentBlockCounter+=picType->ConfigAddress;
        
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

        if (retCode <= 0)
            return -1;          // generic error (e.g. failed USB communication)

        if (m_abortOperations && (blocktype&BLOCKTYPE_LAST)==BLOCKTYPE_LAST)
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
        wxLogWarning(_("No data to verify"));

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
    {
        PicType picBoot = PicType::FindPIC(("18F2550"));
        cout<<"Bootloader Devid: "<<std::hex<<picBoot.DevId<<endl;
        return picBoot.DevId;
    }
    
    PicType pic16 = PicType::FindPIC(("16F628A"));
    if(setPicType(&pic16)<0)
        return -1;

    int devId=readId();
    if(devId<0)
        return -1;

    PicType picType = PicType::FindPIC(devId);

    if(picType.ok())
        return devId; 
    else
    {
    
        // need to set hardware to PIC18, no matter which one
        PicType pic18 = PicType::FindPIC(("18F2550"));
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
            PicType pic30 = PicType::FindPIC(("P30F1010"));
            if (setPicType(&pic30) < 0)
                return -1;

            devId=readId();
                cout<<"Devid PIC30: "<<std::hex<<(devId|0x20000)<<std::dec<<endl;
            if (devId < 0)
                return -1;
            
            PicType picType = PicType::FindPIC(devId|0x20000);
            if (picType.ok())
                return devId|0x20000;
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
            firmwareVersion->versionString=("");
            firmwareVersion->major=0;
            firmwareVersion->minor=0;
            firmwareVersion->release=0;
            return -1;
        }
        
        // read back the reply
        int nBytes = readString(msg,64);
        if (nBytes < 0)
        {
            firmwareVersion->versionString=("");
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

    int nBytes, retcode;
    if (m_modeReadEndpoint == LIBUSB_TRANSFER_TYPE_INTERRUPT)  
        retcode = libusb_interrupt_transfer(m_handle, READ_ENDPOINT, msg, size, &nBytes, USB_OPERATION_TIMEOUT);
    else
        retcode = libusb_bulk_transfer(m_handle, READ_ENDPOINT, msg, size, &nBytes, USB_OPERATION_TIMEOUT);

    if (retcode != LIBUSB_SUCCESS)
    {
        wxLogError(_("USB error while reading: %s"), libusb_strerror((libusb_error)retcode));
        return -1;
    }
    return nBytes;
}

int Hardware::writeString(const unsigned char* msg, int size) const
{
    if (m_handle == NULL) return -1;

    int nBytes, retcode;
    if (m_modeWriteEndpoint == LIBUSB_TRANSFER_TYPE_INTERRUPT)
        retcode = libusb_interrupt_transfer(m_handle, WRITE_ENDPOINT, (unsigned char*)msg, size, &nBytes, USB_OPERATION_TIMEOUT);
    else 
        retcode = libusb_bulk_transfer(m_handle, WRITE_ENDPOINT, (unsigned char*)msg, size, &nBytes, USB_OPERATION_TIMEOUT);

    if (retcode != LIBUSB_SUCCESS || nBytes < size)
    {
        wxLogError(_("USB error while writing to device: %d bytes, errCode: %d; %s"), size, nBytes, 
                   libusb_strerror((libusb_error)retcode));
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
            uppPackage.data[up_cmd]=CMD_READ_CODE;
            break;
        case TYPE_DATA:
            uppPackage.data[up_cmd]=CMD_READ_DATA;
            break;
        }
        uppPackage.data[up_size]=size;
        uppPackage.data[up_addrU]=(unsigned char)((address>>16)&0xFF);
        uppPackage.data[up_addrH]=(unsigned char)((address>>8)&0xFF);
        uppPackage.data[up_addrL]=(unsigned char)(address&0xFF);
        uppPackage.data[up_blocktype]=(unsigned char)lastblock;

        // send the command
        nBytes = writeString(uppPackage.data,6);
        if (nBytes < 0)
		{
            return nBytes;
		}
        // read back the bytes
        nBytes = readString(msg,size);
    }
    else
    {
        BootloaderPackage bootloaderPackage;

        switch (type)
        {
        case TYPE_CODE:
            bootloaderPackage.data[bp_cmd]=CMD_BOOT_READ_CODE;
            break;
        case TYPE_CONFIG:
            bootloaderPackage.data[bp_cmd]=CMD_BOOT_READ_CONFIG;
            break;
        case TYPE_DATA:
            bootloaderPackage.data[bp_cmd]=CMD_BOOT_READ_DATA;
            break;
        }

        bootloaderPackage.data[bp_size]=size;
        bootloaderPackage.data[bp_addrU]=(unsigned char)((address>>16)&0xFF);
        bootloaderPackage.data[bp_addrH]=(unsigned char)((address>>8)&0xFF);
        bootloaderPackage.data[bp_addrL]=(unsigned char)(address&0xFF);

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
            delete [] tmpmsg;
            return nBytes;
        }
        /*for(int i=0;i<size+6;i++)
            cout<<std::hex<<(int)tmpmsg[i]<<" ";
        cout<<endl;*/

        memcpy(msg,tmpmsg+5,nBytes);
        delete [] tmpmsg;
    }
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
            uppPackage.data[up_cmd]=CMD_WRITE_CODE;
            break;
        case TYPE_CONFIG:
            //cout<<"Write config block, address: "<<address<<",size: "<<size<<", lastblock: "<<lastblock<<endl;
            uppPackage.data[up_cmd]=CMD_WRITE_CONFIG;
            break;
        case TYPE_DATA:
            //cout<<"Write data block, address: "<<address<<",size: "<<size<<", lastblock: "<<lastblock<<endl;
            uppPackage.data[up_cmd]=CMD_WRITE_DATA;
            break;
        }
        uppPackage.data[up_size]=size;
        uppPackage.data[up_addrU]=(unsigned char)((address>>16)&0xFF);
        uppPackage.data[up_addrH]=(unsigned char)((address>>8)&0xFF);
        uppPackage.data[up_addrL]=(unsigned char)(address&0xFF);
        uppPackage.data[up_blocktype]=(unsigned char)lastblock;
        memcpy(uppPackage.data+up_data,msg,size);

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
            bootloaderPackage.data[bp_cmd]=CMD_BOOT_WRITE_CODE;
            break;
        case TYPE_DATA:
            bootloaderPackage.data[bp_cmd]=CMD_BOOT_WRITE_DATA;
            break;
        default:
            //nothing to do for CONFIG
            break;
        }

        bootloaderPackage.data[bp_size]=size;
        bootloaderPackage.data[bp_addrU]=((address>>16)&0xFF);
        bootloaderPackage.data[bp_addrH]=((address>>8)&0xFF);
        bootloaderPackage.data[bp_addrL]=(address&0xFF);
        memcpy(bootloaderPackage.data + bp_data ,msg,size);

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
    if ( usb_get_driver_np(m_handle, 0, dname, 255)<0 ) return;
    //  log(Log::DebugLevel::Normal, QString("  a driver \"%1\" is already installed...").arg(dname));
#if defined(LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP) && LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
    usb_detach_kernel_driver_np(m_handle, 0);
    // log(Log::DebugLevel::Normal, "  try to detach it...");
    if ( usb_get_driver_np(m_handle, 0, dname, 255)<0 ) return;
    // log(Log::DebugLevel::Normal, "  failed to detach it");
#endif
#endif
}


double Hardware::getVppVoltage()
{
    int iValue;
    if (m_hwCurrent != HW_UPP) return -1;
    if (m_handle == NULL) return -1;
    unsigned char msg[64];
    msg[0]=CMD_GET_PIN_STATUS;
    msg[1]=SUBCMD_PIN_VPP_VOLTAGE;
    writeString(msg,2);
    readString(msg,64);
    iValue = ((int)msg[0])+(((int)msg[1])<<8);

    float fValue = ((float)iValue)*(147.0/47.0)*(5.0/1024.0);

    //cout<<"ADC Value: "<<iValue<<" "<<fValue<<" [V]"<<endl;
    return fValue;

}


PIN_STATE Hardware::getPinState(SUBCMD_PIN pin)
{
    if (m_hwCurrent != HW_UPP) return PIN_STATE_INVALID;
    if (m_handle == NULL) return PIN_STATE_INVALID;
    unsigned char msg[64];
    msg[0]=CMD_GET_PIN_STATUS;
    msg[1]=pin;
    writeString(msg,2);
    readString(msg,64);
    return (PIN_STATE) msg[0];
}

int Hardware::setPinState(SUBCMD_PIN pin, PIN_STATE state)
{
    if (m_hwCurrent != HW_UPP) return -1;
    if (m_handle == NULL) return -1;
    unsigned char msg[64];
    msg[0]=CMD_SET_PIN_STATUS;
    msg[1]=pin;
    msg[2]=state;
    writeString(msg,3);
    readString(msg,64);
    return (int) msg[0];
}

int Hardware::debug()
{
    if (m_handle == NULL) return -1;

    statusCallBack (0);

    if (m_hwCurrent == HW_UPP)
    {
        cout<<"usage\ncat debugfile | src/usbpicprog -d\n\nwith the in the debugfile:\ndcccccc"<<endl;
        while(1)
        {
            cout<<"> ";
            string debugData;
            cin>>debugData;
            if(debugData.length()==0)break;
            int debugCommand,debugValue;
            sscanf(debugData.c_str(),"%1X%6X",&debugCommand,&debugValue);
            unsigned char msg[64];
            msg[0]=0xA0;
            switch(debugCommand)
            {	
                case 0:
                    msg[1]=0;
                    if(writeString(msg,2)<0)cout<<"Error writing string"<<endl;
                    if(readString(msg,1)<0)cout<<"Error reading string"<<endl;
                    cout<<"Set VPP to HIGH: ";
                    if(msg[0]==1)cout<<"Ok"<<endl;
                    else cout <<"fail"<<endl;
                    break;
                case 1:
                    msg[1]=1;
                    if(writeString(msg,2)<0)cout<<"Error writing string"<<endl;
                    if(readString(msg,1)<0)cout<<"Error reading string"<<endl;
                    cout<<"Set VPP to LOW: ";
                    if(msg[0]==1)cout<<"Ok"<<endl;
                    else cout <<"fail"<<endl;
                    break;
                case 2:
                    msg[1]=2;
                    msg[2]=(unsigned char)debugValue;
                    msg[3]=(unsigned char)(debugValue>>8);
                    msg[4]=(unsigned char)(debugValue>>16);
                    if(writeString(msg,5)<0)cout<<"Error writing string"<<endl;
                    if(readString(msg,1)<0)cout<<"Error reading string"<<endl;
                    cout<<"Sending command: 0x"<<std::hex<<debugValue<<", ";
                    if(msg[0]==1)cout<<"Ok"<<endl;
                    else cout <<"fail"<<endl;
                    break;
                case 3:
                    msg[1]=3;
                    if(writeString(msg,2)<0)cout<<"Error writing string"<<endl;
                    if(readString(msg,2)<0)cout<<"Error reading string"<<endl;
                    cout<<"Read from dsPIC: 0x"<<std::hex<<(int)msg[0]<<" "<<(int)msg[1]<<endl;
                    break;
            }
        }
    }
    return 0;
}


