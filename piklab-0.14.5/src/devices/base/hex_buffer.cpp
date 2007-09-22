/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *             (C) 2003 by Alain Gibaud                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "hex_buffer.h"

#include <qtextstream.h>

#include "common/global/global.h"
#include "devices/base/generic_device.h"

//-----------------------------------------------------------------------------
void HexBuffer::set(uint k, uint value)
{
  _data[k] = value;
}

uint HexBuffer::operator[](uint k) const
{
  const_iterator it = _data.find(k);
  if ( it!=_data.end() ) return it.data();
  return Device::INVALID;
}

//-----------------------------------------------------------------------------
const char * const HexBuffer::FORMATS[Nb_Formats] = {
  "inhx8m", /*"inhx8s", */"inhx16", "inhx32"
};

void HexBuffer::savePartial(QTextStream &stream, Format format) const
{
  uint oldseg = Device::INVALID;
  const_iterator block = _data.begin();
  int len;
  while ( fetchNextBlock(block, _data.end(), &len) ) {
    // block found, write it
    uint seg = block.key() >> 15 ; // 2 * seg address
    if ( format == IHX32 && seg != oldseg ) {
      char buf[50];
      unsigned int check = 0x02 + 0x04 + (seg >> 8) + (seg & 0xFF);
      sprintf(buf,":02000004%04X%02X\n", seg, -check & 0xFF );
      stream << buf;
      oldseg = seg;
    }
    writeHexBlock(stream, len, block, format);
  }
}

void HexBuffer::saveEnd(QTextStream &stream) const
{
  stream << ":00000001FF\n";
}

/* Write one line of Intel-hex file
 * Original code source from Timo Rossi,
 * modified by Alain Gibaud to support large blocks write
 */
void HexBuffer::writeHexBlock(QTextStream &stream, int reclen, // length (in words)
			  const_iterator& data, // pointer to 1st data word (incremented by function)
                          Format format)
{
    while ( reclen>HEXBLKSIZE ) {
      writeHexBlock(stream, HEXBLKSIZE, data, format);
      reclen -= HEXBLKSIZE;
    }
    if ( reclen<=0 ) return; /* Oops, block has just a HEXBLKSIZE * n size */

    char buf[20];
    int check = 0;

    // line start
    uint loc =  data.key();
    switch (format) {
      case IHX8M:
      case IHX32:
        loc *= 2;
        sprintf(buf,  ":%02X%04X00", 2*reclen, loc & 0xFFFF);
        check += ((loc) & 0xff) + (((loc) >> 8) & 0xff) + 2*reclen;
        break;
      case IHX16:
        sprintf(buf, ":%02X%04X00", reclen, loc & 0xFFFF);
        check += (loc & 0xff) + ((loc >> 8) & 0xff) + reclen;
        break;
      case Nb_Formats: Q_ASSERT(false); break;
    }
    stream << buf;

    // data
    for (; reclen > 0; ++data, --reclen) {
      uint word =  data.data();
      switch (format) {
        case IHX8M:
        case IHX32:
          sprintf( buf, "%02X%02X", (int)(word & 0xff), (int)((word >> 8) & 0xff));
          break;
        case IHX16:
          sprintf(buf, "%02X%02X", (int)((word >> 8) & 0xff), (int)(word & 0xff));
          break;
        case Nb_Formats: Q_ASSERT(false); break;
      }
      stream << buf;
      check += (word & 0xff) + ((word >> 8) & 0xff);
    }

    // checksum, assumes 2-complement
    sprintf(buf, "%02X\n", (-check) & 0xff);
    stream << buf;
}

