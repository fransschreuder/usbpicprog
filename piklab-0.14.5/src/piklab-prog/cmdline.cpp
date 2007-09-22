/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "cmdline.h"

#if defined(HAVE_READLINE)
#  include <readline/readline.h>
#  include <readline/history.h>
#else
#  include <stdio.h>
#endif
#include <signal.h>
#include <qtimer.h>

#include "devices/list/device_list.h"
#include "devices/base/device_group.h"
#include "common/global/about.h"
#include "progs/base/prog_config.h"
#include "devices/pic/pic/pic_memory.h"
#include "devices/pic/pic/pic_prog.h"
#include "progs/list/prog_list.h"
#include "libcli/cli_log.h"
#include "cli_prog_manager.h"
#include "cli_debug_manager.h"

//-----------------------------------------------------------------------------
const CLI::CommandData CLI::NORMAL_COMMAND_DATA[] = {
  { "connect",         NeedProgrammer | NeedDevice,
    I18N_NOOP("Connect programmer.") },
  { "run",             NeedProgrammer | NeedDevice,
    I18N_NOOP("Run device (release reset).") },
  { "stop",            NeedProgrammer | NeedDevice,
    I18N_NOOP("Stop device (hold reset).") },
  { "program",         NeedProgrammer | InputHex | NeedDevice,
    I18N_NOOP("Program device memory: \"program <hexfilename>\".") },
  { "verify",          NeedProgrammer | InputHex | NeedDevice,
    I18N_NOOP("Verify device memory: \"verify <hexfilename>\".") },
  { "read",            NeedProgrammer | OutputHex | NeedDevice,
    I18N_NOOP("Read device memory: \"read <hexfilename>\".") },
  { "erase",           NeedProgrammer | NeedDevice,
    I18N_NOOP("Erase device memory.") },
  { "blank_check",     NeedProgrammer | NeedDevice,
    I18N_NOOP("Blank check device memory.") },
  { "checksum",        InputHex | NeedDevice,
    I18N_NOOP("Return checksum: \"checksum <hexfilename>\".") },
  { "upload_firmware", NeedProgrammer | InputHex,
    I18N_NOOP("Upload firmware to programmer: \"upload_firmware <hexfilename>\".") },
  { 0, NoCommandProperty, 0 }
};

const KCmdLineOptions OPTIONS[] = {
  { "r",              0, 0 },
  { "range <name>",   I18N_NOOP("Memory range to operate on."), 0 },
  { "range-list",     I18N_NOOP("Return the list of memory ranges."), 0 },
  KCmdLineLastOption
};

//-----------------------------------------------------------------------------
const Programmer::Group *CLI::_progGroup = 0;
const Device::Data *CLI::_device = 0;
HexBuffer::Format CLI::_format = HexBuffer::IHX32;
QString CLI::_port, CLI::_targetSelfPowered;
PURL::Directory CLI::_firmwareDir;
PURL::Url CLI::_hexUrl, CLI::_coffUrl;
Device::Memory *CLI::_memory = 0;
CLI::Interactive *CLI::_interactive = 0;

//-----------------------------------------------------------------------------
CLI::ExitCode CLI::Main::formatList()
{
  log(Log::Data, i18n("Supported hex file formats:"));
  for (uint i=0; i<HexBuffer::Nb_Formats; i++)
    log(Log::Data, QString(" ") + HexBuffer::FORMATS[i]);
  return OK;
}

CLI::ExitCode CLI::Main::programmerList()
{
  log(Log::Data, i18n("Supported programmers:"));
  for (uint i=0; i<Programmer::lister().nbGroups(); i++)
    log(Log::Data, " " + QString(Programmer::lister().group(i).name()));
  return OK;
}

CLI::ExitCode CLI::Main::deviceList()
{
  QValueVector<QString> devices;
  if ( _progGroup==0 ) {
    log(Log::Data, i18n("Supported devices:"));
    devices = Programmer::lister().supportedDevices();
  } else {
    log(Log::Data, i18n("Supported devices for \"%1\":").arg(_progGroup->label()));
    devices = _progGroup->supportedDevices();
  }
  qHeapSort(devices);
  QString s;
  for (uint i=0; i<uint(devices.count()); i++) s += " " + devices[i];
  log(Log::Data, s + "\n");
  return OK;
}

