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
#include "common/cli/cli_log.h"
#include "devices/list/device_list.h"
#include "devices/base/device_group.h"
#include "common/global/about.h"
#include "coff/base/text_coff.h"
#include "coff/base/coff_archive.h"

//-----------------------------------------------------------------------------
const KCmdLineOptions OPTIONS[] = {
  KCmdLineLastOption
};

const CLI::CommandData CLI::NORMAL_COMMAND_DATA[] = {
  { "info",      NeedSource, I18N_NOOP("Return general informations.") },
  { "variables", NeedSource, I18N_NOOP("Return informations about variables (for object).") },
  { "symbols",   NeedSource, I18N_NOOP("Return informations about symbols.") },
  { "sections",  NeedSource, I18N_NOOP("Return informations about sections (for object).") },
  { "lines",     NeedSource, I18N_NOOP("Return informations about code lines (for object).") },
  { "files",     NeedSource, I18N_NOOP("Return informations about files.") },
  { 0, NoCommandProperty, 0 }
};
const CLI::CommandData CLI::INTERACTIVE_COMMAND_DATA[] = {
  { 0, NoCommandProperty, 0 }
};

const CLI::PropertyData CLI::PROPERTY_DATA[] = {
  { "device", "device <name>", "d", I18N_NOOP("Target device."), "device-list", I18N_NOOP("Return the list of supported devices.") },
  { 0, 0, 0, 0, 0, 0 }
};

const KCmdLineOptions CLI::OPTIONS[] = {
  KCmdLineLastOption
};

//-----------------------------------------------------------------------------
CLI::Main::Main()
 : MainBase(HasForce), _device(0)
{}

CLI::Main::~Main()
{}

CLI::ExitCode CLI::Main::prepareCommand(const QString &command)
{
  const CommandData *data = findCommandData(command);
  CommandProperties properties = static_cast<CommandProperties>(data->properties);
  int nbArgs = 0;
  if ( properties & NeedSource ) nbArgs++;
  if ( properties & NeedDestination ) nbArgs++;
  if ( _args->count()<nbArgs ) return errorExit(i18n("Too few arguments."), ARG_ERROR);
  if ( _args->count()>nbArgs ) return errorExit(i18n("Too many arguments."), ARG_ERROR);
  uint argIndex = 0;
  if ( properties & NeedSource ) {
    _source = PURL::Url(_args->url(argIndex));
    argIndex++;
    PURL::File file(_source, *_view);
    if ( !file.openForRead() ) return FILE_ERROR;
  }
  if ( properties & NeedDestination ) {
    _dest = PURL::Url(_args->url(argIndex));
    argIndex++;
    if ( !_force && _dest.exists() ) return errorExit(i18n("Destination file already exists."), FILE_ERROR);
  }
  if ( _device==0 && (properties & NeedDevice) ) return errorExit(i18n("Device not specified."), ARG_ERROR);
  return OK;
}

QString CLI::Main::prettyAuxSymbol(const Coff::AuxSymbol &aux)
{
  QString s = (aux.type()==Coff::AuxSymbolType::Nb_Types ? "?" : i18n(aux.type().label()));
  switch (aux.type().type()) {
    case Coff::AuxSymbolType::Direct: break;
    case Coff::AuxSymbolType::File: s += "=" + static_cast<const Coff::AuxSymbolFile &>(aux).filename(); break;
    case Coff::AuxSymbolType::Identifier: s += "=" + static_cast<const Coff::AuxSymbolIdentifier &>(aux).string(); break;
    case Coff::AuxSymbolType::Section: break;
    case Coff::AuxSymbolType::Nb_Types: break;
  }
  return s;
}

