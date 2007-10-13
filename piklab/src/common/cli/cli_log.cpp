/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "cli_log.h"

#include "common/global/global.h"
#include "cli_global.h"

void CLI::View::doLog(Log::Type type, const QString &text, Log::Action)
{
  QString s = text + "\n";
  switch (type) {
    case Log::Error:
    case Log::SoftError: s = "Error: " + s; break;
    case Log::Warning: s = "Warning: " + s; break;
    default: break;
  }
#if QT_VERSION<0x040000
  if ( type==Log::Error || type==Log::SoftError ) fprintf(stderr, "%s", s.latin1());
  else fprintf(stdout, "%s", s.latin1());
#else
  QByteArray ba = s.toLatin1();
  if ( type==Log::Error || type==Log::SoftError ) fprintf(stderr, "%s", ba.constData());
  else fprintf(stdout, "%s", ba.constData());
#endif
}

void CLI::View::doLog(Log::DebugLevel, const QString &text, Log::Action)
{
  QString s = text + "\n";
#if QT_VERSION<0x040000
  fprintf(stdout, "%s", s.latin1());
#else
  QByteArray ba = s.toLatin1();
  fprintf(stdout, "%s", ba.constData());
#endif
}

void CLI::View::appendToLastLine(const QString &text)
{
#if QT_VERSION<0x040000
  fprintf(stdout, "%s", text.latin1());
#else
  QByteArray ba = text.toLatin1();
  fprintf(stdout, "%s", ba.constData());
#endif
}

void CLI::View::sorry(const QString &message, const QString &details)
{
  if ( details.isEmpty() ) log(Log::Error, message, Log::Immediate);
  else log(Log::Error, message + " (" + details + ")", Log::Immediate);
}

bool CLI::View::askContinue(const QString &message)
{
  log(Log::Warning, message + " " + (_force ? i18n("*yes*") : i18n("*no*")), Log::Immediate);
  if (_force) return true;
  if ( !_isInteractive ) return false; // always fail
  // #### TODO
  return false;
}

void CLI::View::logUserAbort()
{
  if ( !_isInteractive ) return;
  return;
  //Log::View::logUserAbort();
}