CLI::ExitCode CLI::Main::portList()
{
  if (_progGroup) log(Log::Data, i18n("Detected ports supported by \"%1\":").arg(_progGroup->label()));
  else log(Log::Data, i18n("Detected ports:"));
  for (uint i=0; i<Port::Nb_Types; i++) {
    if ( _progGroup && !_progGroup->isPortSupported(Port::Type(i)) ) continue;
    QString s = "- " + i18n(Port::DATA[i].label) + ":";
    if ( !Port::isAvailable(Port::Type(i)) ) {
      log(Log::Data, s + i18n(" support disabled"));
      continue;
    }
    QStringList list = Port::probedDeviceList(Port::Type(i));
    if ( list.count()==0 ) log(Log::Data, s + i18n(" no port detected"));
    else {
      log(Log::Data, s);
      for (uint k=0; k<uint(list.count()); k++) log(Log::Data, "   " + list[k]);
    }
  }
  return OK;
}

CLI::ExitCode CLI::Main::propertyList()
{
  uint nb = 0;
  for (uint i=0; PROPERTY_DATA[i].name; i++) {
    nb = QMAX(nb, uint(strlen(PROPERTY_DATA[i].name)));
  }
  for (uint i=0; PROPERTY_DATA[i].name; i++) {
    QString s = QString(PROPERTY_DATA[i].name);
    log(Log::Data, " " + s.leftJustify(nb+2) + i18n(PROPERTY_DATA[i].help));
  }
  return OK;
}

CLI::ExitCode CLI::Main::rangeList()
{
  log(Log::Data, i18n("Memory ranges for PIC/dsPIC devices:"));
  for (uint i=0; i<Pic::Nb_MemoryRangeTypes; i++)
    log(Log::Data, QString(" %1").arg(Pic::MEMORY_RANGE_TYPE_DATA[i].name));
  return OK;
}

CLI::ExitCode CLI::Main::prepareCommand(const QString &command)
{
  const CommandData *data = findCommandData(command);
  CommandProperties properties = static_cast<CommandProperties>(data->properties);
  if ( _device==0 && (properties & NeedDevice) ) return errorExit(i18n("Device not specified."), ARG_ERROR);
  if ( _progGroup==0 && (properties & NeedProgrammer) ) return errorExit(i18n("Programmer not specified."), ARG_ERROR);

  if ( (properties & InputHex) || (properties & OutputHex) ) {
    if ( _hexUrl.isEmpty() ) return errorExit(i18n("Hex filename not specified."), ARG_ERROR);
    //if ( !_filename.isLocalFile() ) return errorExit(i18n("Only local files are supported."), ARG_ERROR);
    PURL::File file(_hexUrl, _view);
    delete _memory;
    _memory = 0;
    if ( properties & NeedDevice ) _memory = _device->group().createMemory(*_device);
    if ( properties & InputHex ) {
      if (_memory) {
        if ( !file.openForRead() ) return FILE_ERROR;
        QStringList errors, warnings;
        Device::Memory::WarningTypes warningTypes;
        if ( !_memory->load(file.stream(), errors, warningTypes, warnings) )
          return errorExit(i18n("Could not load hex file: %1.").arg(errors[0]), FILE_ERROR);
        if ( warningTypes!=Device::Memory::NoWarning )
          log(Log::Warning, i18n("Hex file seems incompatible with device: %1").arg(warnings.join(" ")));
      }
    } else if ( properties & OutputHex ) {
      if ( !_force && _hexUrl.probablyExists() ) return errorExit(i18n("Output hex filename already exists."), FILE_ERROR);
    }
  }

  return OK;
}

CLI::OptionList CLI::Main::optionList() const
{
  OptionList list(HasForce | HasInteractiveMode, OPTIONS);
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
  opt.name = "+[file]";
  opt.description = I18N_NOOP("Hex filename for programming.");
  opt.def = 0;
  list.append(opt);
  return list;
}

CLI::Main::Main()
  : _range(0)
{
  _range = new Device::MemoryRange;
  Programmer::manager = new Programmer::CliManager(this);
  Programmer::manager->setView(_view);
  Debugger::manager = new Debugger::CliManager;
}

CLI::Main::~Main()
{
  delete _range;
}

