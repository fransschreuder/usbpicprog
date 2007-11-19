/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PFILE_H
#define PFILE_H

#include <qtextstream.h>
#include "purl.h"

namespace PURL
{
//-----------------------------------------------------------------------------
class FileBase
{
public:
  FileBase(Log::Generic &log, const QString &extension);
  ~FileBase();
  QFile *qfile();
  const QFile *qfile() const;
  QTextStream &stream();
  QString readText() { return stream().read(); }
  QString readLine() { return stream().readLine(); }
  QStringList readLines();
  QByteArray readAll();
  void appendText(const QString &text) { stream() << text; }
  void flush();
  bool hasError() const;
  QString errorString() const;

protected:
  KTempFile    *_tmp;
  QFile        *_file;
  QTextStream  *_stream;
  QString       _error, _extension;
  Log::Generic &_log;

private: // disable copy constructor and operator =
  FileBase(const FileBase &base);
  FileBase &operator =(const FileBase &base);
};

//-----------------------------------------------------------------------------
class File : public FileBase
{
public:
  File(const Url &url, Log::Generic &log);
  ~File() { close(); }
  void setUrl(const Url &url); // close file too
  Url url() const { return _url; }
  bool openForWrite();
  bool openForRead();
  bool close();
  bool remove();

private:
  Url _url;
};

} // namespace

#endif
