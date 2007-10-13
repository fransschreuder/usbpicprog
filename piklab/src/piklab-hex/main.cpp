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
#include "devices/pic/base/pic.h"
#include "devices/pic/pic/pic_memory.h"
#include "devices/base/device_group.h"
#include "common/global/about.h"

//-----------------------------------------------------------------------------
const KCmdLineOptions OPTIONS[] = {
  KCmdLineLastOption
};

const CLI::CommandData CLI::NORMAL_COMMAND_DATA[] = {
  { "check",    NeedSource1 | NeedCorrectInput, I18N_NOOP("Check hex file for correctness (if a device is specified, check if hex file is compatible with it).") },
  { "info",     NeedSource1 | NeedCorrectInput, I18N_NOOP("Return information about hex file.") },
  { "fix",      NeedSource1 | NeedDestination,  I18N_NOOP("Clean hex file and fix errors (wrong CRC, truncated line, truncated file).") },
  { "compare",  NeedSource1 | NeedSource2 | NeedCorrectInput, I18N_NOOP("Compare two hex files.") },
  { "checksum", NeedSource1 | NeedCorrectInput | NeedDevice, I18N_NOOP("Return checksum.") },
  { "create",   NeedDestination | NeedDevice, I18N_NOOP("Create an hex file for the specified device.") },
  { 0, NoCommandProperty, 0 }
};
const CLI::CommandData CLI::INTERACTIVE_COMMAND_DATA[] = {
  { 0, NoCommandProperty, 0 }
};

const CLI::PropertyData CLI::PROPERTY_DATA[] = {
  { "device", "device <name>", "d", I18N_NOOP("Target device."), "device-list", I18N_NOOP("Return the list of supported devices.") },
  { "fill",   "fill <value>",  0,   I18N_NOOP("Fill option."),   "fill-list",   I18N_NOOP("Return the list of supported fill options.") },
  { 0, 0, 0, 0, 0, 0 }
};

const KCmdLineOptions CLI::OPTIONS[] = {
  KCmdLineLastOption
};

const CLI::FillOptions CLI::FILL_OPTIONS[] = {
  { "blank", I18N_NOOP("Fill with blank values (default).") },
  { "zero",  I18N_NOOP("Fill with zeroes.") },
  { "checksum_check", I18N_NOOP("Fill for checksum verification (cf datasheets).") },
  { 0, 0 }
};

//-----------------------------------------------------------------------------
CLI::Main::Main()
 : MainBase(HasForce), _device(0), _memory(0)
{}

CLI::Main::~Main()
{
  delete _memory;
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
    PURL::File file(url, *_view);
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
    PURL::File file(url, *_view);
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
    if ( !_force && _dest.exists() ) return errorExit(i18n("Destination file already exists."), FILE_ERROR);
  }
  if ( _device==0 && (properties & NeedDevice) ) return errorExit(i18n("Device not specified."), ARG_ERROR);
  return OK;
}

