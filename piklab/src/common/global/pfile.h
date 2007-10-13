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
class QFile;
class KTempFile;

namespace PURL
{
//-----------------------------------------------------------------------------
class FileBase
{
public:
  FileBase(Log::View &view, const QString &extension);
  virtual ~FileBase();
  virtual bool close() = 0;
  QFile *qfile();
  const QFile *qfile() const;
  virtual Url url() const = 0;
  QTextStream &stream();
  QString readText() { return stream().read(); }
  QString readLine() { return stream().readLine(); }
  QStringList readLines();
  QByteArray readAll();
  void appendText(const QString &text) { stream() << text; }
  void flush();
  bool hasError() const;
  QString errorString() const;
  bool openForWrite();

protected:
  KTempFile    *_tmp;
  QFile        *_file;
  QTextStream  *_stream;
  QString       _error, _extension;
  Log::View    &_view;

private: // disable copy constructor and operator =
  FileBase(const FileBase &base);
  FileBase &operator =(const FileBase &base);
};

class File : public FileBase
{
public:
  File(const Url &url, Log::View &view);
  virtual ~File() { close(); }
  void setUrl(const Url &url); // close file too
  virtual Url url() const { return _url; }
  bool openForRead();
  virtual bool close();
  bool remove();

private:
  Url _url;
};

//-----------------------------------------------------------------------------
#if !defined(NO_KDE)
class TempFile : public FileBase
{
public:
  TempFile(Log::View &view, const QString &extension = QString::null);
  virtual Url url() const;
  virtual bool close();
};
#endif

} // namespace

#endif