CLI::ExitCode CLI::Main::list(const QString &command)
{
  if ( command=="command-list" ) return commandList();
  if ( command=="format-list" ) return formatList();
  if ( command=="programmer-list" ) return programmerList();
  if ( command=="port-list" ) return portList();
  if ( command=="device-list" ) return deviceList();
  if ( command=="property-list" ) return propertyList();
  if ( command=="range-list" ) return rangeList();
  Q_ASSERT(false);
  return OK;
}

CLI::ExitCode CLI::Main::run()
{
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
  // argument
  if ( _args->count()>1 ) return errorExit(i18n("Too many arguments."), ARG_ERROR);
  if ( _args->count()==1 ) {
    PURL::Url url(_args->url(0));
    ExitCode code = OK;
    if ( url.fileType()==PURL::Hex ) code = executeSetCommand("hex", url.filepath(false));
    else if ( url.fileType()==PURL::Coff ) code = executeSetCommand("coff", url.filepath(false));
    else return errorExit(i18n("Argument file type not recognized."), ARG_ERROR);
    if ( code!=OK ) return code;
  }

  if ( _args->isSet("cli") ) {
    _interactive = new Interactive(this);
    log(Log::Data, i18n("Interactive mode: type help for help"));
    log(Log::Data, QString::null);
    return ExitCode(qApp->exec());
  }
  if ( _args->isSet("command-list") ) return list("command-list");
  if ( _args->isSet("range-list") ) return list("range-list");
  for (uint i=0; PROPERTY_DATA[i].name; i++) {
    if ( PROPERTY_DATA[i].list==0 ) continue;
    if ( _args->isSet(PROPERTY_DATA[i].list) ) return list(PROPERTY_DATA[i].list);
  }

  // find command
  QString command = _args->getOption("command");
  ExitCode code = findCommand(command);
  if ( code!=OK ) return code;

  // range
  code = extractRange(_args->getOption("range"));
  if ( code!=OK ) return code;

  // execute command
  code = prepareCommand(command);
  if ( code!=OK ) return code;
  return executeCommand(command);
}

CLI::ExitCode CLI::Main::extractRange(const QString &range)
{
  delete _range;
  _range = 0;
  if ( !range.isEmpty() ) {
    if ( _device==0 ) return errorExit(i18n("Cannot specify range without specifying device."), ARG_ERROR);
    if ( _device->group().name()=="pic" ) {
      for (uint i=0; i<Pic::Nb_MemoryRangeTypes; i++) {
        if ( range!=Pic::MEMORY_RANGE_TYPE_DATA[i].name ) continue;
        if ( !static_cast<const Pic::Data *>(_device)->isReadable(Pic::MemoryRangeType(i)) ) return errorExit(i18n("Memory range not present on this device."), ARG_ERROR);
        _range = new Pic::MemoryRange(Pic::MemoryRangeType(i));
        break;
      }
      if ( _range==0 ) return errorExit(i18n("Memory range not recognized."), ARG_ERROR);
    } else return errorExit(i18n("Memory ranges are not supported for the specified device."), ARG_ERROR);
  } else _range = new Device::MemoryRange;
  return OK;
}

