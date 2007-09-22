/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "cli_global.h"

#include <qdir.h>

#include "common/global/purl.h"
#include "cli_log.h"

bool CLI::_force = false;
bool CLI::_isInteractive = false;
CLI::View     *CLI::_view = 0;
CLI::MainBase *CLI::_main = 0;

CLI::ExitCode CLI::errorExit(const QString &message, ExitCode code)
{
  Q_ASSERT( code!=OK );
  _view->log(Log::SoftError, message);
  return code;
}

CLI::ExitCode CLI::okExit(const QString &message)
{
  _view->log(Log::Information, message);
  return OK;
}

PURL::Directory CLI::runDirectory()
{
  return PURL::Directory(QDir::currentDirPath());
}
