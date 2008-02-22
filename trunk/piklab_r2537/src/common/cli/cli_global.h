/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CLI_GLOBAL_H
#define CLI_GLOBAL_H

#include <qstring.h>
namespace PURL { class Directory; }

namespace CLI
{

class View;
class MainBase;
enum ExitCode { EXITING = 1, OK = 0, ARG_ERROR = -1, NOT_SUPPORTED_ERROR = -2,
                FILE_ERROR = -3, EXEC_ERROR = -4 };
extern ExitCode errorExit(const QString &message, ExitCode code);
extern ExitCode okExit(const QString &message);
extern PURL::Directory runDirectory();

extern bool      _force;
extern bool      _isInteractive;
extern View     *_view;
extern MainBase *_main;

} // namespace

#endif
