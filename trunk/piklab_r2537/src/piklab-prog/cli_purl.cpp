/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "common/global/purl.h"

#include <qfileinfo.h>
#include <qdatetime.h>

bool PURL::Base::probablyExists() const
{
  QFileInfo fi(_url.path());
  return fi.exists();
}

bool PURL::Url::isDestinationUpToDate(FileType type) const
{
  if ( !(data().properties & Source) ) return true;
  Url dest = toFileType(type);
  QFileInfo sfi(_url.path());
  if ( !sfi.exists() ) return true; // source does not exists
  QFileInfo dfi(dest._url.path());
  if ( !dfi.exists() ) return false; // destination does not exists
  return sfi.lastModified()<dfi.lastModified();
}