/* -------------------------------------------------------------------------
    This routine detects the next block to output
    A block is a set of consecutive addresse words not
    containing 0xFFFFFFFF
    @return true if a block has been detected
    'it' is updated to point to the first address of the block
    '*len' contains the size of the block
*/
bool HexBuffer::fetchNextBlock(const_iterator& it, const const_iterator &end, int *len)
{
    uint startadr, curadr;
    // for( i = *start ; (i < MAXPICSIZE) && (Mem[i] == INVALID) ; ++i) ;
    // skip non-used words

    // Search block start
    while ( it!=end && it.data()==Device::INVALID ) ++it;

    // if(i >= MAXPICSIZE ) return false ;
    if ( it==end ) return false;

    //for( *start = i ; (i < MAXPICSIZE) && (Mem[i] != INVALID) ; ++i) ;
    //*len = i - *start ;
    // search block end - a block may not cross a segment boundary
    const_iterator itt(it) ;
    for (curadr = startadr = itt.key(), ++itt; itt!=end; ++itt) {
      if ( itt.key()!=curadr+1 ) break;    // non contiguous addresses
      if ( itt.data()==Device::INVALID ) break;   // unused word found
      if ( ((itt.key()) & 0xFFFF0000U)!=(curadr & 0xFFFF0000U) ) break; // cross segment boundary
      curadr = itt.key();
    }
    *len = curadr - startadr + 1 ;

    return *len != 0  ;
}

QString HexBuffer::ErrorData::message() const
{
  switch (type) {
    case UnrecognizedFormat: return i18n("Unrecognized format (line %1).").arg(line);
    case UnexpectedEOF: return i18n("Unexpected end-of-file.");
    case UnexpectedEOL: return i18n("Unexpected end-of-line (line %1).").arg(line);
    case WrongCRC: return i18n("CRC mismatch (line %1).").arg(line);
  }
  Q_ASSERT(false);
  return QString::null;
}

bool HexBuffer::load(QTextStream &stream, QStringList &errors)
{
  Format format;
  QValueList<ErrorData> list = load(stream, format);
  if ( list.isEmpty() ) return true;
  errors.clear();
  for (uint i=0; i<uint(list.count()); i++) errors += list[i].message();
  return false;
}

QValueList<HexBuffer::ErrorData> HexBuffer::load(QTextStream &stream, Format &format)
{
  clear();
  format = Nb_Formats;
  QValueList<HexBuffer::ErrorData> errors;
  load(stream, format, errors);
  if ( format==Nb_Formats ) format = IHX8M; // default
  return errors;
}

/* -------------------------------------------------------------------------
   Read a Intel HEX file of either INHX16 or INHX8M format type, detecting
   the format automagicly by the wordcount and length of the line
   Tested in 8 and 16 bits modes
   ------------------------------------------------------------------------ */
