/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef RANGE_H
#define RANGE_H

#include "common/global/global.h"

//-----------------------------------------------------------------------------
template <typename Type>
class GenericRange
{
public:
  virtual ~GenericRange() {}
  virtual bool isEmpty() const = 0;
  bool contains(Type v) const { return !isEmpty() && v>=start && v<=end; }

  Type start, end;
};

class Range : public GenericRange<uint>
{
public:
  Range() { start = 0; end = 0; }
  Range(uint s, uint e) { start = s; end = e; }
  virtual bool isEmpty() const { return end<=start; }
};

template <typename Type>
inline QDataStream &operator >>(QDataStream &s, GenericRange<Type> &r) { s >> r.start >> r.end; return s; }
template <typename Type>
inline QDataStream &operator <<(QDataStream &s, const GenericRange<Type> &r) { s << r.start << r.end; return s; }
template <typename Type>
inline bool operator ==(const GenericRange<Type> &r1, const GenericRange<Type> &r2) { return ( r1.start==r2.start && r1.end==r2.end ); }

template <typename Type, typename RangeType>
class GenericRangeVector : public QValueVector<RangeType>
{
public:
  GenericRangeVector() {}
  GenericRangeVector(const RangeType &range) { append(range); }
  bool isEmpty() const {
    uint nb = this->count();
    for (uint i=0; i<nb; i++) if ( !this->at(i).isEmpty() ) return false;
    return true;
  }
  bool contains(Type v) const {
    uint nb = this->count();
    for (uint i=0; i<nb; i++) if ( this->at(i).contains(v) ) return true;
    return false;
  }
};

typedef GenericRangeVector<uint, Range> RangeVector;

#endif
