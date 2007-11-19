/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "common/global/pfile.h"

#include <qfile.h>

//-----------------------------------------------------------------------------
bool PURL::File::openForWrite()
{
  close();
  _file->setName(url().filepath());
  if ( !_file->open(IO_WriteOnly) ) {
    _error = i18n("Could not open file for writing.");
    _log.sorry(_error, i18n("File: %1").arg(_file->name()));
    return false;
  }
  return true;
}

bool PURL::File::close()
{
  _file->close();
  return ( uint(_file->status())==IO_Ok );
}

bool PURL::File::openForRead()
{
  close();
  _file->setName(_url.filepath());
  if ( !_file->open(IO_ReadOnly) ) {
    _error = i18n("Could not open file for reading.");
    _log.sorry(_error, i18n("File: %1").arg(_file->name()));
    return false;
  }
  return true;
}

bool PURL::File::remove()
{
  return _file->remove();
}
