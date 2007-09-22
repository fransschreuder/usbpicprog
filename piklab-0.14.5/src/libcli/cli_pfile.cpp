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
PURL::FileBase::~FileBase()
{
  delete _stream;
  delete _file;
}

const QFile *PURL::FileBase::qfile() const
{
  return _file;
}

QFile *PURL::FileBase::qfile()
{
  return _file;
}

bool PURL::FileBase::openForWrite()
{
  close();
  if ( !_file->open(IO_WriteOnly) ) {
    _error = i18n("Could not open file for writing.");
    if (_view) _view->sorry(_error, i18n("File: %1").arg(_file->name()));
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
PURL::File::File(const Url &url, Log::View *view)
  : FileBase(view, QString::null), _url(url)
{
  _file = new QFile(url.filepath(false));
}

bool PURL::File::close()
{
  _file->close();
  return ( uint(_file->status())==IO_Ok );
}

bool PURL::File::openForRead()
{
  close();
  if ( !_file->open(IO_ReadOnly) ) {
    _error = i18n("Could not open file for reading.");
    if (_view) _view->sorry(_error, i18n("File: %1").arg(_file->name()));
    return false;
  }
  return true;
}
