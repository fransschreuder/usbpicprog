/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "coff.h"

#include <time.h>

#include "common/common/misc.h"
#include "devices/list/device_list.h"
#include "devices/base/device_group.h"
#include "devices/pic/base/pic_register.h"
#include "coff_data.h"
#include "common/global/pfile.h"

//----------------------------------------------------------------------------
const CoffType::Data CoffType::DATA[Nb_Types] = {
  { "archive", I18N_NOOP("Archive") },
  { "object",  I18N_NOOP("Object")  }
};

const Coff::Format::Data Coff::Format::DATA[Nb_Types] = {
  { 0, I18N_NOOP("Old Microchip"), 0x1234, { 20, 16, 40, 18, 16, 12 } },
  { 0, I18N_NOOP("PIC30"),         0x1236, { 20, 28, 40, 18,  8, 10 } }, // from PIC30 binutils "coff.h" file
  { 0, I18N_NOOP("New Microchip"), 0x1240, { 20, 18, 40, 20, 16, 12 } }  // (C18 >= 3.0)
};

CoffType Coff::identify(const PURL::Url &url, Log::Base &log)
{
  PURL::File file(url, log);
  if ( !file.openForRead() ) return CoffType::Nb_Types;
  QByteArray data = file.readAll();
  if ( log.hasError() ) return CoffType::Nb_Types;
  uint offset = 0;
  Format format;
  Q_UINT32 magic;
  return identify(data, offset, log, format, magic);
}

CoffType Coff::identify(const QByteArray &data, uint &offset, Log::Base &log, Format &format, Q_UINT32 &magic)
{
  QString id = "!<arch>\012";
  if ( data.count()>=id.length() ) {
    QString s = QString::fromAscii(data.data(), id.length());
    if ( s==id ) {
      offset += id.length();
      return CoffType::Archive;
    }
  }
  if ( !getULong(data, offset, 2, log, magic) ) return CoffType::Nb_Types;
  log.log(Log::DebugLevel::Extra, QString("COFF format: %1").arg(toHexLabel(magic, 4)));
  format = Format::Nb_Types;
  FOR_EACH(Format, f) if ( magic==f.data().magic ) format = f;
  return CoffType::Object;
}

//----------------------------------------------------------------------------
bool Coff::getULong(const QByteArray &data, uint &offset, uint nbBytes, Log::Base &log, Q_UINT32 &v)
{
  bool ok;
  v = ::getULong(data, offset, nbBytes, &ok);
  if ( !ok ) {
    log.log(Log::LineType::Error, i18n("COFF file is truncated (offset: %1  nbBytes: %2  size:%3).").arg(offset).arg(nbBytes).arg(data.count()));
    return false;
  }
  offset += nbBytes;
  return true;
}

bool Coff::getString(const QByteArray &data, uint &offset, uint nbChars, Log::Base &log, QString &name)
{
  if ( !checkAvailable(data, offset, nbChars) ) {
    log.log(Log::LineType::Error, i18n("COFF file is truncated (offset: %1  nbBytes: %2  size:%3).").arg(offset).arg(nbChars).arg(data.count()));
    return false;
  }
  name = QString::fromAscii(data.data()+offset, nbChars);
  offset += nbChars;
  return true;
}

//----------------------------------------------------------------------------
bool Coff::Base::initParse(CoffType type, QByteArray &data, uint &offset, Log::Base &log)
{
  PURL::File file(_url, log);
  if ( !file.openForRead() ) return false;
  data = file.readAll();
  if ( log.hasError() ) return false;
  if ( identify(data, offset, log, _format, _magic)!=type ) {
    log.log(Log::LineType::Error, i18n("Could not recognize COFF file (magic number is %1).").arg(toHexLabel(_magic, 4)));
    return false;
  }
  return true;
}
