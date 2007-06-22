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
 * $Id: DataBuffer.h,v 1.2 2006/04/08 14:49:45 gaufille Exp $
 */
#ifndef __DataBuffer_h
#define __DataBuffer_h

#include <sys/types.h>
#include <map>

/** \file */


/** This class implements a sparse buffer by allocating chunks of the
 * buffer only as they are needed. The maximum size that the buffer can
 * grow is \c num_chunks * \c chunk_size words. On an access to the
 * DataBuffer, new chunks will be allocated automatically when needed.
 */
class DataBuffer {
public:
	/** The maximum number of chunks to which the buffer can grow. */
	static const long num_chunks = 4096;

	/** The number of words in a chunk. Must be a power of 2. */
	static const int chunk_size = 4096;

	/** The number of bits to represent the offset in a chunk.
	 * (log2 chunk_size) */
	static const int chunk_size_bits = 12;

	/** Constructs a DataBuffer. When the DataBuffer is created, all the
	 * locations will be cleared (all binary 1's) according to the wordsize.
	 * \param wordsize The size of a word (in bits) that each location of
	 *        the DataBuffer will contain.
	 */
	DataBuffer(int wordsize=16);

	/** Frees all memory and resources associated with this instance. */
	~DataBuffer();

	/** Clears the entire contents of the buffer. */
	void clear(void);

	/** Gets the word size that was specified to the constructor.
	 * \returns The word size in bits. */
	int get_wordsize(void);

	/** Compare the contents of this DataBuffer with another.
	 * \param other The DataBuffer to compare with this one.
	 * \retval -1 If the DataBuffers have the same contents.
	 * \retval >=0 The first address where the DataBuffers differ.
	 */
	long compare(DataBuffer& other);

	/** Checks to see if a memory location contains the blank value.
	 * \param addr The offset into the DataBuffer to check.
	 * \returns A boolean value indicating if the location contains
	 *          a value of all binary 1's.
	 */
	bool isblank(size_t addr);

	/** The index operator used for retrieving the address of elements.
	 * If the access is to a chunk which doesn't exist, the chunk will be
	 * allocated and the contents of the chunk will be set to the clear
	 * value.
	 * \throws bad_alloc If a new chunk couldn't be allocated.
	 * \throws out_of_range If the offset \c n is greater than the
	 *         maximum size of the DataBuffer.
	 */
	unsigned int& operator[](size_t n);

private:
	int wordsize;
	unsigned int clearvalue;
	unsigned int *chunktable[num_chunks];
};


#endif