void HexBuffer::load(QTextStream &stream, Format &format, QValueList<ErrorData> &errors)
{
    uint addrH = 0; // upper 16 bits of 32 bits address (inhx32 format)
    uint line = 1;

    for (; !stream.atEnd(); line++) { // read each line
        QString s = stream.readLine();
        if ( !s.startsWith(":") ) continue; // skip invalid intel hex line
        s = s.stripWhiteSpace(); // clean-up white spaces at end-of-line
        if ( s==":" ) continue; // skip empty line

        const char *p = s.latin1();
        p += 1; // skip ':'
        uint bytecount = (s.length()-11) / 2; // number of data bytes of this record

        // get the byte count, the address and the type for this line.
        uint count, addr, type;
        if ( sscanf(p, "%02X%04X%02X", &count , &addr, &type)!=3 ) {
          errors += ErrorData(line, UnrecognizedFormat);
          return;
        }
        p += 8;
        uint cksum = count + (addr >> 8) + (addr & 0xFF) + type;

        if( type==0x01 ) { // EOF field :00 0000 01 FF
            uint data;
            if ( sscanf(p, "%02X", &data)!=1 ) errors += ErrorData(line, UnexpectedEOL);
            else if ( ((cksum+data) & 0xFF)!=0 ) errors += ErrorData(line, WrongCRC);
            return;
        }

        if ( type==0x04 ) { // linear extended record (for 0x21xxxx, :02 0000 04 0021 D9)
            if( sscanf(p, "%04X", &addrH)!=1 ) {
              errors += ErrorData(line, UnrecognizedFormat); // bad address record
              return;
            }
            p += 4;
            cksum += (addrH & 0xFF);
            cksum += (addrH >> 8);
            if ( format==Nb_Formats ) format = IHX32;
            else if ( format!=IHX32 ) {
              errors += ErrorData(line, UnrecognizedFormat); // inconsistent format
              return;
            }
            uint data;
            if ( sscanf(p, "%02X", &data)!=1 ) errors += ErrorData(line, UnexpectedEOL);
            else if ( ((cksum+data) & 0xFF)!=0 ) errors += ErrorData(line, WrongCRC);
            //qDebug("new address high: %s", toHex(addrH<<16, 8).data());
            continue; // goto next record
        }

        /* Figure out if its INHX16 or INHX8M
            if count is a 16 bits words count => INHX16
            if count is a byte count => INHX8M or INHX32 */
        if ( bytecount==count ) {
            if ( format==Nb_Formats ) format = IHX8M;
            else if ( format!=IHX8M && format!=IHX32 ) {
              errors += ErrorData(line, UnrecognizedFormat); // inconsistent format
              return;
            }
            /* Processing a INHX8M line */
            /* Modified to be able to read fuses from hexfile created by C18 toolchain */
            /* changed by Tobias Schoene 9 April 2005, */
            /* modified by A.G, because low and hi bytes was swapped in Tobias's code , 8 may 2005
            */
            uint addrbase = ((addrH << 16) | addr);
            //qDebug("line %i: address %s", line, toHex(addrbase, 8).data());
            for (uint x = 0; x<count; x++) {
              uint data;
              if ( sscanf(p, "%02X", &data)!=1 ) {
                errors += ErrorData(line, UnexpectedEOL);
                break;
              }
              p += 2;
              // A.G: I suspect possible initialization problem
              // if block begins at odd address
              // because |= works on an uninitalized word
              // however, I  don't know if such a situation can occurs
              uint a = addrbase+x >> 1;
              uint value = (*this)[a];
              if ( addrbase+x & 1 ) {
                value &= 0xFFU;            // clear out hi bits (patch from Chris Teague chris.teague@gmail.com)
                set(a, value | data << 8); // Odd addr => Hi byte
              } else {
                value &= ~0xFFU;      // clear out low bits (patch from Chris Teague chris.teague@gmail.com)
                set(a, value | data); // Low byte
              }
              //if ( x==0 ) qDebug("fb@%s: %s", toHex(addrbase+x >> 1, 8).data(), toHex(fb[addrbase+x >> 1], 8).data());
              cksum += data;
            }
        } else if ( bytecount==count*2 ) {
            if ( format==Nb_Formats ) format = IHX16;
            else if ( format!=IHX16 ) {
                errors += ErrorData(line, UnrecognizedFormat); // inconsistent format
                return;
            }
            /* Processing a INHX16 line */
            for(uint x=0; x<count; x++) {
                uint datal, datah;
                if( sscanf(p, "%02X%02X", &datah, &datal)!=2 ) {
                  errors += ErrorData(line, UnexpectedEOL);
                  break;
                }
                p += 4;
                //qDebug("%s: %s", toHexLabel(addr+x, 4).latin1(), toHexLabel(datal | (datah << 8), 4).latin1());
                set(addr+x, datal | (datah << 8));
                cksum += datah;
                cksum += datal;
            }
        } else {
            errors += ErrorData(line, UnrecognizedFormat); // Brrrr !! Strange format.
            return;
        }

        /* Process the checksum */
        uint data;
        if( sscanf(p, "%02X", &data)!=1 ) errors += ErrorData(line, UnexpectedEOL);
        else if( ((data + cksum) & 0xFF)!=0 ) errors += ErrorData(line, WrongCRC);
    }

    errors += ErrorData(line, UnexpectedEOF);
    return;
}
