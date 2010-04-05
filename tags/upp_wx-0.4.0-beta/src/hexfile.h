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

#ifndef HEXFILE_H
#define HEXFILE_H

#include <vector>
using namespace std;

#include "pictype.h"

/**
    The identifiers of the different records contained into an HEX file.
    See HexFile for more info.
*/
typedef enum {
    DATA=0,       ///< Data record, contains data and 16-bit address.
    ENDOFFILE=1,  ///< End Of File record, Usually ':00000001FF'.
    EXTADDR=4     ///< Extended Linear Address Record, allowing for fully 32 bit addressing.
                  ///< The address field is 0000, the byte count is 02. The two data bytes
                  ///< represent the upper 16 bits of the 32 bit address, when combined with
                  ///< the address of the 00 type record.
} RecordType;

/** 
    The type of memory area to read/write/verify.
    See the VerifyResult structure.
*/
typedef enum
{
    TYPE_CODE=0,
    TYPE_DATA,
    TYPE_CONFIG
} MemoryType;

/** 
    Return values for a verify operation stored inside VerifyResult structure. 
*/
typedef enum
{
    VERIFY_SUCCESS=0,
    VERIFY_MISMATCH,
    VERIFY_USB_ERROR,
    VERIFY_OTHER_ERROR
} VRResult;

/** 
    The return value of Hardware::verify() and HexFile::verifyCode() functions.
*/
typedef struct
{
    VRResult Result;
    MemoryType DataType;
    int Address;
    int Read;
    int Expected;
} VerifyResult;


/**
    This class represents an HEX file.

    Supports loading and saving in the Intel HEX format (the one commonly
    used by PIC compilers): http://en.wikipedia.org/wiki/Intel_HEX.

    Note that this class is used by UppMainWindow to store the current status
    of the code/config/data bytes; this class however doesn't store any info
    about the PIC model for which we store the code/config/data bytes;
    see the Pic class for that.
*/
class HexFile
{
public:
    HexFile(PicType* picType=NULL, const char* filename=NULL);

    /**
        Resets this instance so that it can store code/config/data memory bytes for
        the given @a picType PIC model.
        
        Fills the memory areas with 0xFF bytes; then calls trimData().
    */
    void newFile(PicType* picType);

    bool open(PicType* picType, const char* filename);
    bool reload(PicType* picType);
    bool saveAs(PicType* picType, const char* filename);
    bool save(PicType* picType);

    /**
        @name Memory manipulation functions
    */
    //@{
    
    void putMemory(MemoryType type, const vector<int>& mem, const PicType* picType);
    void putMemory(MemoryType type, int address, int mem, const PicType* picType);

    
    //@}
    
    
    /**
        @name Memory read functions
    */
    //@{

	vector<int>& getMemory(MemoryType type);
	int getMemory(MemoryType type, int address) const;
    
    //@}
    
    VerifyResult verify(MemoryType type, const HexFile* other,bool skipBootSection) const;
    
    /**
        Dumps into the @a output wxString the current code/config/data memory contents.
    */
    void print(wxString* output, PicType* picType);
    
    /**
        Masks code, config and data memory areas with the @c 0xFF or @c 0x3F mask
        respectively for 16 and 8 bit devices.
    */
    void trimData(PicType* picType);
    const char* getFileName() const { return m_filename.c_str(); }
    bool hasFileName() const { return !m_filename.empty(); }
    
    /**
        Returns true if any memory byte was modified (through the putXXXMemory functions)
        since last call to newFile(), open(), reload(). saveAs() or save().
    */
    bool wasModified() const { return m_bModified; }

	/**
	 Two registers, OscCal and Bandgap are backed up during erase. Using this function they can be substituted into the hex file.
	 */
	void putOscCalBandGap(PicType *picType);

private:
    wxString m_filename;
    bool m_bModified;

    /**
        @name Memory areas
        
        Each integer stored in these arrays represent a single word of code/data/config 
        memory (obviously for 8bit PICs a word is 1 byte; for 16bit PICs a word is 2 bytes).
    */
    //@{
    
    vector<int> m_codeMemory;
    vector<int> m_dataMemory;
    vector<int> m_configMemory;
    
    //@}

    /**
        The masks to be used for the various elements of the #m_configMemory array.
        
        Whenever you compare or set or read the i-th byte of #m_configMemory you should
        always mask it with the i-th element of #m_configMask.
        
        This array is set by calcConfigMask().
    */
    vector<int> m_configMask;
    
    bool calcCheckSum(int byteCount, int address, 
                      RecordType recordType, const vector<int>& lineData, int checkSum);
    void calcConfigMask(const PicType* pic);
    void makeLine(int address, RecordType recordType, 
                  const vector<int>& lineData, char* output_line);
};

#endif //HEXFILE_H
