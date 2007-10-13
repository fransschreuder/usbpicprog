/****************************************************************************
**
** Copyright (C) 1992-2005 Trolltech AS. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/
#ifndef QFLAGS_H
#define QFLAGS_H

#include <qglobal.h>

class QFlag
{
    int i;
public:
    inline QFlag(int i);
    inline operator int() const { return i; }
};

inline QFlag::QFlag(int ai) : i(ai) {}

template<typename Enum>
class QFlags
{
    typedef void **Zero;
    int i;
public:
    typedef Enum enum_type;
    inline QFlags(const QFlags &f) : i(f.i) {}
    inline QFlags(Enum f) : i(f) {}
    inline QFlags(Zero = 0) : i(0) {}
    inline QFlags(QFlag f) : i(f) {}

    inline QFlags &operator=(const QFlags &f) { i = f.i; return *this; }
    inline QFlags &operator&=(int mask) {  i &= mask; return *this; }
    inline QFlags &operator&=(uint mask) {  i &= mask; return *this; }
    inline QFlags &operator|=(QFlags f) {  i |= f.i; return *this; }
    inline QFlags &operator|=(Enum f) {  i |= f; return *this; }
    inline QFlags &operator^=(QFlags f) {  i ^= f.i; return *this; }
    inline QFlags &operator^=(Enum f) {  i ^= f; return *this; }


    inline operator int() const { return i;}

    inline QFlags operator|(QFlags f) const { QFlags g; g.i = i | f.i; return g; }
    inline QFlags operator|(Enum f) const { QFlags g; g.i = i | f; return g; }
    inline QFlags operator^(QFlags f) const { QFlags g; g.i = i ^ f.i; return g; }
    inline QFlags operator^(Enum f) const { QFlags g; g.i = i ^ f; return g; }
    inline QFlags operator&(int mask) const { QFlags g; g.i = i & mask; return g; }
    inline QFlags operator&(uint mask) const { QFlags g; g.i = i & mask; return g; }
    inline QFlags operator&(Enum f) const { QFlags g; g.i = i & f; return g; }
    inline QFlags operator~() const { QFlags g; g.i = ~i; return g; }

    inline bool operator!() const { return !i; }
};

#define Q_DECLARE_FLAGS(Flags, Enum)\
typedef QFlags<Enum> Flags;
#define Q_DECLARE_OPERATORS_FOR_FLAGS(Flags) \
inline QFlags<Flags::enum_type> operator|(Flags::enum_type f1, Flags::enum_type f2) \
{ return QFlags<Flags::enum_type>(f1) | f2; } \
inline QFlags<Flags::enum_type> operator|(Flags::enum_type f1, QFlags<Flags::enum_type> f2) \
{ return f2 | f1; }

#endif