CLI::ExitCode CLI::Main::executeCommand(const QString &command)
{
  Programmer::Base *programmer = Programmer::manager->programmer();
  if ( command=="connect" ) return (Programmer::manager->connectDevice() ? OK : EXEC_ERROR);
  if ( command=="disconnect" ) {
    if ( programmer==0 || programmer->state()==Programmer::NotConnected )
      return okExit(i18n("Programmer is already disconnected."));
    return (Programmer::manager->disconnectDevice() ? OK : EXEC_ERROR);
  }
  if ( command=="run" ) {
    if ( programmer && programmer->state()==Programmer::Running ) return okExit(i18n("Programmer is already running."));
    return (Programmer::manager->run() ? OK : EXEC_ERROR);
  }
  if ( command=="stop" ) {
    if ( programmer && programmer->state()!=Programmer::Running ) return okExit(i18n("Programmer is already stopped."));
    return (Programmer::manager->halt() ? OK : EXEC_ERROR);
  }
  if ( command=="step" ) {
    if ( !_progGroup->isDebugger() ) return errorExit(i18n("Debugging is not supported for specified programmer."), NOT_SUPPORTED_ERROR);
    if ( programmer && programmer->state()==Programmer::Running ) return (Programmer::manager->halt() ? OK : EXEC_ERROR);
    return (Programmer::manager->step() ? OK : EXEC_ERROR);
  }
  if ( command=="start" ) {
    if ( !_progGroup->isDebugger() ) return errorExit(i18n("Debugging is not supported for specified programmer."), NOT_SUPPORTED_ERROR);
    return (Programmer::manager->restart() ? OK : EXEC_ERROR);
  }
  if ( command=="program" ) {
    if ( _progGroup->isSoftware() ) return errorExit(i18n("Reading device memory not supported for specified programmer."), NOT_SUPPORTED_ERROR);
    return (Programmer::manager->program(*_memory, *_range) ? OK : EXEC_ERROR);
  }
  if ( command=="verify" ) {
    if ( _progGroup->isSoftware() )
      return errorExit(i18n("Reading device memory not supported for specified programmer."), NOT_SUPPORTED_ERROR);
    return (Programmer::manager->verify(*_memory, *_range) ? OK : EXEC_ERROR);
  }
  if ( command=="read" ) {
    if ( _progGroup->isSoftware() )
      return errorExit(i18n("Reading device memory not supported for specified programmer."), NOT_SUPPORTED_ERROR);
    if ( !Programmer::manager->read(*_memory, *_range) ) return EXEC_ERROR;
    PURL::File file(_hexUrl, _view);
    if ( !file.openForWrite() ) return FILE_ERROR;
    if ( !_memory->save(file.stream(), _format) )
      return errorExit(i18n("Error while writing file \"%1\" (%2).").arg(_hexUrl.pretty()).arg(file.errorString()), FILE_ERROR);
    return OK;
  }
  if ( command=="erase" ) {
    if ( _progGroup->isSoftware() )
      return errorExit(i18n("Erasing device memory not supported for specified programmer."), NOT_SUPPORTED_ERROR);
    return (Programmer::manager->erase(*_range) ? OK : EXEC_ERROR);
  }
  if ( command=="blank_check" ) {
    if ( _progGroup->isSoftware() )
      return errorExit(i18n("Blank-checking device memory not supported for specified programmer."), NOT_SUPPORTED_ERROR);
    return (Programmer::manager->blankCheck(*_range) ? OK : EXEC_ERROR);
  }
  if ( command=="checksum" ) {
    uint cs = _memory->checksum();
    log(Log::Information, i18n("Checksum: %1").arg(toHexLabel(cs, 4)));
    if ( _device->group().name()=="pic" ) {
      uint ucs = static_cast<Pic::Memory *>(_memory)->unprotectedChecksum();
      if ( ucs!=cs ) log(Log::Information, i18n("Unprotected checksum: %1").arg(toHexLabel(ucs, 4)));
    }
    return OK;
  }
  if ( command=="upload_firmware" ) {
    if ( !(_progGroup->properties() & ::Programmer::CanUploadFirmware) )
      return errorExit(i18n("Uploading firmware is not supported for the specified programmer."), NOT_SUPPORTED_ERROR);
    if ( Programmer::manager->programmer()==0 ) Programmer::manager->createProgrammer(0); // no device specified
    return (Programmer::manager->programmer()->uploadFirmware(_hexUrl.filepath(false)) ? OK : EXEC_ERROR);
  }
  Q_ASSERT(false);
  return EXEC_ERROR;
}

CLI::ExitCode CLI::Main::checkProgrammer()
{
  if ( _progGroup==0 ) return OK;
  if ( _progGroup->isSoftware() && _progGroup->supportedDevices().isEmpty() )
    return errorExit(i18n("Please check installation of selected software debugger."), NOT_SUPPORTED_ERROR);
  if ( _device && !_progGroup->isSupported(_device->name()) )
    return errorExit(i18n("The selected device \"%1\" is not supported by the specified programmer.").arg(_device->name()), NOT_SUPPORTED_ERROR);
  return OK;
}

