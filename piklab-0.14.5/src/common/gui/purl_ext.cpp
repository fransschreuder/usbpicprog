/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "common/global/purl.h"

#include <qfile.h>
#include <qapplication.h>
#include <kio/netaccess.h>
#include <kfileitem.h>
#include <ktempfile.h>

#include "common/global/pfile.h"

bool PURL::Base::probablyExists() const
{
  // assume file exists if ioslave cannot tell...
  return KIO::NetAccess::exists(_url, true, qApp->mainWidget());
}

bool PURL::Url::copyTo(const Url &destination, Log::View *view) const
{
//#if defined(NO_KDE)
//  Synchronous sync;
//  if ( sync.op().copy(_url.filepath(), destination.filepath()) && sync.execute() ) {
//    if ( show==Log::Show ) ConsoleView::sorry(i18n("Could not copy file"), sync.error());
//    return false;
//  }
//#else
  // do not overwrite
  bool ok = KIO::NetAccess::file_copy(_url, destination._url, -1, false, false, qApp->mainWidget());
  if ( !ok ) {
    _error = KIO::NetAccess::lastErrorString();
    if (view) view->sorry(i18n("Could not copy file"), _error);
  }
  return ok;
//#endif
}

bool PURL::Url::create(Log::View *view) const
{
//#if defined(NO_KDE)
//  QByteArray a;
//  Synchronous sync;
//  if ( sync.op().put(a, _url.filepath()) && sync.execute() ) {
//    if ( show==Log::Show ) ConsoleView::sorry(i18n("Could not create file"), sync.error());
//    return false;
//  }
//#else
  // assume file do no exist if ioslave cannot tell...
  if ( KIO::NetAccess::exists(_url, false, qApp->mainWidget()) ) return true;
  KTempFile tmp;
  tmp.setAutoDelete(true);
  // do not overwrite
  bool ok = KIO::NetAccess::file_copy(tmp.name(), _url, -1, false, false, qApp->mainWidget());
  if ( !ok ) {
    _error = KIO::NetAccess::lastErrorString();
    if (view) view->sorry(i18n("Could not create file"), _error);
  }
//#endif
  return ok;
}

bool PURL::Url::write(const QString &text, Log::View *view) const
{
  File file(*this, view);
  if ( file.openForWrite() ) file.stream() << text;
  bool ok = file.close();
  _error = file.errorString();
  return ok;
}

bool PURL::Url::del(Log::View *view) const
{
//#if defined(NO_KDE)
//  Synchronous sync;
//  if ( sync.op().remove(_url.filepath()) && sync.execute() ) {
//    if ( show==Log::Show ) ConsoleView::sorry(i18n("Could not delete file"), sync.error());
//    return false;
//  }
//#else
  bool ok = KIO::NetAccess::del(_url, qApp->mainWidget());
  if ( !ok ) {
    _error = KIO::NetAccess::lastErrorString();
    if (view) view->sorry(i18n("Could not delete file."), _error);
  }
  return ok;
//#endif
}

bool PURL::Url::isDestinationUpToDate(FileType type) const
{
  if ( !(data().properties & Source) ) return true;
  Url dest = toFileType(type);
  KIO::UDSEntry suds, duds;
  if ( !KIO::NetAccess::stat(_url, suds, qApp->mainWidget()) ) return true; // source does not exists
  if ( !KIO::NetAccess::stat(dest._url, duds, qApp->mainWidget()) ) return false; // destination does not exists
  KFileItem sitem(suds, _url), ditem(duds, dest._url);
  return ( sitem.time(KIO::UDS_MODIFICATION_TIME)<ditem.time(KIO::UDS_MODIFICATION_TIME) );
}

bool PURL::Url::isDosFile() const
{
  File file(*this, 0);
  if( !file.openForRead() ) return false;
  int oldc = 0;
  for (;;) {
    int c = file.qfile()->getch();
    if ( c==-1 )  break;
    if( c=='\n' && oldc=='\r' ) return true;
    oldc = c;
  }
  return false;
}

//-----------------------------------------------------------------------------
bool PURL::Directory::create(Log::View *view) const
{
//#if defined(NO_KDE)
//  Synchronous sync;
//  if ( sync.op().mkdir(_url.filepath()) && sync.execute() ) {
//    if ( show==Log::Show ) ConsoleView::sorry(i18n("Could not create directory"), sync.error());
//    return false;
//  }
//#else
  // assume dir do no exist if ioslave cannot tell...
  if ( KIO::NetAccess::exists(_url, false, qApp->mainWidget()) ) return true;
  if ( !KIO::NetAccess::mkdir(_url, qApp->mainWidget()) ) {
    _error = KIO::NetAccess::lastErrorString();
    if (view) view->sorry(i18n("Could not create directory"), _error);
    return false;
  }
//#endif
  return true;
}
