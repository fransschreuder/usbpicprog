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

struct EnumData {
  const char *key, *label;
};

#define BEGIN_DECLARE_ENUM(EnumClass) \
  class EnumClass \
  { \
  public: \
    enum Type {

#define END_DECLARE_ENUM(EnumClass, EnumData) \
    , Nb_Types }; \
    typedef EnumData Data; \
    EnumClass(int i = 0) : _value(Type(i)) { Q_ASSERT( i>=0 && i<=Nb_Types ); } \
    bool operator ==(Type value) { return _value==value; } \
    bool operator !=(Type value) { return _value!=value; } \
    EnumClass &operator ++() { _value = Type(int(_value) + 1); return *this; } \
    operator Type() const { return _value; } \
    static EnumClass fromKey(const QString &key) { \
      for (uint i=0; i<Nb_Types; i++) \
        if ( key==DATA[i].key ) return Type(i); \
      return Nb_Types; \
    } \
    const Data *data() const { \
      if ( _value==Nb_Types ) return 0; \
      return &DATA[_value]; \
    } \
    QString key() const { \
      if ( _value==Nb_Types ) return QString::null; \
      return DATA[_value].key; \
    } \
    const char *label() const { \
      Q_ASSERT( _value!=Nb_Types ); \
      return DATA[_value].label; \
    } \
  private: \
    static const Data DATA[Nb_Types]; \
    Type _value; \
  }; \
  \
  inline QDataStream &operator <<(QDataStream &s, const EnumClass &e) \
  { \
    s << Q_UINT8(e); \
    return s; \
  } \
  \
  inline QDataStream &operator >>(QDataStream &s, EnumClass &e) \
  { \
    Q_UINT8 tmp; \
    s >> tmp; \
    e = EnumClass::Type(tmp); \
    return s; \
  }

#define FOR_EACH(EnumClass, v) \
  for(EnumClass v; v<EnumClass::Nb_Types; ++v)

#endif
