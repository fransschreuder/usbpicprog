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

#include "common/global/about.h"
#include "common/global/log.h"
#include "cli_global.h"

namespace CLI
{
//-----------------------------------------------------------------------------
enum Property { NoProperty = 0, HasForce = 1, HasInteractiveMode = 2 };
Q_DECLARE_FLAGS(Properties, Property)
Q_DECLARE_OPERATORS_FOR_FLAGS(Properties)

extern const KCmdLineOptions OPTIONS[];
//-----------------------------------------------------------------------------
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
struct PropertyData
{
  const char *name, *optName, *alias, *help, *list, *listHelp;
};
extern const PropertyData PROPERTY_DATA[];
extern bool isPropertyList(const QString &s);
extern bool isProperty(const QString &s);
extern ExitCode propertyList();

//-----------------------------------------------------------------------------
class OptionList : public Piklab::OptionList
{
public:
  OptionList(Properties properties);
  OptionList(Properties properties, const KCmdLineOptions *options);

private:
  void init(Properties properties);
};

//-----------------------------------------------------------------------------
class MainBase : public QObject, public Log::Base
{
Q_OBJECT
public:
  MainBase(Properties properties);
  virtual OptionList optionList(const char *fileDescription) const;
  virtual ExitCode doRun();
  virtual ExitCode list(const QString &listName);
  virtual ExitCode prepareCommand(const QString &command) = 0;
  virtual ExitCode executeCommand(const QString &command) = 0;
  virtual ExitCode executeSetCommand(const QString &property, const QString &value) = 0;
  virtual QString executeGetCommand(const QString &property) = 0;

protected:
  Properties    _properties;
  KCmdLineArgs *_args;

private:
  virtual ExitCode prepareRun(bool &interactive) = 0;
  virtual void init();
};

} // namespace

#endif
