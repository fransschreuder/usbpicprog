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
 * $Id: DataBuffer.cxx,v 1.2 2006/04/08 14:49:45 gaufille Exp $
 */
using namespace std;

#include <stdio.h>
#include <string.h>
#include <stdexcept>
#include "DataBuffer.h"


DataBuffer::DataBuffer(int wordsize) {
	this->wordsize = wordsize;

	this->clearvalue = 0;
	for(int i=0; i<wordsize; i++) {
		this->clearvalue <<= 1;
		this->clearvalue |= 0x0001;
	}
	memset(chunktable, 0, sizeof(chunktable));
}


DataBuffer::~DataBuffer() {
	this->clear();
}


void DataBuffer::clear(void) {
	for(int i=0; i < num_chunks; i++) {
		if(chunktable[i] != NULL) {
			delete chunktable[i];
			chunktable[i] = NULL;
		}
	}
}


int DataBuffer::get_wordsize(void) {
	return wordsize;
}


long DataBuffer::compare(DataBuffer& other) {
	int offset, chunk_num;

	for(chunk_num=0; chunk_num < num_chunks; chunk_num++) {
		if(chunktable[chunk_num] == other.chunktable[chunk_num]) {
			/* Same DataBuffer or both pointers are NULL. They are == */
			continue;
		} else if(chunktable[chunk_num] == NULL) {
			/* Make sure other chunk only contains blank values */
			unsigned int *chunk = other.chunktable[chunk_num];

			for(offset=0; offset < other.chunk_size; offset++) {
				if(chunk[offset] != other.clearvalue)
					return (chunk_num * other.chunk_size) + offset;
			}
		} else if(other.chunktable[chunk_num] == NULL) {
			/* Make sure this chunk only contains blank values */
			unsigned int *chunk = chunktable[chunk_num];

			for(offset=0; offset < chunk_size; offset++) {
				if(chunk[offset] != clearvalue)
					return (chunk_num * chunk_size) + offset;
			}
		} else {
			/* Compare the contents of the 2 chunks */
			unsigned int *chunk1 = chunktable[chunk_num];
			unsigned int *chunk2 = other.chunktable[chunk_num];

			for(offset=0; offset < chunk_size; offset++) {
				if(chunk1[offset] != chunk2[offset])
					return (chunk_num * chunk_size) + offset;
			}
		}
	}

	return -1;
}


bool DataBuffer::isblank(size_t addr) {
	unsigned int *chunk;

	chunk = chunktable[(addr >> chunk_size_bits) % num_chunks];
	if(chunk == NULL) return true;
	if(chunk[addr % chunk_size] == clearvalue)
		return true;
	return false;
}


unsigned int& DataBuffer::operator[](size_t n) {
	unsigned int *chunk;

	if(n >= (num_chunks * chunk_size))
		throw out_of_range("DataBuffer index out of range");

	chunk = chunktable[(n >> chunk_size_bits) % num_chunks];
	if(chunk == NULL) {
		/* Need to allocate a new chunk */
		//printf("Allocating chunk 0x%04lx\n", (n >> chunk_size_bits) % num_chunks);
		chunk = new unsigned int[chunk_size];
		if(chunk == NULL) throw bad_alloc();
		chunktable[(n >> chunk_size_bits) % num_chunks] = chunk;

		/* Clear the chunk */
		for(int i=0; i<chunk_size; i++) chunk[i] = clearvalue;
	}
	return chunk[n % chunk_size];
}
