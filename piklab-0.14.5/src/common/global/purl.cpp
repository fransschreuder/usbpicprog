/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "purl.h"

#include <qfileinfo.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qregexp.h>
#include <qmap.h>

#include "process.h"

//-----------------------------------------------------------------------------
class PURL::Private
{
public:
  QString convertWindowsFilepath(const QString &filepath);

private:
  QMap<char, QString>    _winDrives;  // drive -> unix path
  QMap<QString, QString> _winPaths;   // windows path -> unix path

  QString getWindowsDrivePath(char drive);
  bool checkCachedPath(QString &filepath) const;
  QString cachePath(const QString &origin, const QString &filepath);
  QString convertWindowsShortFilepath(const QString &filepath);
  QString findName(const QString &path, const QString &name);
  static QString findName(const QString &filepath);
};

QString PURL::Private::getWindowsDrivePath(char drive)
{
#if defined(Q_OS_UNIX)
  if ( !_winDrives.contains(drive) ) {
    QStringList args;
    args += "-u";
    QString s;
    s += drive;
    args += s + ":\\";
    ::Process::StringOutput process;
    process.setup("winepath", args, false);
    ::Process::State state = ::Process::runSynchronously(process, ::Process::Start, 3000);
    if ( state!=::Process::Exited ) qWarning("Error running \"winepath\" with \"%s\" (%i)", args.join(" ").latin1(), state);
    s = process.sout() + process.serr();
    QDir dir(s.stripWhiteSpace());
    _winDrives[drive] = dir.canonicalPath();
  }
  return _winDrives[drive];
#else
  return QString("%1:\\").arg(drive);
#endif
}

bool PURL::Private::checkCachedPath(QString &filepath) const
{
  if ( !_winPaths.contains(filepath) ) return false;
  filepath = _winPaths[filepath];
  return true;
}

QString PURL::Private::cachePath(const QString &origin, const QString &filepath)
{
  _winPaths[origin] = filepath;
  return filepath;
}

QString PURL::Private::convertWindowsFilepath(const QString &filepath)
{
  // appears to be an absolute windows path
  if ( filepath[0]=='\\' ) {
    QString tmp = filepath;
    if ( checkCachedPath(tmp) ) return tmp;
    return cachePath(filepath, convertWindowsShortFilepath(tmp.replace('\\', "/")));
  }
  // appears to be a windows path with a drive
  if ( (filepath.length()>=2 && filepath[0].isLetter() && filepath[1]==':') ) {
    QString tmp = filepath;
    if ( checkCachedPath(tmp) ) return tmp;
#if QT_VERSION<0x040000
    tmp = getWindowsDrivePath(filepath[0]) + tmp.mid(2).replace('\\', "/");
#else
    tmp = getWindowsDrivePath(filepath[0].toLatin1()) + tmp.mid(2).replace('\\', "/");
#endif
    return cachePath(filepath, convertWindowsShortFilepath(tmp));
  }
  return filepath;
}

QString PURL::Private::findName(const QString &path, const QString &name)
{
  QString filepath = path + '/' + name;
  if ( checkCachedPath(filepath) ) return filepath;
  return cachePath(filepath, findName(filepath));
}

QString PURL::Private::findName(const QString &filepath)
{
  QFileInfo finfo(filepath);
  if ( finfo.exists() || !finfo.dir().exists() ) return finfo.filePath();
  QStringList list = finfo.dir().entryList(QDir::All, QDir::Name);
  // find if name is just in a different case
  for (uint j=0; j<uint(list.count()); j++) {
    if ( list[j].lower()!=finfo.fileName().lower() ) continue;
    return finfo.dirPath() + '/' + list[j];
  }
  // find if name is a shorted filename
  QRegExp rexp("([^~]+)~(\\d+).*");
  if ( !rexp.exactMatch(finfo.fileName()) ) return finfo.filePath();
  QString start = rexp.cap(1).lower();
  uint index = rexp.cap(2).toUInt();
  uint k = 0;
  for (uint j = 0; j<uint(list.count()); j++) {
    if ( !list[j].lower().startsWith(start) ) continue;
    k++;
    if ( k==index ) return finfo.dirPath() + '/' + list[j];
  }
  return finfo.filePath();
}

QString PURL::Private::convertWindowsShortFilepath(const QString &filepath)
{
  // apparently "winepath" cannot do that for us and it is a real pain too...
  // we assume filepath is an absolute unix path
  // first see if we know the dirpath
  QFileInfo finfo(filepath);
  QString path = finfo.dirPath();
  if ( checkCachedPath(path) ) return findName(path, finfo.fileName());

  // otherwise go down the path
  QStringList names = QStringList::split('/', filepath);
  QString tmp;
  for (uint i=0; i<uint(names.count()); i++)
    tmp = findName(tmp, names[i]);
  return tmp;
}

//-----------------------------------------------------------------------------
PURL::Private *PURL::Base::_private = 0;

PURL::Base::Base(const QString &filepath)
{
  _empty = filepath.isEmpty();
  if ( !_empty ) {
    if ( _private==0 ) _private = new Private;
#if defined(Q_OS_UNIX)
    QString tmp = _private->convertWindowsFilepath(filepath);
#else
    QString tmp = filepath;
#endif
    if ( tmp.startsWith("~") ) tmp = QDir::homeDirPath() + tmp.mid(1);
    _relative = Q3Url::isRelativeUrl(tmp);
#if defined(Q_OS_UNIX)
    tmp = '/' + tmp;
#endif
#if defined(NO_KDE)
    _url.setPath(tmp);
#else
    _url = KURL::fromPathOrURL(tmp);
#endif
    _url.cleanPath();
  } else _relative = true;
}

