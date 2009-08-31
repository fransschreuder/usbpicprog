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

#include "hexfile.h"
#include "pictype.h"

#define UPP_VENDOR 0x04D8
#define UPP_PRODUCT 0x000E

#define BOOTLOADER_VENDOR 0x04D8
#define BOOTLOADER_PRODUCT 0x000B

#define CMD_ERASE 0x10
#define CMD_READ_ID 0x20
#define CMD_WRITE_CODE 0x30
#define CMD_READ_CODE 0x40
#define CMD_WRITE_DATA 0x50
#define CMD_READ_DATA 0x60
#define CMD_WRITE_CONFIG 0x70
#define CMD_SET_PICTYPE 0x80
#define CMD_FIRMWARE_VERSION 0x90
#define CMD_DEBUG 0xA0

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
#define BLOCKSIZE_CODE_DSPIC 24
#define BLOCKSIZE_DATA_DSPIC 32
#define BLOCKSIZE_BOOTLOADER 16

#define BLOCKSIZE_MAXSIZE 128
    // only used when allocating temporary buffers

#define ENDPOINT 1
#define READ_ENDPOINT (ENDPOINT|USB_ENDPOINT_IN)
#define WRITE_ENDPOINT (ENDPOINT|USB_ENDPOINT_OUT)

#define OPERATION_ABORTED 2

#define USB_OPERATION_TIMEOUT 5000
    // timeout in milliseconds for USB operations


// forward declaration:
class UppMainWindow;
struct usb_dev_handle;
struct usb_interface_descriptor;


typedef struct
{
	wxString versionString;
	int major;
	int minor;
	int release;
	bool stableRelease;
	bool isBootloader;
}FirmwareVersion;

/** 
    UppPackage is the data header which is sent to the programmer hardware
*/
typedef union
{
    struct
    {
        unsigned cmd:8;   /// One of the CMD_ defines above
        unsigned size:8;  /// Size of the datafield
        unsigned addrU:8; /// The address is devided in 3 bytes, Upper, High and Low
        unsigned addrH:8;
        unsigned addrL:8;
        unsigned blocktype:8; /// The blocktype can be middle, first or last (or first|last)
        unsigned char dataField[32];
    } fields;
    unsigned char data[38];
} UppPackage;

/** 
    BootloaderPackage is the data header which is sent to the bootloader of
    the programmer hardware.
*/
typedef union
{
    struct
    {
        unsigned cmd:8;   /// One of the CMD_ defines above
        unsigned size:8;  /// Size of the datafield
        unsigned addrL:8; /// Little-endian order
        unsigned addrH:8;
        unsigned addrU:8;
        unsigned char dataField[32];
    } fields;
    unsigned char data[37];
} BootloaderPackage;

/** 
    The different types of hardware circuits supported by upp_wx.
*/
typedef enum
{
    /// No hardware connected.
    HW_NONE=0,

    /// The real programmer based on the PIC18F2550.
    HW_UPP,

    /// The bootloader of the programmer based on the PIC18F2550.
    HW_BOOTLOADER,

    HARDWARETYPE_NUM
} HardwareType;


/** 
    This class connects to the USB PIC programmer hardware using libusb.

    Libusb (http://libusb.wiki.sourceforge.net/) allows user level applications 
    to access USB devices regardless of OS.
    API docs for that library is at http://libusb.sourceforge.net/doc/

    This class opens two hardware endpoints for reading/writing to the connected
    hardware respectively identified by the @c READ_ENDPOINT and @c WRITE_ENDPOINT 
    constants.

    TODO: add const attributes to func arguments not modified
*/
class Hardware
{
public:
    /**
        The possible operation modes for an USB endpoint.
    */
    enum EndpointMode { 
        Bulk = 0, 
        Interrupt, 
        Control, 
        Isochronous, 

        /// An invalid endpoint mode.
        Nb_EndpointModes 
    };

    /** 
        Default constructor.

        @param CB
            Links this instance with the parent UppMainWindow window; this link is used 
            for updating the progress bar.
            If NULL is given, progress bar is not updated.
        @param hwtype
            Indicates the hardware type which should be searched first.
            If no hardware of the given type is found, then the other types of supported
            hardwares are searched.
    */
    Hardware(UppMainWindow* CB=NULL, HardwareType hwtype=HW_UPP);

    /**
        Releases the USB interface acquired in the ctor.
    */
    ~Hardware();

