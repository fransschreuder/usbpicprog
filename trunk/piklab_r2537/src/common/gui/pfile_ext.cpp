/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pfile_ext.h"

#include <qfile.h>
#include <kio/netaccess.h>
#include <ktempfile.h>
#include "common/gui/misc_gui.h"

//-----------------------------------------------------------------------------
bool PURL::File::openForWrite()
{
  close();
  if (_tmp) delete _tmp;
  _tmp = new KTempFile(QString::null, _extension);
  _tmp->setAutoDelete(true);
  if ( _tmp->status()!=0 ) {
    _error = i18n("Could not create temporary file.");
    _log.sorry(_error, i18n("File: %1").arg(_tmp->name()));
    return false;
  }
  return true;
}

bool PURL::File::close()
{
  if (_tmp) _tmp->close();
  else _file->close();
  bool ok = (_tmp ? _tmp->status() : _file->status())==IO_Ok;
  if ( !_file->name().isEmpty() ) {
    KIO::NetAccess::removeTempFile(_file->name());
    _file->setName(QString::null);
  }
  delete _stream;
  _stream = 0;
  if ( ok && _tmp && !_url.isEmpty() && !KIO::NetAccess::upload(_tmp->name(), _url.kurl(), qApp->mainWidget()) ) {
    _error = KIO::NetAccess::lastErrorString();
    ok = false;
    _log.sorry(i18n("Could not save file."), errorString());
  }
  delete _tmp;
  _tmp = 0;
  return ok;
}

bool PURL::File::openForRead()
{
  close();
  QString tmp;
  if ( !KIO::NetAccess::download(_url.kurl(), tmp, qApp->mainWidget()) ) {
    _error = KIO::NetAccess::lastErrorString();
    _log.sorry(i18n("Could not open file for reading."), errorString());
    return false;
  }
  _file->setName(tmp);
  if ( !_file->open(IO_ReadOnly) ) {
    _error = i18n("Could not open temporary file.");
    _log.sorry(_error, i18n("File: %1").arg(_file->name()));
    return false;
  }
  return true;
}

bool PURL::File::remove()
{
  close();
  if ( !_url.isEmpty() ) return _url.del(_log);
  return false;
}

//-----------------------------------------------------------------------------
PURL::TempFile::TempFile(Log::Generic &log, const QString &extension)
  : FileBase(log, extension)
{}

PURL::Url PURL::TempFile::url() const
{
  return (_tmp ? Url::fromPathOrUrl(_tmp->name()) : Url());
}

bool PURL::TempFile::close()
{
  delete _stream;
  _stream = 0;
  if (_tmp) {
    _tmp->close();
    if ( _tmp->status()!=IO_Ok ) {
      _error = i18n("Could not write to temporary file.");
      _log.sorry(_error, i18n("File: %1").arg(_tmp->name()));
      return false;
    }
  }
  return true;
}

bool PURL::TempFile::openForWrite()
{
  close();
  if (_tmp) delete _tmp;
  _tmp = new KTempFile(QString::null, _extension);
  _tmp->setAutoDelete(true);
  if ( _tmp->status()!=0 ) {
    _error = i18n("Could not create temporary file.");
    _log.sorry(_error, i18n("File: %1").arg(_tmp->name()));
    return false;
  }
  return true;
}
