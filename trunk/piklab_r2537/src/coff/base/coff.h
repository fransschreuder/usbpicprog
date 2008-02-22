/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef COFF_H
#define COFF_H

#include "common/global/global.h"
#include "devices/pic/base/pic.h"
#include "common/global/purl.h"
#include "common/global/log.h"

BEGIN_DECLARE_ENUM(CoffType)
  Archive = 0, Object
END_DECLARE_ENUM_STD(CoffType)

//----------------------------------------------------------------------------
namespace Coff
{
extern bool getString(const QByteArray &data, uint &offset, uint nbChars, Log::Base &log, QString &name);
extern bool getULong(const QByteArray &data, uint &offset, uint nbBytes, Log::Base &log, Q_UINT32 &v);

//----------------------------------------------------------------------------
enum SizeType { HeaderSize = 0, OptHeaderSize, SectionHeaderSize, SymbolSize,
                LineNumberSize, RelocationSize, Nb_SizeTypes };
struct FormatData {
  const char *key, *label;
  uint magic;
  uint sizes[Nb_SizeTypes];
};
BEGIN_DECLARE_ENUM(Format)
  OldMicrochip = 0, PIC30, NewMicrochip
END_DECLARE_ENUM(Format, FormatData)

extern CoffType identify(const PURL::Url &url, Log::Base &log);
extern CoffType identify(const QByteArray &data, uint &offset, Log::Base &log, Format &format, Q_UINT32 &magic);

//----------------------------------------------------------------------------
class Base
{
public:
  Base(const PURL::Url &url) : _url(url) {}
  virtual ~Base() {}
  virtual bool parse(Log::Base &log) = 0;
  PURL::Url url() const { return _url; }
  uint magic() const { return _magic; }

  virtual Log::KeyList information() const = 0;

protected:
  PURL::Url  _url;
  Format     _format;
  Q_UINT32   _magic;

  bool initParse(CoffType type, QByteArray &data, uint &offset, Log::Base &log);
};

} // namespace

#endif
