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
 * $Id: HexFile_ihx8.cxx,v 1.2 2006/04/08 14:49:45 gaufille Exp $
 */
using namespace std;

#include <stdio.h>
#include <stdexcept>
#include "HexFile.h"


HexFile_ihx8::HexFile_ihx8(char *filename, hexfile_mode_t mode) : HexFile(filename, mode) {
	uaddr = 0;
}


HexFile_ihx8::HexFile_ihx8(FILE *fp, hexfile_mode_t mode) : HexFile(fp, mode) {
	uaddr = 0;
}


HexFile_ihx8::~HexFile_ihx8(void) {
	fprintf(this->fp, ":00000001FF\n");
}


void HexFile_ihx8::read(DataBuffer& buf) {
	if(this->mode == HEXFILE_WRITE)
		throw logic_error("Attempt to read a file which was opened for writing.");

	int count, type, cksum, offset, bufwordlen;
	unsigned long extbase;
	unsigned int addr, data;
	char in[100];
	char *p;

	extbase = 0;
	bufwordlen = (buf.get_wordsize() + 7) & ~7;
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
		} else if(type == 0x04) {
			/* Extended linear address record */
			if((count != 2) || (addr != 0))
				throw runtime_error("Invalid extended linear address record.");
			if(sscanf(p, "%04lX", &extbase) != 1) break;
			cksum += (extbase & 0xff) + ((extbase >> 8) & 0xff);
			p += 4;
			extbase <<= 16;
			goto check_cksum;
		} else if(type != 0x00) {
			throw runtime_error("Unknown record type encountered.");
		}

		for(offset=0; offset<count; offset++) {
			if(sscanf(p, "%02X", &data) == 0) goto corrupted;
			cksum += ((data >> 8) & 0xff);
			cksum += (data & 0xff);
			p += 2;
			switch(bufwordlen) {
			case 8:
				buf[extbase+addr+offset] = data;
				break;
			case 16:
				if(((extbase+addr+offset) % 2) == 0) {
					/* Low byte of the word */
					buf[(extbase+addr+offset)/2] &= ~0x00ff;
					buf[(extbase+addr+offset)/2] |= data;
				} else {
					/* High byte of the word */
					buf[(extbase+addr+offset)/2] &= ~0xff00;
					buf[(extbase+addr+offset)/2] |= (data << 8);
				}
				break;
			default:
				throw runtime_error("Unsupported data buffer word size.");
			}
		}
check_cksum:;
		if(sscanf(p, "%02X", &data) == 0) break;
		if((unsigned int)((-cksum) & 0xff) != (data & 0xff))
			throw runtime_error("Checksum error in hex file");
	}
corrupted:;
	throw runtime_error("Corrupted hex file");
}


void HexFile_ihx8::write(DataBuffer& buf, long start, long len) {
	if(this->mode == HEXFILE_READ)
		throw logic_error("Attempt to write to a file which was opened for reading.");

	int numwords, cksum;
	unsigned int data;
	int bufwordsize = ((buf.get_wordsize() + 7) & ~7) / 8;
	unsigned long addr;

	addr = start;
	while(addr < (unsigned long)start+len) {
		if((((addr*bufwordsize) >> 16) & 0xffff) != uaddr) {
			/* Need to output an extended linear address record */
			uaddr = (((addr*bufwordsize) >> 16) & 0xffff);
			cksum = 0x02 + 0x04 + (uaddr & 0xff) + ((uaddr >> 8) & 0xff);
			fprintf(fp, ":02000004%04X%02X\n", uaddr, ((-cksum) & 0xff));
		}

		/* Calculate # of DataBuffer words to print on this line */
		numwords = ((start+len) - addr) % (16 / bufwordsize);
		if(numwords == 0) numwords = 16 / bufwordsize;

		fprintf(fp, ":%02X%04X00", numwords*bufwordsize,
			(unsigned int)(addr*bufwordsize) & 0xffff);
		cksum = ((numwords*bufwordsize) & 0xff) +
		  (((addr*bufwordsize) >> 8) & 0xff) + ((addr*bufwordsize) & 0xff);
		while(numwords > 0) {
			switch(bufwordsize) {
			case 1:
				data = buf[addr] & 0xff;
				cksum += data;
				fprintf(fp, "%02X", data);
				break;
			case 2:
				/* Print low byte, then high byte */
				data = buf[addr] & 0xff;
				cksum += data;
				fprintf(fp, "%02X", data);
				data = (buf[addr] >> 8) & 0xff;
				cksum += data;
				fprintf(fp, "%02X", data);
				break;
			default:
				throw runtime_error("Unsupported data buffer word size.");
			}
			numwords--;
			addr++;
		}
		fprintf(fp, "%02X\n", ((-cksum) & 0xff));
	}
}
