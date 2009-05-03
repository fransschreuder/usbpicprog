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

                if ((dev->descriptor.idVendor == UPP_VENDOR) && (dev->descriptor.idProduct == UPP_PRODUCT))
                {
                    m_handle = usb_open(dev);
                    if (!m_handle)
                        continue; // failed to open this device, choose the next one

                    hwtype=HW_UPP;
                    break; // found usbpicprog, exit the for loop
                }

                if ((dev->descriptor.idVendor == BOOTLOADER_VENDOR) && (dev->descriptor.idProduct == BOOTLOADER_PRODUCT))
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

                if ((dev->descriptor.idVendor == BOOTLOADER_VENDOR) && (dev->descriptor.idProduct == BOOTLOADER_PRODUCT))
                {
                    m_handle = usb_open(dev);
                    if (!m_handle)
                        continue; // failed to open this device, choose the next one

                    hwtype=HW_BOOTLOADER;
                    break; // found bootloader, exit the for loop
                }
                if ((dev->descriptor.idVendor == UPP_VENDOR) && (dev->descriptor.idProduct == UPP_PRODUCT))
                {
                    m_handle = usb_open(dev);
                    if (!m_handle)continue; // failed to open this device, choose the next one

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
        cerr<<"Couldn't reset interface"<<endl;

    usb_close(m_handle);
    m_handle = usb_open(dev);

    if(!m_handle)
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
        cerr<<"Error setting configuration"<<endl;
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
        cerr<<"Interface "<<old<<" not present: using "<<m_nInterfaceNumber<<endl;
    }

    m_interface = &(configd.interface[i].altsetting[0]);


    // claim the USB interface

    if ( usb_claim_interface(m_handle, m_nInterfaceNumber)<0 )
    {
        cerr<<"Error claiming interface"<<endl;
        m_hwCurrent = HW_NONE;
        m_handle = NULL;
        return;
    }

    // everything completed successfully:

    wxASSERT(m_handle != NULL && m_hwCurrent != HW_NONE);
}

