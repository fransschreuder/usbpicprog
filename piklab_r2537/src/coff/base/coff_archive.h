/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef COFF_ARCHIVE_H
#define COFF_ARCHIVE_H

#include "coff.h"

namespace Coff
{
//----------------------------------------------------------------------------
class Member
{
public:
  Member(const QByteArray &data, uint &offset, Log::Base &log);
  QString name() const { return _name; }
  uint nbBytes() const { return _nbBytes; }

private:
  QString _name;
  uint    _nbBytes;
};

//----------------------------------------------------------------------------
class Archive : public Base
{
public:
  Archive(const PURL::Url &url);
  virtual ~Archive();
  virtual bool parse(Log::Base &log);
  const QMap<QString, Member *>members() const { return _members; }
  const QMap<QString, Member *>symbols() const { return _symbols; }

  virtual Log::KeyList information() const;

private:
  QMap<QString, Member *> _members; // name -> Member *
  QMap<uint, Member *>    _offsets; // offset -> Member *
  QMap<QString, Member *> _symbols; // name -> Member *

  bool readSymbols(const QByteArray &data, uint offset, Log::Base &log);
};

} // namespace

#endif
