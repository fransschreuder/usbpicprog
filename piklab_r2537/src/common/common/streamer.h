/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef STREAMER_H
#define STREAMER_H

#include <qdatastream.h>
#include <qtextstream.h>

#include "common/global/global.h"
#include "common/common/number.h"

template <class DataType>
class DataStreamer
{
public:
  uint toCppString(const QValueList<DataType *> &list, QTextStream &s) {
    QByteArray a;
#if QT_VERSION<0x040000
    QDataStream ds(a, IO_WriteOnly);
#else
    QDataStream ds(&a, QIODevice::WriteOnly);
#endif
    for (uint i=0; i<uint(list.count()); i++) ds << *list[i];
    s << "\"";
    for (uint i=0; i<uint(a.count()); i++) {
      if ( i!=0 && (i%40)==0 ) s << "\"" << endl << "\"";
      s << "\\x" << toChar(NumberBase::Hex, uchar(a[i])/16) << toChar(NumberBase::Hex, uchar(a[i])%16);
    }
    s << "\"";
    return a.count();
  }

  QValueList<DataType *> fromCppString(const char *data, uint size) {
    QByteArray a;
    a.setRawData(data, size);
#if QT_VERSION<0x040000
    QDataStream ds(a, IO_ReadOnly);
#else
    QDataStream ds(&a, QIODevice::ReadOnly);
#endif
    QValueList<DataType *> list;
    for (;;) {
      if ( ds.atEnd() ) break;
      DataType *data = new DataType;
      ds >> *data;
      list.append(data);
    }
    a.resetRawData(data, size);
    return list;
  }
};

#endif
