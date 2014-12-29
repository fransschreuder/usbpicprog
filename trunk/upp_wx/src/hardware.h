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

typedef enum
{
    CMD_ERASE = 0x10,
    CMD_READ_ID = 0x20,
    CMD_WRITE_CODE = 0x30,
    CMD_READ_CODE_OLD = 0x40,			// original read code/config command differentiated by address (and blocksize <= 8 except for PIC18F2221
    	    	    	    	    	    	//    and PIC18's don't distinguish code/config )
    CMD_WRITE_DATA = 0x50,
    CMD_READ_DATA = 0x60,
    CMD_WRITE_CONFIG = 0x70,
    CMD_SET_PICTYPE = 0x80,
    CMD_FIRMWARE_VERSION = 0x90,
    CMD_DEBUG = 0xA0,
    CMD_GET_PIN_STATUS = 0xB0,
    CMD_SET_PIN_STATUS = 0xC0,
    CMD_GET_PROTOCOL_VERSION = 0xD0,		// PROT_UPP1
    CMD_READ_CODE = 0xD1,			        // PROT_UPP1
    CMD_READ_CONFIG = 0xD2,			        // PROT_UPP1
    CMD_EXIT_TO_BOOTLOADER = 0xD3,		    // PROT_UPP1
    CMD_MREAD_CODE = 0xD4,                  // PROT_UPP2
	CMD_APPLY_SETTINGS = 0xD5,				// PROT_UPP3
	CMD_INVALID
}CMD_UPP;

typedef enum
{
	PROT_NONE,				    // not connected
	PROT_BOOTLOADER0,			// Connected to bootloader
	PROT_UPP0 = 100,			// Original command set to firmware
	PROT_UPP1,				    // deleted READ_CODE(0x40) added READ_CODE(0xD1),READ_CONFIG(0xD2),EXIT_TO_BOOTLOADER(0xD3)
	PROT_UPP2,                  // added multi-block read to READ_CODE(0xD4)
	PROT_UPP3,					// added configuration to limit VDD, VPP and PGD, PGC
	PROT_UPP4					// Changed address to 32 bit (was 24)
}PROTOCOL;

typedef enum
{
    SUBCMD_PIN_PGC = 0x01,
    SUBCMD_PIN_PGD = 0x02,
    SUBCMD_PIN_VDD = 0x03,
    SUBCMD_PIN_VPP = 0x04,
    SUBCMD_PIN_VPP_VOLTAGE = 0x05,
    SUBCMD_PIN_INVALID
}SUBCMD_PIN;

typedef enum
{
	PIN_STATE_0V = 0x01,
	PIN_STATE_3_3V = 0x02,
	PIN_STATE_5V = 0x03,
	PIN_STATE_12V = 0x04,
	PIN_STATE_FLOAT = 0x05,
	PIN_STATE_INPUT = 0x06,
	PIN_STATE_INVALID
}PIN_STATE;

typedef enum 
{
    CMD_BOOT_FIRMWARE_VERSION = 0x00,
    CMD_BOOT_READ_CODE = 0x01,
    CMD_BOOT_WRITE_CODE = 0x02,
    CMD_BOOT_ERASE = 0x03,
    CMD_BOOT_READ_DATA = 0x04,
    CMD_BOOT_WRITE_DATA = 0x05,
    CMD_BOOT_READ_CONFIG = 0x06,
    CMD_BOOT_WRITE_CONFIG = 0x07,
    CMD_BOOT_UPDATE_LED = 0x32,
    CMD_BOOT_RESET = 0xFF
}CMD_BOOT;

#define CONFIG_DISABLE_VDD_MASK 0x1
#define CONFIG_LIMIT_VPP_MASK 0x2
#define CONFIG_LIMIT_PGDPGC_MASK 0x4


#define BLOCKTYPE_MIDDLE 0
#define BLOCKTYPE_FIRST	 0x01
#define BLOCKTYPE_LAST	 0x02
#define BLOCKTYPE_CONFIG 0x10
#define BLOCKTYPE_CODE	 0x20
#define BLOCKTYPE_DATA	 0x40
#define BLOCKTYPE_FIRST_LAST (BLOCKTYPE_FIRST|BLOCKTYPE_LAST)

