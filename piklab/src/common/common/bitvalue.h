/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BITVALUE_H
#define BITVALUE_H

#include "number.h"
#include "misc.h"

//----------------------------------------------------------------------------
class GenericValue
{
public:
  GenericValue(uint value) : _value(value) {}

  bool operator <(GenericValue v) const  { CRASH_ASSERT(_value!=INVALID); return _value<v._value;  }
  bool operator >(GenericValue v) const  { CRASH_ASSERT(_value!=INVALID); return _value>v._value;  }
  bool operator <=(GenericValue v) const { CRASH_ASSERT(_value!=INVALID); return _value<=v._value; }
  bool operator >=(GenericValue v) const { CRASH_ASSERT(_value!=INVALID); return _value>=v._value; }
  bool operator ==(GenericValue v) const { return _value==v._value; }
  bool operator !=(GenericValue v) const { return _value!=v._value; }

  bool bit(uint index) const { return (_value >> index) & 0x1; }
  uchar nybble(uint index) const { return (_value >> (4*index)) & 0xF; }
  uchar byte(uint index) const { return (_value >> (8*index)) & 0xFF; }
  uint toUInt() const { return _value; }

protected:
  static const uint INVALID;
  uint _value;

private:
  friend QDataStream &operator <<(QDataStream &s, GenericValue v);
  friend QDataStream &operator >>(QDataStream &s, GenericValue &v);
};

inline QDataStream &operator <<(QDataStream &s, GenericValue v) { s << v._value; return s; }
inline QDataStream &operator >>(QDataStream &s, GenericValue &v) { s >> v._value; return s; }

inline QString toLabel(GenericValue v) { return QString::number(v.toUInt()); }
inline QString toLabel(Number::Base base, GenericValue v, uint nbChars) { return toLabel(base, v.toUInt(), nbChars); }
inline QString toHexLabel(GenericValue v, uint nbChars) { return toHexLabel(v.toUInt(), nbChars); }
inline QString toHex(GenericValue v, uint nbChars) { return toHex(v.toUInt(), nbChars); }
inline QString toHexLabelAbs(GenericValue v) { return ::toHexLabelAbs(v.toUInt()); }

//----------------------------------------------------------------------------
class Address : public GenericValue
{
public:
  Address(uint value = INVALID) : GenericValue(value) {}
  bool isValid() const { return ( _value!=INVALID ); }

  Address operator +(int dec) const { CRASH_ASSERT(_value!=INVALID); return _value + dec; }
  Address &operator +=(int dec)     { CRASH_ASSERT(_value!=INVALID);_value += dec; return *this; }
  Address operator -(int dec) const { CRASH_ASSERT(_value!=INVALID);return _value - dec; }
  Address &operator -=(int dec)     { CRASH_ASSERT(_value!=INVALID);_value -= dec; return *this; }
  int operator -(Address a) const   { CRASH_ASSERT(_value!=INVALID && a._value!=INVALID); return _value - a._value; }
};

class AddressRange : public GenericRange<Address>
{
public:
  AddressRange() {}
  AddressRange(Address s, Address e) { start = s; end = e; }
  virtual bool isEmpty() const { return !start.isValid() || !end.isValid() || end<=start; }
};

typedef GenericRangeVector<Address, AddressRange> AddressRangeVector;

//----------------------------------------------------------------------------
class BitValue : public GenericValue
{
public:
  BitValue(uint value = INVALID) : GenericValue(value) {}
  bool isInitialized() const { return ( _value!=INVALID ); }

  BitValue operator |(BitValue v) const { return _value | v._value; }
  BitValue operator <<(uint shift) const { return _value << shift; }
  BitValue operator >>(uint shift) const { return _value >> shift; }
  BitValue operator +(BitValue v) const { return _value + v._value; }

  BitValue &operator |=(BitValue v) { _value |= v._value; return *this; }
  BitValue &operator <<=(uint shift) { _value <<= shift; return *this; }
  BitValue &operator >>=(uint shift) { _value >>= shift; return *this; }
  BitValue &operator +=(BitValue v) { _value += v._value; return *this; }

  BitValue XORn(uint n) const; // XOR between groups of n bits inside value
  BitValue XNORn(uint n) const; // XORn then NOT on n bits

  BitValue maskWith(BitValue mask) const { return _value & mask._value; }
  bool isInside(BitValue v) const { return ( (_value & v._value)==_value ); }
  BitValue complementInMask(BitValue mask) const { return mask._value & ~_value; }
  BitValue twoComplement() const { return -_value; }
  BitValue clearMaskBits(BitValue mask) const { return _value & ~mask._value; }
  bool isOverlapping(BitValue v) const { return ( _value & v._value ); }

  class const_iterator {
  public:
    const_iterator() {}
    bool operator !=(const_iterator it) const { return ( _current!=it._current ); }
    BitValue operator *() const { return BitValue(_current); }
    const_iterator &operator ++() {
      do {
        if ( _current==_value+1 ) _current = INVALID;
        if ( _current==INVALID ) break;
        _current++;
      } while ( (_current & _value)!=_current );
      return *this;
    }
  private:
    const_iterator(uint value, uint current) : _value(value), _current(current) {}
    uint _value, _current;
    friend class BitValue;
  };
  const_iterator begin() const { return const_iterator(_value, 0); }
  const_iterator end() const { return const_iterator(_value, INVALID); }

};

#endif
