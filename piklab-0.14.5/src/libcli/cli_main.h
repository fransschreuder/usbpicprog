/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CLI_MAIN_H
#define CLI_MAIN_H

#include "common/global/global.h"
#include "common/global/log.h"
#include "cli_global.h"

namespace CLI
{
enum Property { NoProperty = 0, HasForce = 1, HasInteractiveMode = 2 };
Q_DECLARE_FLAGS(Properties, Property)
Q_DECLARE_OPERATORS_FOR_FLAGS(Properties)

struct CommandData {
  const char *name;
  int properties;
  const char *help;
};
extern const CommandData NORMAL_COMMAND_DATA[];
extern const CommandData INTERACTIVE_COMMAND_DATA[];
extern const CommandData *findCommandData(const QString &command);
extern ExitCode findCommand(const QString &s);
class KeyList {
public:
  void append(const QString &key, const QString &label) { _keys += key; _labels += label; }
  void display() const;

private:
  QStringList _keys, _labels;
};
extern ExitCode commandList();

//-----------------------------------------------------------------------------
class OptionList : public QValueList<KCmdLineOptions>
{
public:
  OptionList(Properties properties);
  OptionList(Properties properties, const KCmdLineOptions *options);
  virtual ~OptionList() { delete _options; }
  const KCmdLineOptions *ptr() const;

private:
  mutable KCmdLineOptions *_options;

  void init(Properties properties);
};

//-----------------------------------------------------------------------------
class MainBase : public QObject, public Log::Base
{
Q_OBJECT
public:
  MainBase();
  virtual OptionList optionList() const = 0;
  virtual void init(Properties properties);
  virtual ExitCode run() = 0;

protected:
  KCmdLineArgs *_args;
};

} // namespace

#endif
