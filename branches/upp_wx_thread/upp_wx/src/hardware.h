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

#ifndef HARDWARE_H
#define HARDWARE_H

// IMPORTANT: do not include here <usb.h>; it will give problems on
//            Windows when building with Unicode mode enabled

#define UPP_VENDOR 0x04D8
#define UPP_PRODUCT 0x000E

#define BOOTLOADER_VENDOR 0x04D8
#define BOOTLOADER_PRODUCT 0x000B

#include "hexfile.h"
#include "pictype.h"

#define CMD_ERASE 0x10
#define CMD_READ_ID 0x20
#define CMD_WRITE_CODE 0x30
#define CMD_READ_CODE 0x40
#define CMD_WRITE_DATA 0x50
#define CMD_READ_DATA 0x60
#define CMD_WRITE_CONFIG 0x70
#define CMD_SET_PICTYPE 0x80
#define CMD_FIRMWARE_VERSION 0x90

#define CMD_BOOT_FIRMWARE_VERSION 0x00
#define CMD_BOOT_READ_CODE 0x01
#define CMD_BOOT_WRITE_CODE 0x02
#define CMD_BOOT_ERASE 0x03
#define CMD_BOOT_READ_DATA 0x04
#define CMD_BOOT_WRITE_DATA 0x05
#define CMD_BOOT_READ_CONFIG 0x06
#define CMD_BOOT_WRITE_CONFIG 0x07
#define CMD_BOOT_UPDATE_LED 0x32
#define CMD_BOOT_RESET 0xFF

#define BLOCKTYPE_MIDDLE 0
#define BLOCKTYPE_FIRST 1
#define BLOCKTYPE_LAST 2
#define BLOCKTYPE_FIRST_LAST 3

#define BLOCKSIZE_DATA 8
#define BLOCKSIZE_CONFIG 8
#define BLOCKSIZE_CODE 32
#define BLOCKSIZE_BOOTLOADER 16

#define ENDPOINT 1
#define READ_ENDPOINT (ENDPOINT|USB_ENDPOINT_IN)
#define WRITE_ENDPOINT (ENDPOINT|USB_ENDPOINT_OUT)


// forward declaration:
class UppMainWindow;
struct usb_dev_handle;
struct usb_interface_descriptor;

/*Upp package is the data header which is sent to usbpicprog*/
typedef union _UppPackage
{
    struct _fields
    {
        unsigned cmd:8;   //one of the CMD_ defines above
        unsigned size:8;  //size of the datafield
        unsigned addrU:8; //the address is devided in 3 bytes, Upper, High and Low
        unsigned addrH:8;
        unsigned addrL:8;
        unsigned blocktype:8; //The blocktype can be middle, first or last (or first|last)
        unsigned char dataField[32];
    }fields;
    char data[38];
}UppPackage;

typedef union _BootloaderPackage
{
    struct _fields
    {
        unsigned cmd:8;   //one of the CMD_ defines above
        unsigned size:8;  //size of the datafield
        unsigned addrL:8;                   //Little-indian order
        unsigned addrH:8;
        unsigned addrU:8;
        unsigned char dataField[32];
    }fields;
    char data[37];
}BootloaderPackage;

typedef enum _VRResult
{
    VERIFY_SUCCESS=0,
    VERIFY_MISMATCH,
    VERIFY_USB_ERROR,
    VERIFY_OTHER_ERROR
}VRResult;

typedef enum _VRDataType
{
    TYPE_CODE=0,
    TYPE_DATA,
    TYPE_CONFIG
}VRDataType;

typedef struct _VerifyResult
{
    VRResult Result;
    VRDataType DataType;
    int Address;
    int Read;
    int Expected;
}VerifyResult;

typedef enum _HardwareType
{
    HW_NONE=0,
    HW_UPP,
    HW_BOOTLOADER,
    HARDWARETYPE_NUM
}HardwareType;


