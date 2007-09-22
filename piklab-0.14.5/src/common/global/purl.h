/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PURL_H
#define PURL_H

#include "common/common/purl_base.h"
#include "common/global/global.h"
#include "common/global/log.h"

namespace PURL
{
class Url;
class Directory;
class Private;

//----------------------------------------------------------------------------
class Base
{
public:
  Base(const QString &filepath = QString::null);
  Base(const KURL &url);
  bool operator <(const Base &url) const { return _url<url._url; }
  bool operator ==(const Base &url) const;
  bool operator !=(const Base &url) const { return !(_url==url._url); }
  const KURL &kurl() const { return _url; }
  QString pretty() const;
  bool isEmpty() const { return _empty; }
  bool isLocal() const { return _url.isLocalFile(); }
  QString path(bool forWindows) const; // with ending '/' unless empty path
  QString unterminatedPath(bool forWindows) const; // no ending '/'
  Directory directory() const;
  bool isInto(const Directory &dir) const;
  bool isRelative() const { return _relative; }
  bool probablyExists() const;

protected:
  bool _relative, _empty;
  KURL _url;
  mutable QString _error;
  static Private *_private;
};

//----------------------------------------------------------------------------
class Url : public Base
{
public:
  Url(const QString &filePath = QString::null);
  Url(const KURL &url) : Base(url) {}
  // add correct extension if filename has no extension
  Url(const Directory &path, const QString &filename, FileType type);
  Url(const Directory &path, const QString &filepath);

  Url toFileType(FileType type) const { return toExtension(DATA[type].extensions[0]); }
  Url toExtension(const QString &extension) const;
  Url appendExtension(const QString &extension) const;

  const Data &data() const { return DATA[fileType()]; }
  FileType fileType() const;
  QString basename() const; // filename without extension
  QString filename() const; // filename without path
  QString filepath(bool forWindows) const; // filename with path
  bool isDestinationUpToDate(FileType type) const;
  QString relativeTo(const Directory &dir, bool forWindows) const;
  Url toAbsolute(const Directory &dir) const;
#if !defined(NO_KDE)
  bool isDosFile() const;
  bool create(Log::View *view) const; // do not overwrite
  bool write(const QString &text, Log::View *view) const;
  bool copyTo(const Url &destination, Log::View *view) const; // do not overwrite
  bool del(Log::View *view) const;
#endif
};

extern bool findExistingUrl(Url &url); // may transform extension's case if needed

//----------------------------------------------------------------------------
class UrlList : public QValueList<Url>
{
public:
  UrlList() {}
  UrlList(const Url &url) { append(url); }
  UrlList(const QValueList<Url> &list) : QValueList<Url>(list) {}
#if !defined(NO_KDE)
  UrlList(const KURL::List &list);
#endif
};

//----------------------------------------------------------------------------
class Directory : public Base
{
public:
  Directory(const QString &path = QString::null);
  QStringList files(const QString &filter) const;
  Url findMatchingFilename(const QString &filename) const;
#if !defined(NO_KDE)
  bool create(Log::View *view) const;
#endif
};

} // namespace

#endif
