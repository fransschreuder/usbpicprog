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
 * $Id: HexFile_ihx16.cxx,v 1.2 2006/04/08 14:49:45 gaufille Exp $
 */
using namespace std;

#include <stdio.h>
#include <stdexcept>
#include "HexFile.h"


HexFile_ihx16::HexFile_ihx16(char *filename, hexfile_mode_t mode) : HexFile(filename, mode) {
	eaddr = 0;
}


HexFile_ihx16::HexFile_ihx16(FILE *fp, hexfile_mode_t mode) : HexFile(fp, mode) {
	eaddr = 0;
}


HexFile_ihx16::~HexFile_ihx16(void) {
	fprintf(this->fp, ":00000001FF\n");
}


void HexFile_ihx16::read(DataBuffer& buf) {
	if(this->mode == HEXFILE_WRITE)
		throw logic_error("Attempt to read a file which was opened for writing.");

	int count, type, cksum, offset;
	unsigned int addr, data;
	unsigned long extbase;
	char in[100];
	char *p;

	extbase = 0;
	while(fgets(in, sizeof(in), fp) != NULL) {
		for(p=in; *p; p++)
			if((*p == '\n') || (*p == '\r')) { *p = 0; break; }

		if(sscanf(in, ":%02X%04X%02X", &count, &addr, &type) != 3) break;
		p = in + 9;

		cksum = (count & 0xff) + ((addr >> 8) & 0xff) +
		  (addr & 0xff) + (type & 0xff);

		if(type == 0x01) {
			/* End-of-file record */
			return;
		} else if(type == 0x02) {
			/* Extended segment address record */
			if((count != 2) || (addr != 0))
				throw runtime_error("Invalid extended segment address record.");
			if(sscanf(p, "%04lX", &extbase) != 1) break;
			cksum += (extbase & 0xff) + ((extbase >> 8) & 0xff);
			p += 4;
			extbase <<= 4;
			goto check_cksum;
		} else if(type != 0x00) {
			throw runtime_error("Unknown record type encountered.");
		}

		for(offset=0; offset<count; offset++) {
			if(sscanf(p, "%04X", &data) == 0) goto corrupted;
			p += 4;
			buf[extbase+addr+offset] = data;
			cksum += ((data >> 8) & 0xff);
			cksum += (data & 0xff);
		}
check_cksum:;
		if(sscanf(p, "%02X", &data) == 0) break;
		if((unsigned int)((-cksum) & 0xff) != (data & 0xff))
			throw runtime_error("Checksum error in hex file");
	}
corrupted:;
	throw runtime_error("Corrupted hex file");
}


void HexFile_ihx16::write(DataBuffer& buf, long start, long len) {
	if(this->mode == HEXFILE_READ)
		throw logic_error("Attempt to write to a file which was opened for reading.");

	int i, cksum;
	unsigned long addr;

	addr = start;
	while(addr < (unsigned long)start+len) {
		if((addr > (eaddr + 0xffff)) || (addr < eaddr)) {
			/* Need to output an extended segment address record */
			eaddr = ((addr >> 4) & 0xffff);
			cksum = 0x02 + 0x04 + (eaddr & 0xff) + ((eaddr >> 8) & 0xff);
			fprintf(fp, ":02000004%04lX%02X\n", eaddr, ((-cksum) & 0xff));
			eaddr <<= 4;
		}
		if((addr - eaddr) > 0xffff)
			throw runtime_error("Data out of range for an ihx16 file. Use another file type.");

		i = ((start+len) - addr) % 8;
		if(i == 0) i = 8;

		fprintf(fp, ":%02X%04X00", i, (unsigned int)(addr - eaddr));
		cksum = (i & 0xff) + (((addr-eaddr) >> 8) & 0xff) +
		  ((addr-eaddr) & 0xff);
		while(i > 0) {
			fprintf(fp, "%04X", (int)buf[addr] & 0xffff);
			cksum += ((buf[addr] >> 8) & 0xff);
			cksum += (buf[addr] & 0xff);
			addr++;
			i--;
		}
		fprintf(fp, "%02X\n", ((-cksum) & 0xff));
	}
}
