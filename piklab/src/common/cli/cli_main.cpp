/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "cli_main.h"

#include "cli_log.h"
#include "common/global/about.h"

//-----------------------------------------------------------------------------
const CLI::CommandData *CLI::findCommandData(const QString &command)
{
  for (uint i=0; NORMAL_COMMAND_DATA[i].name; i++)
    if ( NORMAL_COMMAND_DATA[i].name==command ) return &NORMAL_COMMAND_DATA[i];
  if ( !_isInteractive) return 0;
  for (uint i=0; INTERACTIVE_COMMAND_DATA[i].name; i++)
    if ( INTERACTIVE_COMMAND_DATA[i].name==command ) return &INTERACTIVE_COMMAND_DATA[i];
  return 0;
}

CLI::ExitCode CLI::findCommand(const QString &s)
{
  if ( s.isEmpty() ) return errorExit(i18n("No command specified"), ARG_ERROR);
  const CommandData *data = findCommandData(s);
  if ( data==0 ) return errorExit(i18n("Unknown command: %1").arg(s), ARG_ERROR);
  return OK;
}

//-----------------------------------------------------------------------------
bool CLI::isPropertyList(const QString &s)
{
  for (uint i=0; PROPERTY_DATA[i].name; i++)
    if ( s==PROPERTY_DATA[i].list ) return true;
  return false;
}

bool CLI::isProperty(const QString &s)
{
  for (uint i=0; PROPERTY_DATA[i].name; i++)
    if ( s==PROPERTY_DATA[i].name ) return true;
  return false;
}

//-----------------------------------------------------------------------------
const KCmdLineOptions STANDARD_OPTIONS[] = {
  { "c",              0, 0 },
  { "command <name>", I18N_NOOP("Perform the requested command."), 0 },
  { "command-list",   I18N_NOOP("Return the list of recognized commands."), 0 },
  { "debug",          I18N_NOOP("Display debug messages."), 0 },
  { "extra-debug",    I18N_NOOP("Display extra debug messages."), 0 },
  { "max-debug",      I18N_NOOP("Display all debug messages."), 0 },
  { "lowlevel-debug", I18N_NOOP("Display low level debug messages."), 0 },
  { "quiet",          I18N_NOOP("Do not display messages."), 0 },
  KCmdLineLastOption
};

const KCmdLineOptions FORCE_OPTIONS[] = {
  { "f",              0, 0 },
  { "force",          I18N_NOOP("Overwrite files and answer \"yes\" to questions."), 0 },
  KCmdLineLastOption
};

const KCmdLineOptions INTERACTIVE_OPTIONS[] = {
  { "i",              0, 0 },
  { "cli",            I18N_NOOP("Interactive mode"), 0 },
  KCmdLineLastOption
};

CLI::OptionList::OptionList(Properties properties)
{
  init(properties);
}

CLI::OptionList::OptionList(Properties properties, const KCmdLineOptions *options)
{
  init(properties);
  for (uint i=0; options[i].name; i++) append(options[i]);
}

void CLI::OptionList::init(Properties properties)
{
  for (uint i=0; STANDARD_OPTIONS[i].name; i++) append(STANDARD_OPTIONS[i]);
  if ( properties & HasForce ) for (uint i=0; FORCE_OPTIONS[i].name; i++) append(FORCE_OPTIONS[i]);
  if ( properties & HasInteractiveMode ) for (uint i=0; INTERACTIVE_OPTIONS[i].name; i++) append(INTERACTIVE_OPTIONS[i]);
}

void CLI::KeyList::display() const
{
  uint nb = 0;
  for (uint i=0; i<uint(_keys.count()); i++) nb = qMax(nb, uint(_keys[i].length()));
  for (uint i=0; i<uint(_keys.count()); i++) _view->log(Log::Data, " " + _keys[i].leftJustify(nb+2) + _labels[i]);
}

CLI::ExitCode CLI::commandList()
{
  _view->log(Log::Data, i18n("Supported commands:"));
  KeyList keys;
  for (uint i=0; NORMAL_COMMAND_DATA[i].name; i++)
    keys.append(NORMAL_COMMAND_DATA[i].name, i18n(NORMAL_COMMAND_DATA[i].help));
  if (_isInteractive) {
    for (uint i=0; INTERACTIVE_COMMAND_DATA[i].name; i++)
      keys.append(INTERACTIVE_COMMAND_DATA[i].name, i18n(INTERACTIVE_COMMAND_DATA[i].help));
  }
  keys.display();
  return OK;
}

