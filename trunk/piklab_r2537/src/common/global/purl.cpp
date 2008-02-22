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
#if QT_VERSION<0x040000
#  include <qnetwork.h>
#endif

#include "common/common/synchronous.h"
#include "process.h"

#if !defined(NO_KDE)
# include <kio/netaccess.h>
# include <kfileitem.h>
# include <kconfigbackend.h>
#endif

//-----------------------------------------------------------------------------
PURL::Http::Http(const QString &hostname)
  : QHttp(hostname)
{
  connect(this, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
          SLOT(responseHeaderReceivedSlot(const QHttpResponseHeader &)));
}

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
  if ( filepath.endsWith("/") ) tmp += "/";
  return tmp;
}

//-----------------------------------------------------------------------------
PURL::Private *PURL::Base::_private = 0;

PURL::Base::Base(const QString &filepath)
  : _relative(true)
{
  if ( !filepath.isEmpty() ) {
    if ( _private==0 ) _private = new Private;
#if defined(Q_OS_UNIX)
    QString tmp = _private->convertWindowsFilepath(filepath);
#else
    QString tmp = filepath;
#endif
    if ( tmp.startsWith("~") ) tmp = QDir::homeDirPath() + tmp.mid(1);
    _relative = Q3Url::isRelativeUrl(tmp);
#if defined(Q_OS_UNIX)
    if ( !tmp.startsWith("/") ) tmp = '/' + tmp;
#endif
#if defined(NO_KDE)
    _url.setPath(tmp);
#else
    _url = KURL::fromPathOrURL(tmp);
#endif
    _url.cleanPath();
  }
}

PURL::Base::Base(const KURL &url)
  : _relative(false), _url(url)
{
  _url.cleanPath();
}

bool PURL::Base::isLocal() const
{
  return ( _url.protocol().isEmpty() || _url.protocol()=="file" );
}

bool PURL::Base::operator ==(const Base &url) const
{
  if ( _url.isEmpty() ) return url._url.isEmpty();
  return _url==url._url;
}

QString PURL::Base::path(SeparatorType type) const
{
#if defined(NO_KDE)
  QString s = _url.dirPath();
  if ( !s.isEmpty() && !s.endsWith("/") ) s += '/';
#else
  QString s = _url.directory(false, false);
#endif
  if ( type==WindowsSeparator ) {
    for (uint i=0; i<uint(s.length()); i++)
      if ( s[i]=='/' ) s[i] = '\\';
  }
  return s;
}

