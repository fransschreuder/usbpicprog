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
#ifndef READHEXFILE_H
#define READHEXFILE_H


#include <vector>
//#include <string>
#include "pictype.h"

typedef enum _RecordType{
			DATA=0,// data record, contains data and 16-bit address. 
			ENDOFFILE=1,// End Of File record, Usually ':00000001FF'.
			EXTADDR =4// Extended Linear Address Record, allowing for fully 32 bit addressing. The address field is 0000, the byte count is 02. The two data bytes represent the upper 16 bits of the 32 bit address, when combined with the address of the 00 type record.
}RecordType;

using namespace std;

class ReadHexFile
{
	public:
		ReadHexFile(PicType* picType,const char* filename);
		ReadHexFile();
		~ReadHexFile();
		void open(PicType* picType, const char* filename);
		void reload(PicType* picType);
		void saveAs(PicType* picType,const char* filename);
	    void save(PicType* picType);
		void putCodeMemory(vector<int> mem);
		void putDataMemory(vector<int> mem);
		void putConfigMemory(vector<int> mem);
		vector<int> getCodeMemory(void);
		vector<int> getDataMemory(void);
		vector<int> getConfigMemory(void);
		
	private:
		char filenameToSave[512];
		vector<int> codeMemory;
		vector<int> dataMemory;
		vector<int> configMemory;
		bool calcCheckSum(int byteCount,int address, RecordType recordType,vector<int> &lineData, int checkSum);
};

#endif //READHEXFILE_H