    /** 
        Gives the hardware the command to switch to a certain PIC programming algorithm.
        Returns the reply code from the attached hardware or negative value if there was an error.
    */
    int setPicType(PicType* picType);

    /** 
        Erases all the contents (code, data and config) of the PIC memory.
        Returns the reply code from the attached hardware or a negative value if there was an error.
    */
    int bulkErase(PicType *picType);

    /**
        Reads the code/config/data memory from the PIC into *hexData.
        If the device is a PIC16 and type==TYPE_CONFIG, then the user ID is read, too.

        Returns the number of bytes read or a negative value if an error occurred.
    */
    int read(MemoryType type, HexFile *hexData, PicType *picType, int numberOfBytes=0, HexFile *verifyData=NULL);

    /**
        Writes the code/config/data memory from *hexData into the PIC.
        If the device is a PIC16 and type==TYPE_CONFIG, then the user ID is written, too.

        Returns 1 or a negative value if an error occurred.
    */
    int write(MemoryType type, HexFile *hexData, PicType *picType);

    /** 
        Reads the whole PIC memory and checks if the config/data/code areas match 
        the relative contents of @a hexData. 
    */
    VerifyResult verify(HexFile *hexData, PicType *picType, 
                        bool doCode=true, bool doConfig=true, bool doData=true);

    /** Reads the whole PIC and checks if it is blank */
    VerifyResult blankCheck(PicType *picType);

    /** 
        This function does nothing but reading the device id from the PIC and then returning it.
        Like for others Hardware class functions, if an error occurs then a negative value is returned.
    */
    int autoDetectDevice();

    /** Returns the current mode of the USB endpoint */
    EndpointMode endpointMode(int ep) const;

    /** 
        Reads the firmware version of the connected hardware. 

        The firmware version is stored as a string in the given buffer which MUST be big enough
        (at least 64 chars).

        Returns the number of bytes read or a negative value if an error occurred.
    */
    int getFirmwareVersion(FirmwareVersion* firmwareVersion) const;

    /** Returns the type of the hardware which we are currently attached to. */
    HardwareType getCurrentHardware() const 
        { return m_hwCurrent; }

    /** 
        This function can be called with abort=true to make all time-consuming functions abort,
        call it with false to reset the abortion.
    */
    void abortOperations(bool abort)
        { m_abortOperations=abort; }

    /**
        Returns true if we aborted the last operation.
    */
    bool operationsAborted() const
        { return m_abortOperations; }

    /** Check if some supported hardware is successfully connected to the usb bus and initialized */
    bool connected() const
        { return m_handle != NULL; }

	int debug();

private:

    /**
        @name Private read/write helpers

        The functions of this group returns the number of bytes successfully read/write
        or -1 if there was an error.
    */
    //@{

    /** Read a string of data from the connected hardware (through usb_interrupt_read). */
    int readString(unsigned char* msg, int size) const;

    /** Send a string of data to the connected hardware (through usb_interrupt write). */
    int writeString(const unsigned char* msg, int size) const;

    /** Private function called by autoDetectDevice(). Returns the ID of the device to program. */
    int readId();

    /** 
        Read a block of a memory area sending the correct command to the hardware and reading
        back its reply. 
    */
    int readBlock(MemoryType type, unsigned char* msg, int address, int size, int lastblock);

    /** 
        Writes a block of a memory area sending the correct command to the hardware and reading
        back its reply. 
    */
    int writeBlock(MemoryType type, unsigned char* msg, int address, int size, int lastblock);

    //@}

    /** 
        When Hardware is constructed, ptCallBack is initiated by a pointer
        to UppMainWindow, this function calls the callback function to update 
        the progress bar.
    */
    void statusCallBack(int value) const;

    /** If the OS supports it, tries to detach an already existing driver */
    void tryToDetachDriver();

private:

    /** Pointer to the class UppMainWindow in order to call back the statusbar */
    UppMainWindow* m_pCallBack;

    /** Are we connected to the UPP bootloader or to the UPP programmer? */
    HardwareType m_hwCurrent;

    /** If this bool becomes true, all operations will be aborted */
    bool m_abortOperations;

private:    // libusb-related members

    /** Device handle containing information about Usbpicprog when it's connected */
    usb_dev_handle* m_handle;

    /** USB interface number. */
    int m_nInterfaceNumber;

    /** Libusb interface descriptor. */
    const usb_interface_descriptor *m_interface;
};

#endif //HARDWARE_H
