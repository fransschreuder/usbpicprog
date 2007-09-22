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
  : _options(0)
{
  init(properties);
}

CLI::OptionList::OptionList(Properties properties, const KCmdLineOptions *options)
  : _options(0)
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

const KCmdLineOptions *CLI::OptionList::ptr() const
{
  delete _options;
  _options = new KCmdLineOptions[count()+1];
  for (uint i=0; i<uint(count()); i++) {
    _options[i] = *at(i);
    Q_ASSERT( _options[i].name );
  }
  _options[count()].name = 0;
  return _options;
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

//-----------------------------------------------------------------------------
CLI::MainBase::MainBase()
  : QObject(0, "main")
{
  Q_ASSERT( _main==0 );
  _main = this;
  _view = new View;
  setView(_view);
}

void CLI::MainBase::init(Properties properties)
{
  _args = KCmdLineArgs::parsedArgs();
  if ( properties & HasInteractiveMode ) _isInteractive = _args->isSet("cli");
  if ( properties & HasForce ) _force = _args->isSet("force");
  if ( _args->isSet("debug") ) _view->setDebugLevel(Log::NormalDebug);
  if ( _args->isSet("extra-debug") ) _view->setDebugLevel(Log::ExtraDebug);
  if ( _args->isSet("max-debug") ) _view->setDebugLevel(Log::MaxDebug);
  if ( _args->isSet("lowlevel-debug") ) _view->setDebugLevel(Log::LowLevelDebug);
  if (_args->isSet("quiet")) _view->setQuiet();
}
