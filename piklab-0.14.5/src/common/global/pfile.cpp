/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pfile.h"

#include <qfile.h>

//-----------------------------------------------------------------------------
PURL::FileBase::FileBase(Log::View *view, const QString &extension)
  : _tmp(0), _file(0), _stream(0), _extension(extension), _view(view)
{}

void PURL::FileBase::flush()
{
  if ( qfile() ) qfile()->flush();
}

QTextStream &PURL::FileBase::stream()
{
  if ( _stream==0 ) _stream = new QTextStream(qfile());
  return *_stream;
}

bool PURL::FileBase::hasError() const
{
  if ( qfile()==0 || !_error.isEmpty() ) return true;
  return ( uint(qfile()->status())!=IO_Ok );
}

QString PURL::FileBase::errorString() const
{
  if ( _error.isEmpty() ) {
    if ( qfile()==0 ) return i18n("File not open.");
    else return qfile()->errorString();
  }
  return _error;
}

QStringList PURL::FileBase::readLines()
{
  QStringList lines;
  for (;;) {
    QString s = stream().readLine();
    if ( s.isNull() ) break;
    lines.append(s);
  }
  return lines;
}

QByteArray PURL::FileBase::readAll()
{
  if ( qfile() ) return qfile()->readAll();
  return QByteArray();
}

//-----------------------------------------------------------------------------
void PURL::File::setUrl(const Url &url)
{
  close();
  _url = url;
}
