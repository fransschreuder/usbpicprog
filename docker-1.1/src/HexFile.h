/* Copyright (C) 2002-2003  Mark Andrew Aikens <marka@desert.cx>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * $Id: HexFile.h,v 1.2 2006/04/08 14:49:45 gaufille Exp $
 */
#ifndef __HexFile_h
#define __HexFile_h

#include <stdio.h>
#include "DataBuffer.h"

/** \file */


/** Modes for open a HexFile */
typedef enum {
	HEXFILE_READ,
	HEXFILE_WRITE
} hexfile_mode_t;


/** This base class allows a uniform access method to different hex file
 * types. To read a hex file, use the load() method to automatically
 * determine the file type. In order to write to a hex file, instantiate the
 * subclass which implements that filetype directly.
 */
class HexFile {
public:
	/** Opens a hex file for reading. The file type will be detected
	 * automatically and the correct subclass will be instantiated and
	 * returned.
	 * \param filename The name of the hex file to open.
	 * \returns A HexFile object that can be used to read the file data.
	 */
	static HexFile *load(char *filename);

	/** On writing, writes any finishing data to the file and closes the file.
	 * On reading, just closes the file. */
	virtual ~HexFile();

	/** Reads data from the hex file into the DataBuffer. This should only
	 * be called when the hex file has been opened for reading.
	 * \param buf A DataBuffer to store the data in.
	 * \throws logic_error If the hex file was opened for writing.
	 * \throws runtime_error Contains a textual description of the error.
	 */
	virtual void read(DataBuffer& buf) = 0;

	/** Writes data to the hex file.
	 * \param buf The DataBuffer containing the data to write.
	 * \param start The starting offset in buf to write.
	 * \param len The number of words, starting at start, to write.
	 * \throws logic_error If the hex file was opened for reading.
	 * \throws runtime_error Contains a textual description of the error.
	 */
	virtual void write(DataBuffer& buf, long start, long len) = 0;

protected:
	/** Protected constructor can only be called from subclasses.
	 * \param filename The name of the file to read/write.
	 * \param mode The mode of the file, indicating reading or writing.
	 */
	HexFile(char *filename, hexfile_mode_t mode=HEXFILE_WRITE);

	/** Protected constructor can only be called from subclasses.
	 * \param fp A handle to the file to read/write.
	 * \param mode The mode of the file, indicating reading or writing.
	 */
	HexFile(FILE *fp, hexfile_mode_t mode=HEXFILE_READ);

	/** The mode that this hex file was opened in. */
	hexfile_mode_t mode;

	/** A file handle for the hex file */
	FILE *fp;
};


/** A HexFile subclass which implements reading/writing to Intex hex16 files.
 */
class HexFile_ihx16 : public HexFile {
public:
	/** Constructor. Call this directly to open an Intel hex16 file for
	 * reading or writing.
	 * \param filename The name of the hex file.
	 * \param mode The mode to open the file in.
	 */
	HexFile_ihx16(char *filename, hexfile_mode_t mode=HEXFILE_WRITE);

	/** Constructor which takes a file handle.
	 * \param fp A handle to the file to read/write.
	 * \param mode The mode of the file, indicating reading or writing.
	 */
	HexFile_ihx16(FILE *fp, hexfile_mode_t mode=HEXFILE_READ);

	/** On writing, writes the EOF marker and closes the file. On reading,
	 * just closes the file. */
	~HexFile_ihx16();

	void read(DataBuffer& buf);
	void write(DataBuffer& buf, long start, long len);

private:
	unsigned long eaddr;		/* Extended segment address */
};


/** A HexFile subclass which implements reading/writing to Intel hex and
 * extended hex files.
 */
class HexFile_ihx8 : public HexFile {
public:
	/** Constructor. Call this directly to open an Intel hex8 file for
	 * reading or writing.
	 * \param filename The name of the hex file.
	 * \param mode The mode to open the file in.
	 */
	HexFile_ihx8(char *filename, hexfile_mode_t mode=HEXFILE_WRITE);

	/** Constructor which takes a file handle.
	 * \param fp A handle to the file to read/write.
	 * \param mode The mode of the file, indicating reading or writing.
	 */
	HexFile_ihx8(FILE *fp, hexfile_mode_t mode=HEXFILE_READ);

	/** On writing, writes the EOF marker and closes the file. On reading,
	 * just closes the file. */
	~HexFile_ihx8();

	void read(DataBuffer& buf);
	void write(DataBuffer& buf, long start, long len);

private:
	unsigned int uaddr;		/* Upper 16-bits of address when writing */
};


#endif