PURL::Base::Base(const KURL &url)
  : _relative(false), _empty(url.isEmpty()), _url(url)
{
  _url.cleanPath();
}

bool PURL::Base::operator ==(const Base &url) const
{
  if ( _url.isEmpty() ) return url._url.isEmpty();
  return _url==url._url;
}

QString PURL::Base::path(bool forWindows) const
{
#if defined(NO_KDE)
  QString s = _url.dirPath();
  if ( !s.isEmpty() && !s.endsWith("/") ) s += '/';
#else
  QString s = _url.directory(false, false);
#endif
  if (forWindows) {
    for (uint i=0; i<uint(s.length()); i++)
      if ( s[i]=='/' ) s[i] = '\\';
  }
  return s;
}

QString PURL::Base::unterminatedPath(bool forWindows) const
{
#if defined(NO_KDE)
  QString s = _url.dirPath();
  if ( s.endsWith("/") ) s = s.mid(0, s.length()-1);
#else
  QString s = _url.directory(true, false);
#endif
  if (forWindows) {
    for (uint i=0; i<uint(s.length()); i++)
      if ( s[i]=='/' ) s[i] = '\\';
  }
  return s;
}

QString PURL::Base::pretty() const
{
#if defined(NO_KDE)
  QString s = _url.toString();
  if ( s.startsWith("://") ) return s.mid(3);
  return s;
#else
  return _url.prettyURL(0, KURL::StripFileProtocol);
#endif
}

PURL::Directory PURL::Base::directory() const
{
  return Directory(path(false));
}

bool PURL::Base::isInto(const Directory &dir) const
{
  return path(false).startsWith(dir.path(false));
}

//----------------------------------------------------------------------------
PURL::Url::Url(const QString &filepath)
  : Base(filepath)
{}

PURL::Url::Url(const Directory &dir, const QString &filename, FileType type)
  : Base(dir.path(false) + '/' + addExtension(filename, type))
{}

PURL::Url::Url(const Directory &dir, const QString &filepath)
  : Base(dir.path(false) + '/' + filepath)
{}

PURL::FileType PURL::Url::fileType() const
{
  QFileInfo info(filename());
  for (uint k=0; k<Nb_FileTypes; k++)
    for (uint i=0; DATA[k].extensions[i]; i++)
      if ( info.extension(false).lower()==DATA[k].extensions[i] ) return FileType(k);
  return Unknown;
}

QString PURL::Url::basename() const
{
  QFileInfo info(_url.fileName());
  return info.baseName(true);
}

QString PURL::Url::filename() const
{
  QFileInfo info(_url.fileName());
  return info.fileName();
}

QString PURL::Url::filepath(bool forWindows) const
{
  //qDebug("%s: %s + %s", _url.prettyURL().latin1(), path().latin1(), filename().latin1());
  return path(forWindows) + filename();
}

PURL::Url PURL::Url::toExtension(const QString &extension) const
{
  QFileInfo info(filename());
  return Url(directory().path(false) + info.baseName(true) + '.' + extension);
}

PURL::Url PURL::Url::appendExtension(const QString &extension) const
{
  QFileInfo info(filename());
  return Url(directory().path(false) + info.fileName() + '.' + extension);
}

QString PURL::Url::relativeTo(const Directory &dir, bool forWindows) const
{
  QString s = filepath(forWindows);
  if ( !isInto(dir) ) return s;
  return s.right(s.length() - dir.path(forWindows).length());
}

PURL::Url PURL::Url::toAbsolute(const Directory &dir) const
{
  if ( isRelative() ) return Url(dir, filepath(false));
  return *this;
}

bool PURL::findExistingUrl(Url &url)
{
  if ( url.probablyExists() ) return true;
  QFileInfo info(url.filename());
  Url tmp = url.toExtension(info.extension(false).upper());
  if ( !tmp.probablyExists() ) {
    tmp = url.toExtension(info.extension(false).lower());
    if ( !tmp.probablyExists() ) return false;
  }
  url = tmp;
  return true;
}

//-----------------------------------------------------------------------------
#if !defined(NO_KDE)
PURL::UrlList::UrlList(const KURL::List &list)
{
  KURL::List::const_iterator it;
  for (it=list.begin(); it!=list.end(); ++it) append(*it);
}
#endif

//-----------------------------------------------------------------------------
PURL::Directory::Directory(const QString &path)
  : Base(path.isEmpty() ? QString::null : path + '/')
{}

QStringList PURL::Directory::files(const QString &filter) const
{
  QDir dir(path(false));
  return dir.entryList(filter, QDir::Files);
}

PURL::Url PURL::Directory::findMatchingFilename(const QString &filename) const
{
  QDir dir(path(false));
  QStringList files = dir.entryList(QDir::Files);
  for (uint i=0; i<uint(files.count()); i++)
    if ( files[i].lower()==filename.lower() ) return Url(*this, files[i]);
  return Url(*this, filename);
}