CLI::ExitCode CLI::propertyList()
{
  KeyList keys;
  for (uint i=0; PROPERTY_DATA[i].name; i++)
    keys.append(PROPERTY_DATA[i].name, i18n(PROPERTY_DATA[i].help));
  keys.display();
  return OK;
}

//-----------------------------------------------------------------------------
CLI::MainBase::MainBase(Properties properties)
  : QObject(0, "main"), _properties(properties)
{
  Q_ASSERT( _main==0 );
  _main = this;
  _view = new View;
  setView(_view);
}

void CLI::MainBase::init()
{
  _args = KCmdLineArgs::parsedArgs();
  if ( _properties & HasInteractiveMode ) _isInteractive = _args->isSet("cli");
  if ( _properties & HasForce ) _force = _args->isSet("force");
  if ( _args->isSet("debug") ) _view->setDebugLevel(Log::NormalDebug);
  if ( _args->isSet("extra-debug") ) _view->setDebugLevel(Log::ExtraDebug);
  if ( _args->isSet("max-debug") ) _view->setDebugLevel(Log::MaxDebug);
  if ( _args->isSet("lowlevel-debug") ) _view->setDebugLevel(Log::LowLevelDebug);
  if (_args->isSet("quiet")) _view->setQuiet();
}

CLI::OptionList CLI::MainBase::optionList(const char *fileDescription) const
{
  OptionList list(_properties, OPTIONS);
  KCmdLineOptions opt;
  for (uint i=0; PROPERTY_DATA[i].name; i++) {
    opt.description = 0;
    opt.def = 0;
    if ( PROPERTY_DATA[i].help==0 ) {
      opt.name = PROPERTY_DATA[i].name; // alias
      list.append(opt);
    } else {
      if ( PROPERTY_DATA[i].optName==0 ) continue; // interactive only
      if ( PROPERTY_DATA[i].alias ) {
        opt.name = PROPERTY_DATA[i].alias;
        list.append(opt);
      }
      opt.name = PROPERTY_DATA[i].optName;
      opt.description = PROPERTY_DATA[i].help;
      list.append(opt);
      if ( PROPERTY_DATA[i].list ) {
        opt.name = PROPERTY_DATA[i].list;
        opt.description = PROPERTY_DATA[i].listHelp;
        list.append(opt);
      }
    }
  }
  if (fileDescription) {
    opt.name = "+[file]";
    opt.description = fileDescription;
    opt.def = 0;
  }
  list.append(opt);
  return list;
}

CLI::ExitCode CLI::MainBase::list(const QString &command)
{
  if ( command=="command-list" ) return commandList();
  if ( command=="property-list" ) return propertyList();
  return ARG_ERROR;
}

CLI::ExitCode CLI::MainBase::doRun()
{
  init();

  // process set options
  for (uint i=0; PROPERTY_DATA[i].name; i++) {
    if ( PROPERTY_DATA[i].optName==0 ) continue; // alias or interactive only
    if ( !_args->isSet(PROPERTY_DATA[i].name) ) continue;
    QString option = _args->getOption(PROPERTY_DATA[i].name);
    if ( PROPERTY_DATA[i].name==PROPERTY_DATA[i].optName ) option = "true";
    ExitCode code = executeSetCommand(PROPERTY_DATA[i].name, option);
    if ( code!=OK ) return code;
    log(Log::Information, QString("%1: %2").arg(PROPERTY_DATA[i].name).arg(executeGetCommand(PROPERTY_DATA[i].name)));
  }

  // process default lists
  if ( _args->isSet("command-list") ) return list("command-list");
  for (uint i=0; PROPERTY_DATA[i].name; i++) {
    if ( PROPERTY_DATA[i].list==0 ) continue;
    if ( _args->isSet(PROPERTY_DATA[i].list) ) return list(PROPERTY_DATA[i].list);
  }

  bool interactive;
  ExitCode code = prepareRun(interactive);
  if ( code!=OK || interactive ) return code;

  // find command
  QString command = _args->getOption("command");
  code = findCommand(command);
  if ( code!=OK ) return code;

  // execute command
  code = prepareCommand(command);
  if ( code!=OK ) return code;
  return executeCommand(command);
}