CLI::ExitCode CLI::Main::executeSetCommand(const QString &property, const QString &value)
{
  if ( property=="programmer" ) {
    _progGroup = 0;
    if ( value.isEmpty() ) return OK;
    QString s = value.lower();
    for (uint i=0; i<Programmer::lister().nbGroups(); i++) {
      const Programmer::Group &group = Programmer::lister().group(i);
      if ( s==group.name() ) {
        _progGroup = &group;
        return checkProgrammer();
      }
    }
    return errorExit(i18n("Unknown programmer \"%1\".").arg(s), ARG_ERROR);
  }
  if ( property=="device" || property=="processor" ) {
    if ( value.isEmpty() ) {
      _device = 0;
      return OK;
    }
    QString s = value.upper();
    _device = Device::lister().data(s);
    Debugger::manager->updateDevice();
    if ( _device==0 ) return errorExit(i18n("Unknown device \"%1\".").arg(s), ARG_ERROR);
    Debugger::manager->init();
    return checkProgrammer();
  }
  if ( property=="format" ) {
    if ( value.isEmpty() ) {
      _format = HexBuffer::IHX32;
      return OK;
    }
    QString s = value.lower();
    for (uint i=0; i<HexBuffer::Nb_Formats; i++)
      if ( s==HexBuffer::FORMATS[i] ) {
        _format = HexBuffer::Format(i);
        return OK;
      }
    return errorExit(i18n("Unknown hex file format \"%1\".").arg(s), ARG_ERROR);
  }
  if ( property=="port" ) { _port = value; return OK; }
  if ( property=="firmware-dir" ) { _firmwareDir = value; return OK; }
  if ( property=="target-self-powered" ) { _targetSelfPowered = value.lower(); return OK; }
  if ( property=="hex" ) {
    PURL::Url url(value);
    if ( url.isRelative() ) _hexUrl = PURL::Url(runDirectory(), value);
    else _hexUrl = url;
    return OK;
  }
  if ( property=="coff" ) {
    PURL::Url url(value);
    if ( url.isRelative() ) _coffUrl = PURL::Url(runDirectory(), value);
    else _coffUrl = url;
    if ( _device && !Debugger::manager->init() ) return ARG_ERROR;
    return OK;
  }
  return errorExit(i18n("Unknown property \"%1\"").arg(property), ARG_ERROR);
}

QString CLI::Main::executeGetCommand(const QString &property)
{
  if ( property=="programmer" ) {
    if ( _progGroup==0 ) return i18n("<not set>");
    return _progGroup->name();
  }
  if ( property=="device" || property=="processor" ) {
    if ( _device==0 ) return i18n("<not set>");
    return _device->name();
  }
  if ( property=="format" ) return HexBuffer::FORMATS[_format];
  if ( property=="port" ) {
    if ( !_port.isEmpty() ) return _port;
    if ( _progGroup==0 ) return i18n("<not set>");
    Programmer::GroupConfig config(*_progGroup);
    Port::Description pd = config.portDescription();
    QString s = Port::DATA[pd.type].key;
    if ( Port::DATA[pd.type].withDevice ) s += " (" + pd.device + ")";
    return s + i18n(" <default>");
  }
  if ( property=="firmware-dir" ) {
    if ( !_firmwareDir.isEmpty() ) return _firmwareDir.pretty();
    if ( _progGroup==0 ) return i18n("<not set>");
    Programmer::GroupConfig config(*_progGroup);
    return config.firmwareDirectory() + i18n(" <default>");
  }
  if ( property=="target-self-powered" ) {
    if ( !_targetSelfPowered.isEmpty() ) return _targetSelfPowered;
    Programmer::Config config;
    return QString(config.isTargetSelfPowered() ? "true" : "false") + i18n(" <default>");
  }
  if ( property=="hex" ) {
    if ( !_hexUrl.isEmpty() ) return _hexUrl.pretty();
    return i18n("<not set>");
  }
  if ( property=="coff" ) {
    if ( !_coffUrl.isEmpty() ) return _coffUrl.pretty();
    return i18n("<not set>");
  }
  log(Log::SoftError, i18n("Unknown property \"%1\"").arg(property));
  return QString::null;
}

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  CLI::Main main;
  Piklab::AboutData *about = new Piklab::AboutData("piklab-prog", I18N_NOOP("Piklab Programmer Utility"), I18N_NOOP("Command-line programmer/debugger."));
  CLI::OptionList list = main.optionList();
  Piklab::init(about, argc, argv, false, list.ptr());
  main.init(CLI::HasForce | CLI::HasInteractiveMode);
  return main.run();
}