Hardware::~Hardware()
{
    if(m_handle)
    {
        usb_release_interface(m_handle, m_nInterfaceNumber);
        usb_close(m_handle);

        m_handle=NULL;
        m_hwCurrent=HW_NONE;
    }
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

/*give the hardware the command to switch to a certain pic algorithm*/
int Hardware::setPicType(PicType* picType)
{
    char msg[64];

    if (!picType->ok()) return -1;
    if (m_hwCurrent == HW_BOOTLOADER) return -1;

    statusCallBack (0);
    msg[0]=CMD_SET_PICTYPE;
    msg[1]=picType->picFamily;

    int nBytes=-1;
    if (m_handle != NULL)
    {
        if(writeString(msg,2) < 0)
            return -1;       // failure

        nBytes = readString(msg,1);

        if (nBytes < 0)
        {
            cerr<<"Usb Error"<<endl;
            return nBytes;
        }
        else
        {
            // cout<<"Id: "<<hex<<((int)msg[0]&0xFF)<<" "<<hex<<((int)msg[1]&0xFF)<<", "<<nBytes<<" bytes"<<endl;
            statusCallBack (100);
            return (int)msg[0];
        }
    }

    return nBytes;      // failure
}

/*Erase all the contents (code, data and config) of the pic*/
int Hardware::bulkErase(PicType* picType)
{
    char msg[64];
    int nBytes=-1;

    if (!picType->ok()) return -1;

    statusCallBack (0);
    if (m_handle != NULL)
    {
        if (m_hwCurrent == HW_UPP)
        {
            if(m_abortOperations)return OPERATION_ABORTED;
            msg[0]=CMD_ERASE;
            if(writeString(msg,1) < 0)
            {
                return 0;
            }
            nBytes = readString(msg,1);

            if (nBytes < 0)
            {
                cerr<<"Usb Error"<<endl;
                return nBytes;
            }
            else
            {
                statusCallBack (100);
                return (int)msg[0];
            }
        }
        else // hardware is HW_BOOTLOADER
        {
            HexFile * hf = new HexFile(picType);
            if(writeData(hf,picType) < 0)return -1;
            statusCallBack(50);
            delete hf;
            for (unsigned int address=0x800; address<picType->CodeSize; address+=64)
            {
                if(m_abortOperations)return OPERATION_ABORTED;
                BootloaderPackage bootloaderPackage;
                bootloaderPackage.fields.cmd=CMD_BOOT_ERASE;
                bootloaderPackage.fields.size=1; // only one block of 64 bytes supported by bootloader
                bootloaderPackage.fields.addrU=(unsigned char)((address>>16)&0xFF);
                bootloaderPackage.fields.addrH=(unsigned char)((address>>8)&0xFF);
                bootloaderPackage.fields.addrL=(unsigned char)(address&0xFF);
                if (writeString(bootloaderPackage.data,5) < 0)
                {
                    return -1;
                }
                if(readString(msg,5) < 0)return -1;
                statusCallBack(100);
            }
            return 1;
        }
    }
    return nBytes;
}

/*Read the code memory from the pic (starting from address 0 into *hexData*/
int Hardware::readCode(HexFile *hexData,PicType *picType)
{
    int nBytes,blocksize,BLOCKSIZE_HW;
    nBytes=-1;
    vector<int> mem;

    if (!picType->ok()) return -1;

    mem.resize(picType->CodeSize, 0xFF);
    char dataBlock[BLOCKSIZE_MAXSIZE];
    int blocktype;
    if(m_abortOperations)return OPERATION_ABORTED;
    if (m_hwCurrent == HW_BOOTLOADER)BLOCKSIZE_HW=BLOCKSIZE_BOOTLOADER;
    else BLOCKSIZE_HW=BLOCKSIZE_CODE;
    statusCallBack (0);
    if (m_handle != NULL)
    {
        nBytes=0;
        for(unsigned int blockcounter=0;blockcounter<picType->CodeSize;blockcounter+=BLOCKSIZE_HW)
        {
            statusCallBack ((blockcounter*100)/((signed)picType->CodeSize));
            blocktype=BLOCKTYPE_MIDDLE;
            if(blockcounter==0)blocktype|=BLOCKTYPE_FIRST;
            if((picType->CodeSize-BLOCKSIZE_HW)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
            if(m_abortOperations)blocktype|=BLOCKTYPE_LAST;
            int	currentBlockCounter=blockcounter;
            if(!picType->is16Bit())currentBlockCounter/=2;
            if(picType->CodeSize>(blockcounter+BLOCKSIZE_HW))blocksize=BLOCKSIZE_HW;
            else blocksize=picType->CodeSize-blockcounter;
            nBytes+=readCodeBlock(dataBlock,currentBlockCounter,blocksize,blocktype);
            for(int i=0;i<blocksize;i++)
            {
                if(picType->CodeSize>(blockcounter+i))
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
                    // return -1;
                }
            }
            if(m_abortOperations)break;
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
int Hardware::writeCode(HexFile *hexData,PicType *picType)
{
    int nBytes;
    int BLOCKSIZE_HW;
    unsigned char dataBlock[BLOCKSIZE_MAXSIZE];
    int blocktype;

    if (!picType->ok()) return -1;

    if(m_abortOperations)return OPERATION_ABORTED;
    if (m_hwCurrent == HW_BOOTLOADER)BLOCKSIZE_HW=BLOCKSIZE_BOOTLOADER;
    else BLOCKSIZE_HW=BLOCKSIZE_CODE;
    if (m_handle != NULL)
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
            if(m_abortOperations)blocktype|=BLOCKTYPE_LAST;
            int	currentBlockCounter=blockcounter;
            if(!picType->is16Bit())currentBlockCounter/=2;
            nBytes=writeCodeBlock(dataBlock,currentBlockCounter,BLOCKSIZE_HW,blocktype);
            if (m_hwCurrent == HW_UPP)
            {
                if(nBytes==3) return -3;	// something not implemented in firmware :(
                if(nBytes==4) return -4;	// verify error
                if(((blocktype==BLOCKTYPE_MIDDLE)||(blocktype==BLOCKTYPE_FIRST))&&(nBytes!=2))return -2; // should ask for next block
                if((blocktype==BLOCKTYPE_LAST)&&(nBytes!=1))return -1;	// should say OK
            }
            if(m_abortOperations)break;
        }
    }
    else return -5;
    return 0;
}

/* read the Eeprom Data area of the pic into *hexData->dataMemory */
int Hardware::readData(HexFile *hexData,PicType *picType)
{
    int nBytes,blocksize;
    nBytes=-1;
    vector<int> mem;

    if (!picType->ok()) return -1;

    mem.resize(picType->DataSize);
    int BLOCKSIZE_HW;
    if (m_hwCurrent == HW_BOOTLOADER)BLOCKSIZE_HW=BLOCKSIZE_BOOTLOADER;
    else BLOCKSIZE_HW=BLOCKSIZE_CODE;

    char dataBlock[BLOCKSIZE_MAXSIZE];
    int blocktype;
    if(m_abortOperations)return OPERATION_ABORTED;
    statusCallBack (0);
    if (m_hwCurrent == HW_BOOTLOADER)return 0; // TODO implement readData for bootloader
    if(picType->DataSize==0)return 0;// no data to read
    if (m_handle != NULL)
    {
        for(unsigned int blockcounter=0;blockcounter<picType->DataSize;blockcounter+=BLOCKSIZE_HW)
        {
            statusCallBack ((blockcounter*100)/((signed)picType->DataSize));
            blocktype=BLOCKTYPE_MIDDLE;
            if(blockcounter==0)blocktype|=BLOCKTYPE_FIRST;
            if((picType->DataSize-BLOCKSIZE_HW)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
            if(m_abortOperations)blocktype|=BLOCKTYPE_LAST;
            if(picType->DataSize>(blockcounter+BLOCKSIZE_HW))blocksize=BLOCKSIZE_HW;
            else blocksize=picType->DataSize-blockcounter;

            nBytes+=readDataBlock(dataBlock,blockcounter,blocksize,blocktype);
            for(int i=0;i<blocksize;i++)
            {
                if(picType->DataSize>(blockcounter+i))
                {
                    mem[blockcounter+i]=(unsigned char)(dataBlock[i]&0xFF);
                }
                else
                {
                    cerr<<"Trying to read memory outside Data area"<<endl;
    // 				return -1;
                }
            }
            if(m_abortOperations)break;
        }
        hexData->putDataMemory(mem);
    }
    return nBytes;

}


/*write the Eeprom data from *hexData->dataMemory into the pic*/
int Hardware::writeData(HexFile *hexData,PicType *picType)
{
    int nBytes;
    int BLOCKSIZE_HW;
    if (m_hwCurrent == HW_BOOTLOADER)BLOCKSIZE_HW=BLOCKSIZE_BOOTLOADER;
    else BLOCKSIZE_HW=BLOCKSIZE_CODE;

    if (!picType->ok()) return -1;

    unsigned char dataBlock[BLOCKSIZE_MAXSIZE];
    int blocktype;
    if(m_abortOperations)return OPERATION_ABORTED;
    statusCallBack (0);
    // if (m_hwCurrent == HW_BOOTLOADER)return 0; // TODO implement writeData for bootloader
    if (m_handle != NULL)
    {
        nBytes=0;
        for(int blockcounter=0;blockcounter<(signed)hexData->getDataMemory().size();blockcounter+=BLOCKSIZE_HW)
        {
            statusCallBack ((blockcounter*100)/((signed)hexData->getDataMemory().size()));
            for(int i=0;i<BLOCKSIZE_HW;i++)
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
            if(((signed)hexData->getDataMemory().size()-BLOCKSIZE_HW)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
            if(m_abortOperations)blocktype|=BLOCKTYPE_LAST;
            nBytes=writeDataBlock(dataBlock,blockcounter,BLOCKSIZE_HW,blocktype);
            if (m_hwCurrent == HW_UPP)
            {
                if(nBytes==3) return -3;	// something not implemented in firmware :(
                if(((blocktype==BLOCKTYPE_MIDDLE)||(blocktype==BLOCKTYPE_FIRST))&&(nBytes!=2))return -2; // should ask for next block
                if((blocktype==BLOCKTYPE_LAST)&&(nBytes!=1))return -1;	// should say OK
                if(m_abortOperations)break;
            }
        }
    }
    else return -4;
    return 0;
}

/* Read the configuration words (and user ID's for PIC16 dev's) */
int Hardware::readConfig(HexFile *hexData,PicType *picType)
{
    int nBytes,blocksize;
    nBytes=-1;

    if (!picType->ok()) return -1;

    vector<int> mem;
    mem.resize(picType->ConfigSize);
    char dataBlock[BLOCKSIZE_CONFIG];
    int blocktype;
    if(m_abortOperations)return OPERATION_ABORTED;
    statusCallBack (0);
    if (m_hwCurrent == HW_BOOTLOADER)return 0; // Better write no configuration words in bootloader, it's unsafe, you might destroy the bootloader
    if (m_handle != NULL)
    {
        for(unsigned int blockcounter=0;blockcounter<picType->ConfigSize;blockcounter+=BLOCKSIZE_CONFIG)
        {
            statusCallBack ((blockcounter*100)/((signed)picType->ConfigSize));
            blocktype=BLOCKTYPE_MIDDLE;
            if(blockcounter==0)blocktype|=BLOCKTYPE_FIRST;
            if((picType->ConfigSize-BLOCKSIZE_CONFIG)<=blockcounter)blocktype|=BLOCKTYPE_LAST;
            if(m_abortOperations)blocktype|=BLOCKTYPE_LAST;
            int	currentBlockCounter=blockcounter;
            if(!picType->is16Bit())currentBlockCounter/=2;
            if(picType->ConfigSize>(blockcounter+BLOCKSIZE_CONFIG))blocksize=BLOCKSIZE_CONFIG;
            else blocksize=picType->ConfigSize-blockcounter;
            nBytes+=readConfigBlock(dataBlock,currentBlockCounter+picType->ConfigAddress,blocksize,blocktype);
            for(int i=0;i<blocksize;i++)
            {
                if(picType->ConfigSize>(blockcounter+i))
                {
                    mem[blockcounter+i]=(unsigned char)(dataBlock[i]&0xFF);
// 					cerr<<hex<<(int)dataBlock[i]<<" "<<dec;
                }
                else
                {
                    cerr<<"Trying to read memory outside Config area: bc+i="<<blockcounter+i<<"Configsize="<<picType->ConfigSize<<endl;
// 					return -1;
                }
            }
            if(m_abortOperations)break;
        }
/*		for(int i=0;i<picType->ConfigSize;i++)
            mem[i]&=picType->ConfigMask[i];*/
        hexData->putConfigMemory(mem);
    }
    cerr<<endl;
    return nBytes;
}

/*Writes the configuration words (and user ID's for PIC16 dev's)*/
int Hardware::writeConfig(HexFile *hexData,PicType *picType)
{
    int nBytes;
    unsigned char dataBlock[BLOCKSIZE_CONFIG];
    int blocksize;
    int blocktype;
    if(m_abortOperations)return OPERATION_ABORTED;
    statusCallBack (0);
    /*if (m_hwCurrent == HW_BOOTLOADER)
    {
        statusCallBack(100);
        return 0; // It's not smart to write config bits in the bootloader
    }*/
    if (m_handle != NULL)
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
            if(m_abortOperations)blocktype|=BLOCKTYPE_LAST;
            int	currentBlockCounter=blockcounter;
            if(!picType->is16Bit())currentBlockCounter/=2;
            nBytes=writeConfigBlock(dataBlock,currentBlockCounter+picType->ConfigAddress,blocksize,blocktype);
            if(nBytes==3) return -3;	// something not implemented in firmware :(
            if(((blocktype==BLOCKTYPE_MIDDLE)||(blocktype==BLOCKTYPE_FIRST))&&(nBytes!=2))return -2; // should ask for next block
            if((blocktype==BLOCKTYPE_LAST)&&(nBytes!=1))return -1;	// should say OK
            if(m_abortOperations)break;
        }
    }
    else return -4;
    return 0;
}

/*Reads the whole PIC and checks if the data matches hexData*/
VerifyResult Hardware::verify(HexFile *hexData, PicType *picType, bool doCode, bool doConfig, bool doData)
{
    // if (!picType->ok()) return -1;

    VerifyResult res;
    HexFile *verifyHexFile = new HexFile;
    if(doCode)
    {
        if(readCode(verifyHexFile,picType) < 0)
        {
            res.Result=VERIFY_USB_ERROR;
            res.DataType=TYPE_CODE;
            return res;
        }
    }
    if(doData)
    {
        if(readData(verifyHexFile,picType) < 0)
        {
            res.Result=VERIFY_USB_ERROR;
            res.DataType=TYPE_DATA;
            return res;
        }
    }
    if(doConfig)// &&(m_hwCurrent != HW_UPP))
    {
        if(readConfig(verifyHexFile,picType) < 0)
        {
            res.Result=VERIFY_USB_ERROR;
            res.DataType=TYPE_CONFIG;
            return res;
        }
    }
    if ((hexData->getCodeMemory().size()+
        hexData->getDataMemory().size()+
        hexData->getConfigMemory().size())>0) // there should be at least some data in the file
    {
        if(doCode)
        {
            res = hexData->verifyCode(verifyHexFile);
            if (res.Result != VERIFY_SUCCESS)
                return res;
        }
        if(doData)
        {
            res = hexData->verifyData(verifyHexFile);
            if (res.Result != VERIFY_SUCCESS)
                return res;
        }
        if(doConfig&&(m_hwCurrent == HW_UPP)) // it's no use to verify config for the bootloader
        {
            res = hexData->verifyConfig(verifyHexFile);
            if (res.Result != VERIFY_SUCCESS)
                return res;
        }
        res.Result=VERIFY_SUCCESS;
    }
    else
    {
        res.Result=VERIFY_OTHER_ERROR;
        cout<<"No data to verify"<<endl;
    }
    return res;
}

/*Reads the whole PIC and checks if it is blank*/
VerifyResult Hardware::blankCheck(PicType *picType)
{
    // if (!picType->ok()) return -1;

    HexFile* blankHexFile=new HexFile(picType);
    return verify(blankHexFile,picType);
}

int Hardware::autoDetectDevice()
{
    if (m_hwCurrent == HW_BOOTLOADER)
        return -1;// 0x11240;     // PIC18F2550

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
        // try PIC16
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

    return -1;
}

/*Return a string containing the firmware version of usbpicprog*/
int Hardware::getFirmwareVersion(char* msg) const
{
    if (m_hwCurrent == HW_UPP)
    {
        msg[0]=CMD_FIRMWARE_VERSION;
        int nBytes=-1;
        statusCallBack (0);
        if (m_handle != NULL)
        {
            if(writeString(msg,1) < 0)
            {
                return -1;
            }
            nBytes = readString(msg,64);

            if (nBytes < 0)
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
        if (m_handle != NULL)
        {
            cout<<"writeString"<<endl;
            if(writeString(msg,5) < 0)
            {
                return -1;
            }
            cout<<"readString"<<endl;
            nBytes = readString(msg,64);

            if (nBytes < 0)
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


// ----------------------------------------------------------------------------
// Hardware - private functions
// ----------------------------------------------------------------------------

int Hardware::readString(char* msg, int size) const
{
    if (m_handle == NULL) return -1;

    int nBytes;
    if ( endpointMode(WRITE_ENDPOINT)==Interrupt )  // FIXME: shouldn't we test READ_ENDPOINT?
        nBytes = usb_interrupt_read(m_handle,READ_ENDPOINT,msg,size,USB_OPERATION_TIMEOUT);
    else
        nBytes = usb_bulk_read(m_handle,READ_ENDPOINT,msg,size,USB_OPERATION_TIMEOUT);

    if (nBytes < 0)
    {
        cerr<<"Usb Error while reading: "<<nBytes<<endl;
        cerr<<usb_strerror()<<endl;
        return -1;
    }

    return nBytes;
}

int Hardware::writeString(const char* msg, int size) const
{
    if (m_handle == NULL) return -1;

    int nBytes;
    if ( endpointMode(WRITE_ENDPOINT)==Interrupt )
        nBytes = usb_interrupt_write(m_handle,WRITE_ENDPOINT,(char*)msg,size,USB_OPERATION_TIMEOUT/10);
    else 
        nBytes = usb_bulk_write(m_handle,WRITE_ENDPOINT,(char*)msg,size,USB_OPERATION_TIMEOUT/10);

    if (nBytes < size)
    {
        cerr<<"Usb Error while writing to device: "<<size<<" bytes, errCode: "<<nBytes<<endl;
        cerr<<usb_strerror()<<endl;
        return -1;
    }

    return nBytes;
}

int Hardware::readId()
{
    if (m_handle == NULL) return -1;

    statusCallBack (0);

    // send to the hardware the READ-ID command
    char msg[64];
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

int Hardware::readBlock(MemoryType type, char* msg, int address, int size, int lastblock)
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
        char* tmpmsg = new char[size+5];
        nBytes = readString(tmpmsg,size+5) - 5;
        if (nBytes < 0)
        {
            cerr<<"Usb Error"<<endl;
            delete [] tmpmsg;
            return nBytes;
        }

        memcpy(msg,tmpmsg+5,nBytes);
        delete [] tmpmsg;
    }

    if (nBytes < 0)
        cerr<<"Usb Error"<<endl;
    return nBytes;
}

int Hardware::writeBlock(MemoryType type, unsigned char* msg, int address, int size, int lastblock)
{
    if (m_handle == NULL) return -1;

    char resp_msg[64];
    if (m_hwCurrent == HW_UPP)
    {
        UppPackage uppPackage;
        switch (type)
        {
        case TYPE_CODE:
            uppPackage.fields.cmd=CMD_WRITE_CODE;
            break;
        case TYPE_CONFIG:
            uppPackage.fields.cmd=CMD_WRITE_CONFIG;
            break;
        case TYPE_DATA:
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
            for(int i=0;i<size+6;i++)
                cout<<hex<<(int)uppPackage.data[i]<<" "<<dec;
            cout<<endl;
        }*/

        int nBytes = writeString(uppPackage.data,size+6);
        if (nBytes < 0)
            return nBytes;

        nBytes = readString(resp_msg,1);
        if (nBytes < 0)
            cerr<<"Usb Error"<<endl;
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

        if(address<0x800)
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

