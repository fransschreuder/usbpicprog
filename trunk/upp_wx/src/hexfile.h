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
    HexFile(PicType* picType=NULL,const char* filename=NULL);
    ~HexFile();

    /**
        Resets this instance and fills it with 0xFF bytes; then calls trimData().
    */
    int newFile(PicType* picType);
    
    int open(PicType* picType, const char* filename);
    int reload(PicType* picType);
    int saveAs(PicType* picType, const char* filename);
    int save(PicType* picType);
    
    /**
        @name Memory manipulation functions
    */
    //@{
    
    void putCodeMemory(vector<int> &mem);
    void putCodeMemory(int address, int mem);
    void putDataMemory(vector<int> &mem);
    void putDataMemory(int address, int mem);
    void putConfigMemory(vector<int> &mem);
    void putConfigMemory(int address, int mem);
    
    //@}
    
    
    /**
        @name Memory read functions
    */
    //@{
    
    vector<int> &getCodeMemory();
    int getCodeMemory(int address);
    vector<int> &getDataMemory();
    int getDataMemory(int address);
    vector<int> &getConfigMemory();
    int getConfigMemory(int address);
    
    //@}
    
    
    /**
        Dumps into the @a output string the current code/config/data memory contents.
    */
    void print(string* output, PicType* picType);
    
    /**
        Masks code, config and data memory areas with the @c 0xFF or @c 0x3F mask
        respectively for 16 and 8 bit devices.
    */
    void trimData(PicType* picType);

    const char* getFileName() const { return m_filename.c_str(); }
    bool hasFileName() const { return m_filename[0] != '\0'; }
    bool wasModified() const { return m_bModified; }

private:
    string m_filename;
    bool m_bModified;

    vector<int> m_codeMemory;
    vector<int> m_dataMemory;
    vector<int> m_configMemory;

    bool calcCheckSum(int byteCount,int address, RecordType recordType,vector<int> &lineData, int checkSum);
    void makeLine(int address, RecordType recordType, vector<int> &lineData, char* output_line);
};

#endif //HEXFILE_H