QString CLI::Main::prettySymbol(const Coff::Symbol &sym)
{
  QStringList saux;
  for (uint i=0; i<uint(sym.auxSymbols().count()); i++) saux += prettyAuxSymbol(*sym.auxSymbols()[i]);
  QString s = (sym.auxSymbols().count()!=0 ? " aux=[" + saux.join(" ") + "]" : QString::null);
  QStringList slist;
  if ( sym.sectionType()!=Coff::SymbolSectionType::Nb_Types ) slist += QString("sectionType=\"%1\"").arg(i18n(sym.sectionType().label()));
  if ( sym.symbolClass()!=Coff::SymbolClass::Nb_Types ) slist += QString("class=\"%1\"").arg(i18n(sym.symbolClass().label()));
  if ( sym.type()!=Coff::SymbolType::Nb_Types ) {
    slist += QString("type=\"%1\"").arg(i18n(sym.type().label()));
    if ( sym.derivedType()!=Coff::SymbolDerivedType::Nb_Types ) slist += QString("/\"%1\"").arg(i18n(sym.derivedType().label()));
  }
  return slist.join(" ") + s;
}

CLI::ExitCode CLI::Main::executeCommandArchive(const QString &command, Log::KeyList &keys)
{
  Coff::Archive coff(_source);
  if ( !coff.parse(*this) ) return ARG_ERROR;

  if ( command=="info" ) {
    keys = coff.information();
    return OK;
  }

  if ( command=="symbols" ) {
    keys.setTitle(i18n("Symbols:"));
    QMap<QString, Coff::Member *>::const_iterator it;
    for (it=coff.symbols().begin(); it!=coff.symbols().end(); ++it)
      keys.append(it.key(), it.data()->name());
    return OK;
  }

  if ( command=="files" ) {
    keys.setTitle(i18n("File Members:"));
    QMap<QString, Coff::Member *>::const_iterator it;
    for (it=coff.members().begin(); it!=coff.members().end(); ++it)
      keys.append(it.key(), i18n("size=%1").arg(it.data()->nbBytes()));
    return OK;
  }

  return errorExit(i18n("Command not available for COFF of type Archive."), ARG_ERROR);
}

CLI::ExitCode CLI::Main::executeCommandObject(const QString &command, Log::KeyList &keys)
{
  Coff::TextObject coff(_device, _source);
  if ( !coff.parse(*this) ) return ARG_ERROR;
  Q_ASSERT( coff.device()==0 || coff.device()->group().name()=="pic" );
  const Pic::Data *pdata = static_cast<const Pic::Data *>(coff.device());
  uint nbCharsAddress = (pdata ? pdata->nbCharsAddress() : 8);

  if ( command=="info" ) {
    keys = coff.information();
    return OK;
  }

  if ( command=="variables" ) {
    keys.setTitle(i18n("Variables:"));
    QMap<QString, Address>::const_iterator it;
    for (it=coff.variables().begin(); it!=coff.variables().end(); ++it)
      keys.append(it.key(), toHexLabelAbs(it.data()));
    return OK;
  }

  if ( command=="symbols" ) {
    keys.setTitle(i18n("Symbols:"));
    for (uint i=0; i<coff.nbSymbols(); i++) {
      if ( coff.symbol(i)==0 || coff.symbol(i)->isAuxSymbol() ) continue;
      const Coff::Symbol &sym = static_cast<const Coff::Symbol &>(*coff.symbol(i));
      keys.append(sym.name(), prettySymbol(sym));
    }
    return OK;
  }

  if ( command=="sections" ) {
    keys.setTitle(i18n("Sections:"));
    for (uint i=0; i<coff.nbSections(); i++) {
      const Coff::Section *s = coff.Coff::Object::section(i);
      keys.append(s->name(), i18n("type=\"%1\" address=%2 size=%3 flags=%4")
        .arg(s->type()==Coff::SectionType::Nb_Types ? "?" : i18n(s->type().label()))
        .arg(toHexLabel(s->address(), nbCharsAddress)).arg(toHexLabel(s->size(), nbCharsAddress)).arg(toHexLabel(s->flags(), 8)));
    }
    return OK;
  }

  if ( command=="lines" ) {
    keys.setTitle(i18n("Source Lines:"));
    keys.append(i18n(" Filename:Line"), i18n("Address"));
    for (uint i=0; i<coff.nbSections(); i++) {
      const Coff::Section *s = coff.Coff::Object::section(i);
      bool first = true;
      for (uint k=0; k<uint(s->lines().count()); k++) {
        if (first) {
          first = false;
          keys.append(i18n("section \"%1\":").arg(s->name()), QString::null);
        }
        const Coff::CodeLine *cl = s->lines()[k];
        QString key = cl->filename() + ":"  + QString::number(cl->line());
        if ( !cl->address().isValid() ) {
          const Coff::Symbol &sym = *cl->symbol();
          keys.append(key, i18n("symbol \"%1\"").arg(sym.name()) + prettySymbol(sym));
        } else keys.append(key, toHexLabel(cl->address(), nbCharsAddress));
      }
    }
    return OK;
  }

  if ( command=="files" ) {
    keys.setTitle(i18n("Files:"));
    QStringList::const_iterator it;
    for (it=coff.filenames().begin(); it!=coff.filenames().end(); ++it)
      keys.append(*it, QString::null);
    return OK;
  }

  return errorExit(i18n("Command not available for COFF of type Object."), ARG_ERROR);
}

