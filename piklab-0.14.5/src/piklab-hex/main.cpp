/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "main.h"

#include "common/global/pfile.h"
#include "libcli/cli_log.h"
#include "devices/base/generic_device.h"
#include "common/global/about.h"

//-----------------------------------------------------------------------------
const KCmdLineOptions OPTIONS[] = {
  KCmdLineLastOption
};

const CLI::CommandData CLI::NORMAL_COMMAND_DATA[] = {
  { "check",   NeedSource1 | NeedCorrectInput, I18N_NOOP("Check hex file for correctness.") },
  { "info",    NeedSource1 | NeedCorrectInput, I18N_NOOP("Return information about hex file.") },
  { "fix",     NeedSource1 | NeedDestination,  I18N_NOOP("Clean hex file and fix errors (wrong CRC, truncated line, truncated file).") },
  { "compare", NeedSource1 | NeedSource2 | NeedCorrectInput, I18N_NOOP("Compare two hex files.") },
  { 0, NoCommandProperty, 0 }
};
const CLI::CommandData CLI::INTERACTIVE_COMMAND_DATA[] = {
  { 0, NoCommandProperty, 0 }
};

//-----------------------------------------------------------------------------
CLI::OptionList CLI::Main::optionList() const
{
  OptionList list(HasForce, OPTIONS);
  KCmdLineOptions opt;
  opt.name = "+[file]";
  opt.description = I18N_NOOP("Hex filename(s).");
  opt.def = 0;
  list.append(opt);
  return list;
}

CLI::ExitCode CLI::Main::prepareCommand(const QString &command)
{
  const CommandData *data = findCommandData(command);
  CommandProperties properties = static_cast<CommandProperties>(data->properties);
  int nbArgs = 0;
  if ( properties & NeedSource1 ) nbArgs++;
  if ( properties & NeedSource2 ) nbArgs++;
  if ( properties & NeedDestination ) nbArgs++;
  if ( _args->count()<nbArgs ) return errorExit(i18n("Too few arguments."), ARG_ERROR);
  if ( _args->count()>nbArgs ) return errorExit(i18n("Too many arguments."), ARG_ERROR);
  uint argIndex = 0;
  if ( properties & NeedSource1 ) {
    PURL::Url url = PURL::Url(_args->url(argIndex));
    argIndex++;
    PURL::File file(url, _view);
    if ( !file.openForRead() ) return FILE_ERROR;
    _errors = _source1.load(file.stream(), _format);
    if ( (properties & NeedCorrectInput) && !_errors.isEmpty() ) {
      QString s = (properties & NeedSource2 ? i18n("First hex file: ") : QString::null);
      for (uint i=0; i<uint(_errors.count()); i++) log(Log::Error, s + _errors[i].message());
      return EXEC_ERROR;
    }
  }
  if ( properties & NeedSource2 ) {
    PURL::Url url = PURL::Url(_args->url(argIndex));
    argIndex++;
    PURL::File file(url, _view);
    if ( !file.openForRead() ) return FILE_ERROR;
    _errors = _source2.load(file.stream(), _format);
    if ( (properties & NeedCorrectInput) && !_errors.isEmpty() ) {
      QString s = (properties & NeedSource1 ? i18n("Second hex file: ") : QString::null);
      for (uint i=0; i<uint(_errors.count()); i++) log(Log::Error, s + _errors[i].message());
      return EXEC_ERROR;
    }
  }
  if ( properties & NeedDestination ) {
    _dest = PURL::Url(_args->url(argIndex));
    argIndex++;
    if ( !_force && _dest.probablyExists() ) return errorExit(i18n("Destination file already exists."), FILE_ERROR);
  }
  return OK;
}

CLI::ExitCode CLI::Main::executeCommand(const QString &command)
{
  if ( command=="check" ) return okExit(i18n("Hex file is valid."));
  if ( command=="info" ) {
    KeyList  keys;
    keys.append(i18n("Format:"), HexBuffer::FORMATS[_format]);
    uint nbWords = 0, start = 0, end = 0;
    HexBuffer::const_iterator it;
    for (it=_source1.begin(); it!=_source1.end(); ++it) {
      if ( it.data()==Device::INVALID ) continue;
      nbWords++;
      if ( nbWords==1 ) {
        start = it.key();
        end = it.key();
      } else {
        start = qMin(start, it.key());
        end = qMax(end, it.key());
      }
    }
    keys.append(i18n("No. of Words:"), toHexLabelAbs(nbWords));
    if ( nbWords!=0 ) {
      uint nbc = nbChars(Number::Hex, end);
      keys.append(i18n("Start:"), toHexLabel(start, nbc));
      keys.append(i18n("End:"), toHexLabel(end, nbc));
    }
    keys.display();
    return okExit(i18n("Hex file is valid."));
  }
  if ( command=="fix" ) {
    for (uint i=0; i<uint(_errors.count()); i++)
      if ( _errors[i].type==HexBuffer::UnrecognizedFormat ) return errorExit(i18n("Hex file cannot be fixed because the format was not recognized or is inconsistent."), EXEC_ERROR);
    if ( _format==HexBuffer::Nb_Formats ) _format = HexBuffer::IHX32;
    PURL::File dest(_dest, _view);
    if ( !dest.openForWrite() ) return FILE_ERROR;
    _source1.savePartial(dest.stream(), _format);
    _source1.saveEnd(dest.stream());
    if ( !dest.close() ) return FILE_ERROR;
    if ( _errors.isEmpty() ) return okExit(i18n("Hex file cleaned."));
    return okExit(i18n("Hex file cleaned and fixed."));
  }
  if ( command=="compare" ) {
    bool firstInSecond = true, secondInFirst = true;
    HexBuffer::const_iterator it;
    for (it=_source1.begin(); it!=_source1.end(); ++it) {
      if ( (it.data() & 0xFFFF)==(_source2[it.key()] & 0xFFFF) ) continue;
      firstInSecond = false;
    }
    for (it=_source2.begin(); it!=_source2.end(); ++it) {
      if ( (it.data() & 0xFFFF)==(_source1[it.key()] & 0xFFFF) ) continue;
      secondInFirst = false;
    }
    if ( firstInSecond && secondInFirst ) return okExit(i18n("The two hex files have the same content."));
    if (firstInSecond) log(Log::Information, i18n("The first hex file is a subset of the second one."));
    if (secondInFirst) log(Log::Information, i18n("The second hex file is a subset of the first one."));
    return errorExit(i18n("The two hex files are different at address %1.").arg(toHexLabel(it.key(), 8)), EXEC_ERROR);
  }
  Q_ASSERT(false);
  return ARG_ERROR;
}

CLI::ExitCode CLI::Main::run()
{
  if ( _args->isSet("command-list") ) return commandList();
  QString command = _args->getOption("command");
  ExitCode code = findCommand(command);
  if ( code!=OK ) return code;

  code = prepareCommand(command);
  if ( code!=OK ) return code;
  return executeCommand(command);
}

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  CLI::Main main;
  Piklab::AboutData *about = new Piklab::AboutData("piklab-hex", I18N_NOOP("Piklab Hex Utility"), I18N_NOOP("Command-line utility to manipulate hex files."));
  CLI::OptionList list = main.optionList();
  Piklab::init(about, argc, argv, false, list.ptr());
  main.init(CLI::HasForce);
  return main.run();
}