/*The class Hardware connects to usbpicprog using libusb.*/
class Hardware
{
public:
    enum EndpointMode { Bulk = 0, Interrupt, Control, Isochronous, Nb_EndpointModes };

/* The CB pointer links with the parent UppMainWindow which is used for updating the progress bar.
   If initiated with no argument, progress is not updated*/
    Hardware(UppMainWindow* CB=NULL, HardwareType SetHardware=HW_UPP);
    ~Hardware();

/*Give the hardware the command to switch to a certain pic algorithm*/
    int setPicType(PicType* picType);
/*Erase all the contents (code, data and config) of the pic*/
    int bulkErase(PicType *picType);
/*Read the code memory from the pic (starting from address 0 into *hexData*/
    int readCode(HexFile *hexData,PicType *picType);
/*Write the code memory area of the pic with the data in *hexData */
    int writeCode(HexFile *hexData,PicType *picType);
/*Read the Eeprom Data area of the pic into *hexData->dataMemory */
    int readData(HexFile *hexData,PicType *picType);
/*Write the Eeprom data from *hexData->dataMemory into the pic*/
    int writeData(HexFile *hexData,PicType *picType);
/*Read the configuration words (and user ID's for PIC16 dev's) */
    int readConfig(HexFile *hexData,PicType *picType);
/*Writes the configuration words (and user ID's for PIC16 dev's)*/
    int writeConfig(HexFile *hexData,PicType *picType);
/*Reads the whole PIC and checks if the data matches hexData*/
    VerifyResult verify(HexFile *hexData, PicType *picType, bool doCode=true, bool doConfig=true, bool doData=true);
/*Reads the whole PIC and checks if it is blank*/
    VerifyResult blankCheck(PicType *picType);
/*This function does nothing but reading the devid from the PIC, call it the following way:
    Hardware* hardware=new Hardware();
    int devId=hardware->autoDetectDevice();
    PicType* picType=new PicType(devId);
    hardware->setPicType(picType); */
    int autoDetectDevice(void);

/*Returns the current mode of the USB endpoint*/
    EndpointMode endpointMode(int ep) const;
/*Get the hardware type, because we autodetect hardware if the type given through SetHardware is not detected*/
    HardwareType getHardwareType(void) const;
/*Check if usbpicprog is successfully connected to the usb bus and initialized*/
    bool connected(void) const;
/*Returns a string containing the firmware version of usbpicprog*/
    int getFirmwareVersion(char* msg) const;
/*Returns the type of the hardware which we are currently attached to*/
    HardwareType getCurrentHardware(void) const {return CurrentHardware;}

private:
    void tryToDetachDriver(void);
/*Read a string of data from usbpicprog (through interrupt_read)*/
    int readString(char* msg,int size) const;
/*Send a string of data to usbpicprog (through interrupt write)*/
    int writeString(const char* msg,int size) const;
/*Private function called by autoDetectDevice */
    int readId(void);
/*Private function to read one block of code memory*/
    int readCodeBlock(char * msg,int address,int size,int lastblock);
/*Private function to write one block of code memory*/
    int writeCodeBlock(unsigned char * msg,int address,int size,int lastblock);
/*Private function to read one block of config memory*/
    int readConfigBlock(char * msg, int address, int size, int lastblock);
/*Private function to write one block of config memory*/
    int writeConfigBlock(unsigned char * msg,int address,int size,int lastblock);
/*Private function to read one block of data memory*/
    int readDataBlock(char * msg,int address,int size,int lastblock);
/*Private function to write one block of data memory*/
    int writeDataBlock(unsigned char * msg,int address,int size,int lastblock);
/*When Hardware is constructed, ptCallBack is initiated by a pointer
to UppMainWindow, this function calls the callback function
to update the progress bar*/
    void statusCallBack(int value) const;

/*Device handle containing information about Usbpicprog while connected*/
    usb_dev_handle* _handle;
/*Pointer to the class UppMainWindow in order to call back the statusbar*/
    UppMainWindow* ptCallBack;

/*USB interface*/
    int bInterfaceNumber;
    const usb_interface_descriptor *privateInterface;

/*Are we connected to the UPP bootloader or to the UPP programmer?*/
    HardwareType CurrentHardware;
};

#endif //HARDWARE_H