#define BLOCKSIZE_DATA 8
#define BLOCKSIZE_CONFIG 8
#define BLOCKSIZE_CODE 32
#define BLOCKSIZE_24XX04	16
#define BLOCKSIZE_24XX01	8
#define BLOCKSIZE_24XX00	1
#define BLOCKSIZE_CODE_PIC18F2450 16
#define BLOCKSIZE_CODE_PIC18F2221 8
#define BLOCKSIZE_CODE_DSPIC 48
#define BLOCKSIZE_CODE_DSPIC_READ 48
#define BLOCKSIZE_CONFIG_DSPIC_READ 2
#define BLOCKSIZE_CONFIG_DSPIC 2
#define BLOCKSIZE_DATA_DSPIC 32
#define BLOCKSIZE_DATA_DSPIC_READ 8
#define BLOCKSIZE_BOOTLOADER 16

#define BLOCKSIZE_MAXSIZE 128
    // only used when allocating temporary buffers

//! The index of the endpoint used for communication.
#define ENDPOINT_INDEX          1

//! The endpoint used for reading data from UPP bootloader/programmer.
#define READ_ENDPOINT           (ENDPOINT_INDEX|LIBUSB_ENDPOINT_IN)

//! The endpoint used for writing data to the UPP bootloader/programmer.
#define WRITE_ENDPOINT          (ENDPOINT_INDEX|LIBUSB_ENDPOINT_OUT)

//! Timeout in milliseconds for USB operations.
#define USB_OPERATION_TIMEOUT   3000

//! Return code for the Hardware class' functions when the operations have been aborted.
#define OPERATION_ABORTED       2


// forward declaration:
class UppMainWindow;
struct libusb_device_handle;


/**
    Structure to pass/transport easily a firmware version for the programmer hardware.
*/
typedef struct
{
    wxString versionString;
    int major;
    int minor;
    int release;
    bool stableRelease;
    bool isBootloader;
} FirmwareVersion;



/** 
    UppPackage is the data header which is sent to the programmer hardware.
*/
typedef struct
{
    /*struct
    {
        unsigned cmd:8;   /// One of the CMD_ defines above
        unsigned size:8;  /// Size of the datafield
        unsigned addrU:8; /// The address is devided in 3 bytes, Upper, High and Low
        unsigned addrH:8;
        unsigned addrL:8;
        unsigned blocktype:8; /// The blocktype can be middle, first or last (or first|last)
        unsigned char dataField[32];
    } fields;*/
    unsigned char data[57];
} UppPackage;