CLI::ExitCode CLI::Main::executeCommand(const QString &command)
{
  if (_device) {
    delete _memory;
    _memory = _device->group().createMemory(*_device);
  }
  if ( command=="check" ) {
    if ( _device==0 ) return okExit(i18n("Hex file is valid."));
    QStringList warnings;
    Device::Memory::WarningTypes wtypes = _memory->fromHexBuffer(_source1, warnings);
    if ( wtypes==Device::Memory::NoWarning ) return okExit(i18n("Hex file is compatible with device \"%1\".").arg(_device->name()));
    return errorExit(warnings.join("\n"), EXEC_ERROR);
  }
  if ( command=="info" ) {
    KeyList  keys;
    keys.append(i18n("Format:"), HexBuffer::FORMATS[_format]);
    uint nbWords = 0, start = 0, end = 0;
    HexBuffer::const_iterator it;
    for (it=_source1.begin(); it!=_source1.end(); ++it) {
      if ( !it.data().isInitialized() ) continue;
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
    PURL::File dest(_dest, *_view);
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
      if ( it.data().maskWith(0xFFFF)==_source2[it.key()].maskWith(0xFFFF) ) continue;
      firstInSecond = false;
    }
    for (it=_source2.begin(); it!=_source2.end(); ++it) {
      if ( it.data().maskWith(0xFFFF)==_source1[it.key()].maskWith(0xFFFF) ) continue;
      secondInFirst = false;
    }
    if ( firstInSecond && secondInFirst ) return okExit(i18n("The two hex files have the same content."));
    if (firstInSecond) log(Log::Information, i18n("The first hex file is a subset of the second one."));
    if (secondInFirst) log(Log::Information, i18n("The second hex file is a subset of the first one."));
    return errorExit(i18n("The two hex files are different at address %1.").arg(toHexLabel(it.key(), 8)), EXEC_ERROR);
  }
  if ( command=="checksum" ) {
    QStringList warnings;
    Device::Memory::WarningTypes wtypes = _memory->fromHexBuffer(_source1, warnings);
    for (uint i=0; i<uint(warnings.count()); i++) log(Log::Warning, warnings[i]);
    log(Log::Warning, i18n("Checksum computation is experimental and is not always correct!")); // #### REMOVE ME
    BitValue cs = _memory->checksum();
    log(Log::Data, i18n("Checksum: %1").arg(toHexLabel(cs, 4)));
    if ( _device->group().name()=="pic" ) {
      BitValue ucs = static_cast<Pic::Memory *>(_memory)->unprotectedChecksum();
      if ( ucs!=cs ) log(Log::Information, i18n("Unprotected checksum: %1").arg(toHexLabel(ucs, 4)));
    }
    return OK;
  }
  if ( command=="create" ) {
    if ( _fill.isEmpty() || _fill=="blank" ) ; // default
    else if ( _fill=="zero" ) _memory->fill(0x0);
    else if ( _fill=="checksum_check" ) {
      if ( _device->group().name()=="pic" ) static_cast<Pic::Memory *>(_memory)->checksumCheckFill();
    } else {
      bool ok;
      uint value = fromAnyLabel(_fill, &ok);
      Q_ASSERT(ok);
      _memory->fill(value);
    }
    PURL::File dest(_dest, *_view);
    if ( !dest.openForWrite() ) return FILE_ERROR;
    _memory->save(dest.stream(), HexBuffer::IHX32);
    if ( !dest.close() ) return FILE_ERROR;
    return okExit(i18n("File created."));
  }
  Q_ASSERT(false);
  return ARG_ERROR;
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
  if ( property=="fill" ) {
    _fill = value;
    for (uint i=0; FILL_OPTIONS[i].name; i++)
      if ( value==FILL_OPTIONS[i].name ) return OK;
    bool ok;
    (void)fromAnyLabel(value, &ok);
    if ( !ok ) return errorExit(i18n("Number format not recognized."), ARG_ERROR);
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
  if ( property=="fill" ) {
    if ( _fill.isEmpty() ) return i18n("<not set>");
    return _fill;
  }
  log(Log::SoftError, i18n("Unknown property \"%1\".").arg(property));
  return QString::null;
}

CLI::ExitCode CLI::Main::list(const QString &command)
{
  if ( MainBase::list(command)==OK ) return OK;
  if ( command=="device-list" ) return deviceList();
  if ( command=="fill-list" ) return fillOptionList();
  Q_ASSERT(false);
  return OK;
}

CLI::ExitCode CLI::Main::deviceList()
{
  QValueVector<QString> devices;
  log(Log::Data, i18n("Supported devices:"));
  devices = Device::lister().supportedDevices();
  qHeapSort(devices);
  QString s;
  for (uint i=0; i<uint(devices.count()); i++) s += " " + devices[i];
  log(Log::Data, s + "\n");
  return OK;
}

CLI::ExitCode CLI::Main::fillOptionList()
{
  log(Log::Data, i18n("Fill options:"));
  KeyList keys;
  for (uint i=0; FILL_OPTIONS[i].name; i++)
    keys.append(FILL_OPTIONS[i].name, i18n(FILL_OPTIONS[i].description));
  keys.append(i18n("<value>"), i18n("Fill with the specified numeric value."));
  keys.display();
  return OK;
}

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  CLI::Main main;
  Piklab::AboutData *about = new Piklab::AboutData("piklab-hex", I18N_NOOP("Piklab Hex Utility"), I18N_NOOP("Command-line utility to manipulate hex files."));
  CLI::OptionList list = main.optionList(I18N_NOOP("Hex filename(s)."));
  Piklab::init(about, argc, argv, false, list.ptr());
  return main.doRun();
}
