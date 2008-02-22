/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef KEY_ENUM_H
#define KEY_ENUM_H

#include <qdatastream.h>

#include "misc.h"

#define BEGIN_DECLARE_ENUM(Enum) \
class Enum \
{ \
public: \
  enum Type {

#define DECLARE_DATA \
  public: \
  static Type fromKey(const QString &key) { \
    for (uint i=0; i<Nb_Types; i++) \
      if ( key==DATA[i].key ) return Type(i); \
    return Type(Nb_Types); \
  } \
  const Data &data() const { \
    CRASH_ASSERT( _value!=Nb_Types ); \
    return DATA[_value]; \
  } \
  const char *key() const { \
    if ( _value==Nb_Types ) return 0; \
    Q_ASSERT(DATA[_value].key); \
    return DATA[_value].key; \
  } \
  const char *label() const { \
    CRASH_ASSERT( _value!=Nb_Types ); \
    Q_ASSERT(DATA[_value].label); \
    return DATA[_value].label; \
  } \
  private: \
    static const Data DATA[Nb_Types]; \

#define DECLARE_ENUM_CLASS(Enum) \
  public: \
  Enum(Type value = Type(0)) : _value(value) { Q_ASSERT( value>=0 && value<=Type(Nb_Types)); } \
  bool operator ==(Enum e) const { return _value==e._value; } \
  bool operator !=(Enum e) const { return _value!=e._value; } \
  bool operator <(Enum e) const { return _value<e._value; } \
  bool operator <=(Enum e) const { return _value<=e._value; } \
  bool operator >(Enum e) const { return _value>e._value; } \
  bool operator >=(Enum e) const { return _value>=e._value; } \
  Enum &operator ++() { _value = Type(int(_value) + 1); return *this; } \
  Type type() const { return Type(_value); } \
  private: \
    uint _value; \
    friend QDataStream &operator >>(QDataStream &s, Enum &e); \
    friend QDataStream &operator <<(QDataStream &s, const Enum &e); \
}; \
inline QDataStream &operator <<(QDataStream &s, const Enum &e) \
{ \
  s << e._value; \
  return s; \
} \
inline QDataStream &operator >>(QDataStream &s, Enum &e) \
{ \
  s >> e._value; \
  return s; \
} \

#define END_DECLARE_ENUM(Enum, EnumData) \
    , Nb_Types \
  }; \
  typedef EnumData Data; \
  DECLARE_DATA \
  DECLARE_ENUM_CLASS(Enum) \

#define END_DECLARE_ENUM_STD(Enum) \
    , Nb_Types \
  }; \
  struct Data { \
    const char *key, *label; \
  }; \
  DECLARE_DATA \
  DECLARE_ENUM_CLASS(Enum)

#define END_DECLARE_ENUM_NO_DATA(Enum) \
    , Nb_Types \
  }; \
  DECLARE_ENUM_CLASS(Enum)

#define FOR_EACH(Enum, e) for(Enum e; e<Enum::Type(Enum::Nb_Types); ++e)

#endif
