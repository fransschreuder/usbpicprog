/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *             (C) 2003 by Alain Gibaud                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef HEX_BUFFER_H
#define HEX_BUFFER_H

#include "common/global/global.h"
class QTextStream;

class HexBuffer
{
public:
  HexBuffer() {}
  void clear() { _data.clear(); }
  void set(uint offset, uint value);
  uint operator[](uint offset) const;
  typedef QMap<uint, uint>::const_iterator const_iterator;
  const_iterator begin() const { return _data.begin(); }
  const_iterator end() const { return _data.end(); }

  enum Format { /// Differents flavors of Intel HEX file formats
    IHX8M = 0,  ///< 8 bits "swapped" format
    //    IHX8S,
    IHX16,      ///< 16 bits format
    IHX32,      ///< 8 bit format with 32 bits addresses
    Nb_Formats
  };
  static const char * const FORMATS[Nb_Formats];

  void savePartial(QTextStream &s, Format format) const;
  void saveEnd(QTextStream &s) const;
  enum ErrorType { UnrecognizedFormat, WrongCRC, UnexpectedEOF, UnexpectedEOL };
  class ErrorData {
  public:
    ErrorData() {}
    ErrorData(uint _line, ErrorType _type) : line(_line), type(_type) {}
    QString message() const;
    uint line;
    ErrorType type;
  };
  QValueList<ErrorData> load(QTextStream &stream, Format &format);
  bool load(QTextStream &stream, QStringList &errors);

private:
  enum { HEXBLKSIZE = 8 }; // line size in HEX files
  QMap<uint, uint> _data;

  static bool fetchNextBlock(const_iterator& start, const const_iterator &end, int *len);
  static void writeHexBlock(QTextStream &stream, int reclen, const_iterator& data, Format format);
  void load(QTextStream &stream, Format &format, QValueList<ErrorData> &errors);
};

#endif