CLI::ExitCode CLI::Main::executeCommand(const QString &command)
{
  CoffType type = Coff::identify(_source, *this);
  if ( type==CoffType::Nb_Types ) return ARG_ERROR;
  Log::KeyList keys;
  if ( command=="info" ) keys.append(i18n("COFF type:"), i18n(type.label()));
  ExitCode code = ARG_ERROR;
  switch (type.type()) {
    case CoffType::Archive: code = executeCommandArchive(command, keys); break;
    case CoffType::Object: code = executeCommandObject(command, keys); break;
    case CoffType::Nb_Types: Q_ASSERT(false); break;
  }
  if ( code==OK ) keys.display(*_view);
  return code;
}

CLI::ExitCode CLI::Main::prepareRun(bool &interactive)
{
  interactive = false;
  return OK;
}

CLI::ExitCode CLI::Main::executeSetCommand(const QString &property, const QString &value)
{
  if ( property=="device" || property=="processor" ) {
    if ( value.isEmpty() ) {
      _device = 0;
      return OK;
    }
    QString s = value.upper();
    _device = Device::lister().data(s);
    if ( _device==0 ) return errorExit(i18n("Unknown device \"%1\".").arg(s), ARG_ERROR);
    return OK;
  }
  return errorExit(i18n("Unknown property \"%1\".").arg(property), ARG_ERROR);
}

QString CLI::Main::executeGetCommand(const QString &property)
{
  if ( property=="device" || property=="processor" ) {
    if ( _device==0 ) return i18n("<not set>");
    return _device->name();
  }
  log(Log::LineType::SoftError, i18n("Unknown property \"%1\".").arg(property));
  return QString::null;
}

CLI::ExitCode CLI::Main::list(const QString &command)
{
  if ( MainBase::list(command)==OK ) return OK;
  if ( command=="device-list" ) return deviceList();
  Q_ASSERT(false);
  return OK;
}

CLI::ExitCode CLI::Main::deviceList()
{
  QValueVector<QString> devices;
  log(Log::LineType::Normal, i18n("Supported devices:"));
  devices = Device::lister().supportedDevices();
  qHeapSort(devices);
  QString s;
  for (uint i=0; i<uint(devices.count()); i++) s += " " + devices[i];
  log(Log::LineType::Normal, s + "\n");
  return OK;
}

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  CLI::Main main;
  Piklab::AboutData *about = new Piklab::AboutData("piklab-coff", I18N_NOOP("Piklab COFF Utility"), I18N_NOOP("Command-line utility to view COFF files."));
  CLI::OptionList list = main.optionList(I18N_NOOP("COFF filename."));
  Piklab::init(about, argc, argv, false, list.ptr());
  return main.doRun();
}
