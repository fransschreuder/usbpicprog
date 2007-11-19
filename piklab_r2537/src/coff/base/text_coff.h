/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TEXT_COFF_H
#define TEXT_COFF_H

#include "coff_object.h"

namespace Coff
{

class TextObject : public Object
{
public:
  TextObject(const Device::Data *device, const PURL::Url &url);
  virtual bool parse(Log::Base &log);
  int lineForAddress(const PURL::Url &url, Address address) const;
  QMap<PURL::Url, uint> sourceLinesForAddress(Address address) const; // url -> line
  QValueVector<Address> addresses(const PURL::Url &url, uint line) const;
  const QStringList &filenames() const;

  QString disassembly() const;
  virtual Log::KeyList information() const;

private:
  mutable bool _initialized;
  mutable QMap<Address, uint> _lines; // address -> line in disassembly listing
  mutable QStringList _list;
  mutable QStringList _filenames;

  uint nbLines(const QString &filename) const;
  QValueVector<const CodeLine *> findCodeLines(const QString &filename, uint line) const;
  PURL::Url urlForFilename(const QString &filename) const;
  void init() const;
  const Section *section(const CodeLine &cline) const;
};

} // namespace

#endif
