/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CLI_INTERACTIVE_H
#define CLI_INTERACTIVE_H

#include <qfile.h>
#include "common/global/global.h"
#include "libcli/cli_global.h"
#include "common/global/log.h"

namespace CLI
{
//----------------------------------------------------------------------------
enum CommandProperty { NoCommandProperty = 0, NeedProgrammer = 1, InputHex = 2,
                       OutputHex = 4, NeedDevice = 8 };
Q_DECLARE_FLAGS(CommandProperties, CommandProperty)
Q_DECLARE_OPERATORS_FOR_FLAGS(CommandProperties)
struct PropertyData
{
  const char *name, *optName, *alias, *help, *list, *listHelp;
};
extern const PropertyData PROPERTY_DATA[];
extern bool isPropertyList(const QString &s);
extern bool isProperty(const QString &s);

//----------------------------------------------------------------------------
class Interactive : public QObject, public Log::Base
{
Q_OBJECT
public:
  Interactive(QObject *parent);
  void redisplayPrompt();

private slots:
  void displayPrompt();

private:
  QFile   _stdin;
  QString _input;
  void lineRead();
  ExitCode processLine(const QString &s);
  ExitCode executeRegister(const QString &name, const QString &value);
  ExitCode registerList();
  ExitCode variableList();
  ExitCode executeRawCommands(const QString &filename);
  uint findRegisterAddress(const QString &name);
  ExitCode start();
  static void signalHandler(int n);
};

} // namespace

#endif
