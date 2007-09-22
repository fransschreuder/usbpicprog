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

#include "coff.h"

namespace Coff
{

class TextObject : public Object
{
public:
  TextObject(const Device::Data &device, const PURL::Url &url, Log::Base &log, bool *identified = 0);
  const Device::Data &device() const { return _device; }
  int lineForAddress(const PURL::Url &url, uint address) const;
  PURL::Url urlForAddress(uint address) const;
  QValueVector<uint> addresses(const PURL::Url &url, uint line) const;
  QString text() const;

private:
  const Device::Data       &_device;
  mutable bool             _initialized;
  mutable QMap<uint, uint> _lines; // address -> line
  mutable QStringList      _list;

  uint nbLines(const QString &filename) const;
  QValueVector<const CodeLine *> findCodeLines(const QString &filename, uint line) const;
  PURL::Url urlForFilename(const QString &filename) const;
  void init() const;
  const Section *section(const CodeLine &cline) const;
};

} // namespace

#endif