QString PURL::Base::unterminatedPath(SeparatorType type) const
{
#if defined(NO_KDE)
  QString s = _url.dirPath();
  if ( s.endsWith("/") ) s = s.mid(0, s.length()-1);
#else
  QString s = _url.directory(true, false);
#endif
  if ( type==WindowsSeparator ) {
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
  return Directory(path());
}

bool PURL::Base::isInto(const Directory &dir) const
{
  return path().startsWith(dir.path());
}

bool PURL::Base::httpUrlExists(bool *ok) const
{
#if QT_VERSION<0x040000
  qInitNetworkProtocols();
#endif
  if (ok) *ok = false;
  Http http(_url.host());
  Synchronous sync(500);
  QObject::connect(&http, SIGNAL(done(bool)), &sync, SLOT(done()));
  QFileInfo info(_url.fileName(false));
  http.head(_url.path());
  if ( !sync.enterLoop() ) return false; // timeout
  if ( http.error()!=QHttp::NoError ) return false;
  if (ok ) *ok = true;
  return ( http._header.statusCode()==200 );
}

bool PURL::Base::exists(QDateTime *lastModified) const
{
  if ( isEmpty() ) return false;
  if ( isLocal() ) {
    QFileInfo fi(_url.path());
    if (lastModified) *lastModified = fi.lastModified();
    return fi.exists();
  }
  if ( _url.protocol()=="http" ) return httpUrlExists();
#if !defined(NO_KDE)
  if (lastModified) {
    KIO::UDSEntry uds;
    if ( !KIO::NetAccess::stat(_url, uds, qApp->mainWidget()) ) return false;
    KFileItem item(uds, _url);
    lastModified->setTime_t(item.time(KIO::UDS_MODIFICATION_TIME));
    return true;
  } else {
    // assume file exists if ioslave cannot tell...
    return KIO::NetAccess::exists(_url, true, qApp->mainWidget());
  }
#else
  if (lastModified) lastModified->setTime_t(0);
  // assume file exists
  return true;
#endif
}

//----------------------------------------------------------------------------
PURL::Url PURL::Url::fromPathOrUrl(const QString &s)
{
  KURL kurl = KURL::fromPathOrURL(s);
  if ( !kurl.protocol().isEmpty() && kurl.protocol()!="file" && kurl.protocol().length()!=1 ) return kurl;
  return Url(s.startsWith("file://") ? s.mid(7) : s);
}

PURL::Url::Url(const Directory &dir, const QString &filename, FileType type)
  : Base(dir.path() + '/' + addExtension(filename, type))
{}

PURL::Url::Url(const Directory &dir, const QString &filepath)
  : Base(dir.path() + '/' + filepath)
{}

PURL::FileType PURL::Url::fileType() const
{
  QFileInfo info(filename());
  FOR_EACH(FileType, type)
    for (uint i=0; type.data().extensions[i]; i++)
      if ( info.extension(false).lower()==type.data().extensions[i] ) return type;
  return Unknown;
}

QString PURL::Url::basename() const
{
  QFileInfo info(_url.fileName(false));
  return info.baseName(true);
}

QString PURL::Url::filename() const
{
  QFileInfo info(_url.fileName(false));
  return info.fileName();
}

QString PURL::Url::filepath(SeparatorType type) const
{
  return path(type) + filename();
}

PURL::Url PURL::Url::toExtension(const QString &extension) const
{
  QFileInfo info(filename());
  return Url(directory().path() + info.baseName(true) + '.' + extension);
}

PURL::Url PURL::Url::appendExtension(const QString &extension) const
{
  QFileInfo info(filename());
  return Url(directory().path() + info.fileName() + '.' + extension);
}

QString PURL::Url::relativeTo(const Directory &dir, SeparatorType type) const
{
  QString s = filepath(type);
  if ( !isInto(dir) ) return s;
  return s.right(s.length() - dir.path(type).length());
}

PURL::Url PURL::Url::toAbsolute(const Directory &dir) const
{
  if ( isRelative() ) return Url(dir, filepath());
  return *this;
}

bool PURL::findExistingUrl(Url &url)
{
  if ( url.exists() ) return true;
  QFileInfo info(url.filename());
  Url tmp = url.toExtension(info.extension(false).upper());
  if ( !tmp.exists() ) {
    tmp = url.toExtension(info.extension(false).lower());
    if ( !tmp.exists() ) return false;
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

PURL::Directory PURL::Directory::up() const
{
  QDir dir(path());
  dir.cdUp();
  return PURL::Directory(dir.path());
}

PURL::Directory PURL::Directory::down(const QString &subPath) const
{
  Q_ASSERT( QDir::isRelativePath(subPath) );
  QDir dir(path());
  dir.cd(subPath);
  return PURL::Directory(dir.path());
}

QStringList PURL::Directory::files(const QString &filter) const
{
  QDir dir(path());
  return dir.entryList(filter, QDir::Files);
}

PURL::Url PURL::Directory::findMatchingFilename(const QString &filename) const
{
  QDir dir(path());
  QStringList files = dir.entryList(QDir::Files);
  for (uint i=0; i<uint(files.count()); i++)
    if ( files[i].lower()==filename.lower() ) return Url(*this, files[i]);
  return Url(*this, filename);
}

PURL::Directory PURL::Directory::current()
{
  return QDir::currentDirPath();
}
