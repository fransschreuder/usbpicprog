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

bool PURL::Url::copyTo(const Url &destination, Log::View &view) const
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
  if ( !ok ) view.sorry(i18n("Could not copy file"), KIO::NetAccess::lastErrorString());
  return ok;
//#endif
}

bool PURL::Url::create(Log::View &view) const
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
  if ( !ok ) view.sorry(i18n("Could not create file"), KIO::NetAccess::lastErrorString());
//#endif
  return ok;
}

bool PURL::Url::write(const QString &text, Log::View &view) const
{
  File file(*this, view);
  if ( !file.openForWrite() ) return false;
  file.stream() << text;
  return file.close();
}

bool PURL::Url::del(Log::View &view) const
{
//#if defined(NO_KDE)
//  Synchronous sync;
//  if ( sync.op().remove(_url.filepath()) && sync.execute() ) {
//    if ( show==Log::Show ) ConsoleView::sorry(i18n("Could not delete file"), sync.error());
//    return false;
//  }
//#else
  bool ok = KIO::NetAccess::del(_url, qApp->mainWidget());
  if ( !ok ) view.sorry(i18n("Could not delete file."), KIO::NetAccess::lastErrorString());
  return ok;
//#endif
}

bool PURL::Url::isDosFile() const
{
  Log::StringView sview;
  File file(*this, sview);
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
bool PURL::Directory::create(Log::View &view) const
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
  bool ok = KIO::NetAccess::mkdir(_url, qApp->mainWidget());
  if ( !ok ) view.sorry(i18n("Could not create directory"), KIO::NetAccess::lastErrorString());
//#endif
  return ok;
}
