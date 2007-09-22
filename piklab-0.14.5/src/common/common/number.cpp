/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "number.h"

#include "misc.h"
#if !defined(NO_KDE)
#  include <kglobal.h>
#endif

//-----------------------------------------------------------------------------
const Number::Data Number::DATA[Nb_Bases] = {
  { 10,  "",   I18N_NOOP("Decimal")     },
  { 16,  "0x", I18N_NOOP("Hexadecimal") },
  { 2,   "0b", I18N_NOOP("Binary")      },
  { 8,   "o",  I18N_NOOP("Octal")       },
  { 256, "",   I18N_NOOP("String")      }
};

char toChar(Number::Base base, uint value)
{
  Q_ASSERT( value<Number::DATA[base].base );
  if ( value>=Number::DATA[base].base ) qDebug("toChar %u (%u)", value, Number::DATA[base].base);
  if ( base==Number::String ) {
    if ( !isprint(value) ) return '.';
    return value;
  }
  if ( value<=9 ) return '0' + value;
  return 'A' + value - 10;
}

QString toString(Number::Base base, ulong value, uint nbChars)
{
  QString s;
  s.fill(0, nbChars);
  for (uint i=0; i<nbChars; i++) {
    s[nbChars-i-1] = toChar(base, uint(value % Number::DATA[base].base));
    value /= Number::DATA[base].base;
  }
  Q_ASSERT( value==0 );
  if ( value!=0 ) qCrash(); //qDebug("toString %s (%lu)", s.latin1(), value);
  return s;
}

QString toLabel(Number::Base base, ulong value, uint nbChars)
{
  if ( base==Number::String ) return "\'" + toString(base, value, nbChars) + "\'";
  return Number::DATA[base].prefix + toString(base, value, nbChars);
}

uint nbChars(Number::Base base, ulong value)
{
  uint nb = 1;
  for (;;) {
    value /= Number::DATA[base].base;
    if ( value==0 ) break;
    nb++;
  }
  return nb;
}

ulong maxValue(Number::Base base, uint nbChars)
{
  uint v = 1;
  for (uint i=0; i<nbChars; i++) v *= Number::DATA[base].base;
  return v - 1;
}

ulong fromString(Number::Base base, const QCString &s, bool *ok)
{
  return fromString(base, s.data(), s.length(), ok);
}
ulong fromString(Number::Base base, const QString &s, bool *ok)
{
#if QT_VERSION<0x040000
  return fromString(base, s.latin1(), s.length(), ok);
#else
  QByteArray a = s.toLatin1();
  return fromString(base, a.data(), a.count(), ok);
#endif
}

ulong fromLabel(Number::Base base, const QString &s, bool *ok)
{
#if QT_VERSION<0x040000
  return fromLabel(base, s.latin1(), s.length(), ok);
#else
  QByteArray a = s.toLatin1();
  return fromLabel(base, a.data(), a.count(), ok);
#endif
}

ulong fromLabel(Number::Base base, const QString &s, uint nbChars, bool *ok)
{
  if ( uint(s.length())!=nbChars+strlen(Number::DATA[base].prefix) ) {
    if (ok) *ok = false;
    return 0;
  }
  return fromLabel(base, s, ok);
}

ulong fromLabel(Number::Base base, const char *s, uint size, bool *ok)
{
  if (ok) *ok = false;
  if ( s==0 ) return 0;
  uint psize = (base==Number::String ? 1 : strlen(Number::DATA[base].prefix));
  uint ssize = (base==Number::String ? 1 : 0);
  if ( size<=(psize+ssize) ) return 0;
  if ( base==Number::String ) {
    if ( s[0]=='"' ) {
      if ( s[size-1]!='"' ) return 0;
    } else if ( s[0]=='\'' ) {
      if ( s[size-1]!='\'' ) return 0;
    } else return 0;
  } else for (uint i=0; i<psize; i++) if ( s[i]!=Number::DATA[base].prefix[i] ) return 0;
  return fromString(base, s+psize, size-psize-ssize, ok);
}

ulong fromAnyLabel(const QString &s, bool *ok)
{
  uint v = 0;
  bool bok = false;
  for (uint i=0; i<Number::Nb_Bases; i++) {
    v = fromLabel(Number::Base(i), s.lower(), &bok);
    if (bok) break;
  }
  if ( !bok ) v = fromString(Number::Dec, s, &bok);
  if ( !bok ) {
    if (ok) *ok = false;
    return 0;
  }
  if (ok) *ok = true;
  return v;
}

uint fromChar(Number::Base base, char c, bool *ok)
{
  uint v = 0;
  if ( base==Number::String ) {
    if (ok) *ok = true;
    return c;
  }
  if ( c>='0' && c<='9' ) v = c - '0';
  else if ( c>='A' && c<'Z' ) v = 10 + c - 'A';
  else if ( c>='a' && c<'z' ) v = 10 + c - 'a';
  else {
    if (ok) *ok = false;
    return 0;
  }
  if (ok) *ok = ( v<Number::DATA[base].base );
  return v;
}

ulong fromString(Number::Base base, const char *s, uint size, bool *ok)
{
  if (ok) *ok = false;
  if ( s==0 || size==0 ) return 0;
  ulong v = 0;
  for (uint i=0; i<size; i++) {
    v *= Number::DATA[base].base;
    bool bok;
    v += fromChar(base, s[i], &bok);
    if ( !bok ) return 0;
  }
  if (ok) *ok = true;
  return v;
}

QString toLabels(Number::Base base, const QMemArray<uint> &values, uint nbChars)
{
  QString s = "[";
  for (uint i=0; i<values.count(); i++) {
    if ( i!=0 ) s += ' ';
    s += toLabel(base, values[i], nbChars);
  }
  s += "]";
  return s;
}

QString formatNumber(ulong v)
{
#if defined(NO_KDE)
  return QString::number(v);
#else
  return KGlobal::locale()->formatNumber(v, 0);
#endif
}

QByteArray toAscii(const QString &s)
{
  QByteArray a(s.length());
  for (uint i=0; i<uint(s.length()); i++) a[i] = s[i].latin1();
  return a;
}