typedef enum 
{
	bp_cmd,
	bp_size,
	bp_addrL,
	bp_addrH,
	bp_addrU,
	bp_data
}BOOT_INDEX;
/** 
    BootloaderPackage is the data header which is sent to the bootloader of
    the programmer hardware.
*/
typedef struct
{
    /*struct
    {
        unsigned cmd:8;   /// One of the CMD_ defines above
        unsigned size:8;  /// Size of the datafield
        unsigned addrL:8; /// Little-endian order
        unsigned addrH:8;
        unsigned addrU:8;
		unsigned char dataField[32];
    } fields;*/
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
        Default constructor.
        Before using an Hardware instance you'll need to call connect() yourself.
    */
    Hardware();

    /**
        Calls disconnect().
    */
    ~Hardware();
    
    /**
        Attempts to connect to the specified hardware type.
        
        @param CB
            Links this instance with the parent UppMainWindow window; this link is used 
            for updating the progress bar.
            If NULL is given, progress bar is not updated.
        @param hwtype
            Indicates the hardware type which should be searched first.
            If no hardware of the given type is found, then the other types of supported
            hardwares are searched.
    */
    bool connect(UppMainWindow* CB=NULL, HardwareType hwtype=HW_UPP);
    
    /**
        Disconnects from the hardware currently attached, if there's any.
    */
    bool disconnect();

    /** 
        Gives the hardware the command to switch to a certain PIC programming algorithm.
        Returns the reply code from the attached hardware or negative value if there was an error.
    */
    int setPicType(PicType* picType);

	int applySettings(bool ConfigDisableVDD, bool ConfigLimitVPP, bool ConfigLimitPGDPGC);
	

    /** 
        Gives the hardware the command to reboot.
        	firmware boots into the bootloader.
        	bootloader reboots (and check bootloader/app switch).
    */
    int reboot(HardwareType what);

    /**
        Erases all the contents (code, data and config) of the PIC memory.
        Returns the reply code from the attached hardware or a negative value if there was an error.
    */
    int bulkErase(PicType *picType, bool doRestoreCalRegs);

	/**
		Backs up two calibration registers (OscCal and BandGap) for PIC12F629 and similar devices and 
		puts them in the corresponding variable @a picType.
	*/
	int backupOscCalBandGap(PicType *picType);

	/**
		Restores the two calibration registers for PIC12F629 and similar devices.
		See backupOscCalBandGap().
	*/
	int restoreOscCalBandGap(PicType *picType, int OscCal, int BandGap);
	
    /**
        Reads the code/config/data memory from the PIC into *hexData.
        If the device is a PIC16 and type==TYPE_CONFIG, then the user ID is read, too.

        Returns the number of bytes read or a negative value if an error occurred.
    */
    int read(MemoryType type, HexFile *hexData, PicType *picType, 
			 unsigned int numberOfBytes, HexFile *verifyData=NULL);

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

    /** Set MCLR to +5V to run target device*/
    bool runTarget();
    
    /** Set MCLR to 0V to stop target device*/
    bool stopTarget();

	/** Enter programming mode, according to selected PIC type */
	//bool enter_ICSP(); 

	/** Enter programming mode, according to selected PIC type */
	//bool exit_ICSP(); 
    
	/** 
        Reads the firmware version of the connected hardware. 

        The firmware version is stored as a string in the given buffer which MUST be big enough
        (at least 64 chars).

        Returns the number of bytes read or a negative value if an error occurred.
    */
    int getFirmwareVersion(FirmwareVersion* firmwareVersion);

    /** Returns the type of the hardware which we are currently attached to. */
    PROTOCOL getCurrentProtocol() const
        { return m_protocol; }

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

    /** Reads the A/D-converted analog voltage of the VPP pin and returns it as a double voltage */
    double getVppVoltage();

    /** Returns the pin status for the given pin */
	PIN_STATE getPinState(SUBCMD_PIN pin);

    /** Sends a command to the hardware to set a specific pin into the given status  */
	int setPinState(SUBCMD_PIN pin, PIN_STATE state);

    /** Just to debug the usbpicprog hardware / firmware, not for debugging the target PIC! */
	int debug();

private:

    /**
        @name Private read/write helpers

        The functions of this group returns the number of bytes successfully read/write
        or -1 if there was an error.
    */
    //@{

    /** Read a string of data from the connected hardware (through usb_interrupt_read). */
    int readString(unsigned char* msg, int size, bool noerror=false) const;

    /** Send a string of data to the connected hardware (through usb_interrupt write). */
    int writeString(const unsigned char* msg, int size, bool noerror=false) const;

    /** Private function called by autoDetectDevice(). Returns the ID of the device to program. */
    int readId();

    /** 
        Read a block of a memory area sending the correct command to the hardware and reading
        back its reply.
        
        Returns the number of bytes read or -1 on error.
    */
    int readBlock(MemoryType type, unsigned char* msg, int address, int size, int lastblock, int no_blocks=1);
    /**
        Read next block after a multi-block readBlock.

        Returns the number of bytes read or -1 on error.
    */
    int readNextBlock(unsigned char* msg, int size );

    /** 
        Writes a block of a memory area sending the correct command to the hardware and reading
        back its reply. 
        
        Returns the number of bytes read or -1 on error.
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

    //** contains the current version of command set used to communicate to the firmware
    PROTOCOL m_protocol;

private:    // libusb-related members

    /** Device handle containing information about Usbpicprog when it's connected */
    libusb_device_handle* m_handle;
    
    /** 
        The mode (interrupt or bulk) of the endpoint used for UPP->SW communications.
        Note that this variable should be of type "enum libusb_transfer_type" but 
        hardware.h doesn't include libusb.h directly so that the definition of that 
        enum is not available to the compiler here. Thus a generic int is used instead.
    */
    int m_modeReadEndpoint;
    
    /** 
        The mode (interrupt or bulk) of the endpoint used for SW->UPP communications.
        Note that this variable should be of type "enum libusb_transfer_type" but 
        hardware.h doesn't include libusb.h directly so that the definition of that 
        enum is not available to the compiler here. Thus a generic int is used instead.
    */
    int m_modeWriteEndpoint;
};

#endif //HARDWARE_H
