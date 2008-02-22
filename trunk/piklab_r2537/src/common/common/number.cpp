/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "number.h"

#include "common/global/global.h"
#include "misc.h"
#if !defined(NO_KDE)
#  include <kglobal.h>
#endif

//-----------------------------------------------------------------------------
const NumberBase::Data NumberBase::DATA[Nb_Types] = {
  { 10,  "",   I18N_NOOP("Decimal"),     "dec" },
  { 16,  "0x", I18N_NOOP("Hexadecimal"), "hex" },
  { 2,   "0b", I18N_NOOP("Binary"),      "bin" },
  { 8,   "o",  I18N_NOOP("Octal"),       "oct" },
  { 256, "",   I18N_NOOP("String"),      "str" }
};

char toChar(NumberBase base, uint value)
{
  Q_ASSERT( value<base.data().base );
  if ( value>=base.data().base ) qDebug("toChar %u (%u)", value, base.data().base);
  if ( base==NumberBase::String ) {
    if ( !isprint(value) ) return '.';
    return value;
  }
  if ( value<=9 ) return '0' + value;
  return 'A' + value - 10;
}

QString toString(NumberBase base, ulong value, uint nbChars)
{
  ulong tmp = value;
  QString s;
  s.fill(0, nbChars);
  for (uint i=0; i<nbChars; i++) {
    s[nbChars-i-1] = toChar(base, uint(value % base.data().base));
    value /= base.data().base;
  }
  Q_ASSERT( value==0 );
  if ( value!=0 ) qDebug("toString %s nbChars=%u", toLabel(base, tmp, ::nbChars(base, tmp)).latin1(), nbChars);
  return s;
}

QString toLabel(NumberBase base, ulong value, uint nbChars)
{
  if ( base==NumberBase::String ) return "\'" + toString(base, value, nbChars) + "\'";
  return base.data().prefix + toString(base, value, nbChars);
}

uint nbChars(NumberBase base, ulong value)
{
  uint nb = 1;
  for (;;) {
    value /= base.data().base;
    if ( value==0 ) break;
    nb++;
  }
  return nb;
}

ulong maxValue(NumberBase base, uint nbChars)
{
  uint v = 1;
  for (uint i=0; i<nbChars; i++) v *= base.data().base;
  return v - 1;
}

ulong fromString(NumberBase base, const QCString &s, bool *ok)
{
  return fromString(base, s.data(), s.length(), ok);
}
ulong fromString(NumberBase base, const QString &s, bool *ok)
{
#if QT_VERSION<0x040000
  return fromString(base, s.latin1(), s.length(), ok);
#else
  QByteArray a = s.toLatin1();
  return fromString(base, a.data(), a.count(), ok);
#endif
}

ulong fromLabel(NumberBase base, const QString &s, bool *ok)
{
#if QT_VERSION<0x040000
  return fromLabel(base, s.latin1(), s.length(), ok);
#else
  QByteArray a = s.toLatin1();
  return fromLabel(base, a.data(), a.count(), ok);
#endif
}

ulong fromLabel(NumberBase base, const QString &s, uint nbChars, bool *ok)
{
  if ( uint(s.length())!=nbChars+strlen(base.data().prefix) ) {
    if (ok) *ok = false;
    return 0;
  }
  return fromLabel(base, s, ok);
}

ulong fromLabel(NumberBase base, const char *s, uint size, bool *ok)
{
  if (ok) *ok = false;
  if ( s==0 ) return 0;
  uint psize = (base==NumberBase::String ? 1 : strlen(base.data().prefix));
  uint ssize = (base==NumberBase::String ? 1 : 0);
  if ( size<=(psize+ssize) ) return 0;
  if ( base==NumberBase::String ) {
    if ( s[0]=='"' ) {
      if ( s[size-1]!='"' ) return 0;
    } else if ( s[0]=='\'' ) {
      if ( s[size-1]!='\'' ) return 0;
    } else return 0;
  } else for (uint i=0; i<psize; i++) if ( s[i]!=base.data().prefix[i] ) return 0;
  return fromString(base, s+psize, size-psize-ssize, ok);
}

ulong fromAnyLabel(const QString &s, bool *ok)
{
  uint v = 0;
  bool bok = false;
  FOR_EACH(NumberBase, base) {
      v = fromLabel(base, s.lower(), &bok);
    if (bok) break;
  }
  if ( !bok ) v = fromString(NumberBase::Dec, s, &bok);
  if ( !bok ) {
    if (ok) *ok = false;
    return 0;
  }
  if (ok) *ok = true;
  return v;
}

uint fromChar(NumberBase base, char c, bool *ok)
{
  uint v = 0;
  if ( base==NumberBase::String ) {
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
  if (ok) *ok = ( v<base.data().base );
  return v;
}

ulong fromString(NumberBase base, const char *s, uint size, bool *ok)
{
  if (ok) *ok = false;
  if ( s==0 || size==0 ) return 0;
  ulong v = 0;
  for (uint i=0; i<size; i++) {
    v *= base.data().base;
    bool bok;
    v += fromChar(base, s[i], &bok);
    if ( !bok ) return 0;
  }
  if (ok) *ok = true;
  return v;
}

QString toLabels(NumberBase base, const QMemArray<uint> &values, uint nbChars)
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
