/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "cli_interactive.h"

#if defined(HAVE_READLINE)
#  include <readline/readline.h>
#  include <readline/history.h>
#else
#  include <stdio.h>
#endif
#include <signal.h>
#include <qtimer.h>
#include <stdlib.h>

#include "progs/base/generic_prog.h"
#include "cmdline.h"
#include "cli_prog_manager.h"
#include "common/cli/cli_log.h"
#include "devices/base/device_group.h"
#include "devices/pic/base/pic_register.h"
#include "devices/pic/prog/pic_prog.h"
#include "coff/base/text_coff.h"
#include "devices/pic/prog/pic_debug.h"

//-----------------------------------------------------------------------------
const CLI::CommandData CLI::INTERACTIVE_COMMAND_DATA[] = {
  { "property-list",   NoCommandProperty, I18N_NOOP("Display the list of available properties.") },
  { "register-list",   NoCommandProperty, I18N_NOOP("Display the list of registers.") },
  { "variable-list",   NoCommandProperty, I18N_NOOP("Display the list of variables.") },
  { "range-list",      NoCommandProperty, I18N_NOOP("Display the list of memory ranges.") },
  { "help",            NoCommandProperty, I18N_NOOP("Display help.") },
  { "quit",            NoCommandProperty, I18N_NOOP("Quit.") },
  { "set",             NoCommandProperty, I18N_NOOP("Set property value: \"set <property> <value>\" or \"<property> <value>\".") },
  { "unset",           NoCommandProperty, I18N_NOOP("Unset property value: \"unset <property>\".") },
  { "get",             NoCommandProperty, I18N_NOOP("Get property value: \"get <property>\" or \"<property>\".") },
  { "disconnect",      NeedProgrammer, I18N_NOOP("Disconnect programmer.") },
  { "start",           NeedDevice | NeedProgrammer, I18N_NOOP("Start or restart debugging session.") },
  { "step",            NeedDevice | NeedProgrammer, I18N_NOOP("Step one instruction.") },
  { "x",               NeedDevice | NeedProgrammer, I18N_NOOP("Read or set a register or variable: \"x PORTB\" or \"x W 0x1A\".") },
  { "break",           NeedDevice | NeedProgrammer, I18N_NOOP("Set a breakpoint \"break e 0x04\" or list all breakpoints \"break\".") },
  { "clear",           NeedDevice | NeedProgrammer, I18N_NOOP("Clear a breakpoint \"clear 0\", \"clear e 0x04\", or clear all breakpoints \"clear all\".") },
  { "raw-com",         NeedDevice | NeedProgrammer, I18N_NOOP("Write and read raw commands to port from given file.") },
  { 0, NoCommandProperty, 0 }
};

const CLI::PropertyData CLI::PROPERTY_DATA[] = {
  { "programmer",   "programmer <name>",  "p", I18N_NOOP("Programmer to use."), "programmer-list", I18N_NOOP("Return the list of supported programmers.") },
  { "hardware",     "hardware <name>",    "h", I18N_NOOP("Programmer hardware configuration to use (for direct programmer)."), "hardware-list", I18N_NOOP("Return the list of supported programmer hardware configurations.") },
  { "device",       "device <name>",      "d", I18N_NOOP("Target device."), "device-list", I18N_NOOP("Return the list of supported devices.") },
  { "format",       "format <name>",      "f", I18N_NOOP("Hex output file format."), "format-list", I18N_NOOP("Return the list of supported hex file formats.") },
  { "port",         "port <name>",        "t", I18N_NOOP("Programmer port (\"usb\" or device such as \"/dev/ttyS0\")"), "port-list", I18N_NOOP("Return the list of detected ports.") },
  { "firmware-dir", "firmware-dir <dir>", 0,   I18N_NOOP("Firmware directory."), 0, 0 },
  { "target-self-powered", "target-self-powered <true|false>", 0, I18N_NOOP("Set if target device is self-powered."), 0, 0 },
  { "hex", 0, 0, I18N_NOOP("Hex file to be used for programming."), 0, 0 },
  { "coff", 0, 0, I18N_NOOP("COFF file to be used for debugging."), 0, 0 },
  { "processor",    0, 0, I18N_NOOP("Same as \"device\"."), 0, 0 },
  { 0, 0, 0, 0, 0, 0 }
};

//-----------------------------------------------------------------------------
void CLI::Interactive::signalHandler(int n)
{
  if ( n!=SIGINT ) return;
  fprintf(stdout, "<CTRL C> Break\n");
  fflush(stdout);
  Programmer::Base *prog = Programmer::manager->programmer();
  if ( prog && prog->state()==Programmer::Running ) Programmer::manager->halt();
  _interactive->redisplayPrompt();
}

CLI::Interactive::Interactive(QObject *parent)
  : QObject(parent, "interactive")
{
  setView(_view);
  ::signal(SIGINT, signalHandler);
#if defined(HAVE_READLINE)
  using_history();
#else
  _stdin.open(IO_ReadOnly, stdin);
#endif
  QTimer::singleShot(0, this, SLOT(displayPrompt()));
}

void CLI::Interactive::redisplayPrompt()
{
#if defined(HAVE_READLINE)
  rl_forced_update_display();
#else
  fprintf(stdout, "> ");
  fflush(stdout);
#endif
}

void CLI::Interactive::displayPrompt()
{
#if defined(HAVE_READLINE)
  char *line = readline("> ");
  _input = QString(line);
  if ( !_input.isEmpty() ) add_history(line);
  free(line);
#else
  fprintf(stdout, "> ");
  fflush(stdout);
  char buffer[1000];
  _stdin.readLine(buffer, 1000);
  _input = QString(buffer);
#endif
  lineRead();
}

CLI::ExitCode CLI::Interactive::processLine(const QString &s)
{
  QStringList words = QStringList::split(" ", s);
  if ( words.count()==0 ) return ARG_ERROR;
  if ( words[0]=="command-list" || words[0]=="property-list" || words[0]=="range-list"
       || isPropertyList(words[0]) ) return static_cast<Main *>(_main)->list(words[0]);
  if ( words[0]=="register-list" ) return registerList();
  if ( words[0]=="variable-list" ) return variableList();
  if ( isProperty(words[0]) ) {
    if ( words.count()==1 ) words.prepend("get");
    else if ( words.count()==2 ) words.prepend("set");
    else return errorExit(i18n("This command takes no or one argument"), ARG_ERROR);
  }
  if ( findCommand(words[0])!=OK ) return ARG_ERROR;
  const CommandData *data = findCommandData(words[0]);
  if ( words[0]=="quit" ) return EXITING;
  if ( words[0]=="set" ) {
    if ( words.count()==2 ) return static_cast<Main *>(_main)->executeSetCommand(words[1], QString::null);
    if ( words.count()!=3 ) return errorExit(i18n("This command takes two arguments"), ARG_ERROR);
    return static_cast<Main *>(_main)->executeSetCommand(words[1], words[2]);
  }
  if ( words[0]=="unset" ) {
    if ( words.count()!=2 ) return errorExit(i18n("This command takes one argument"), ARG_ERROR);
    return static_cast<Main *>(_main)->executeSetCommand(words[1], QString::null);
  }
  if ( words[0]=="get" ) {
    if ( words.count()!=2 ) return errorExit(i18n("This command takes one argument"), ARG_ERROR);
    QString s = static_cast<Main *>(_main)->executeGetCommand(words[1]);
    if ( !s.isEmpty() ) log(Log::LineType::Normal, words[1] + ": " + s);
    return OK;
  }
  if ( words[0]=="help" ) return static_cast<Main *>(_main)->list("command-list");
  if ( words[0]=="x" ) {
    if ( words.count()!=2 && words.count()!=3 ) return errorExit(i18n("This command takes one or two argument"), ARG_ERROR);
    return executeRegister(words[1], words.count()==3 ? words[2] : QString::null);
  }
  if ( words[0]=="break" ) {
    if ( words.count()==3 ) {
      if ( words[1]=="e" ) {
        bool ok;
        ulong address = fromAnyLabel(words[2], &ok);
        if ( !ok ) return errorExit(i18n("Number format not recognized"), ARG_ERROR);
        PURL::Url dummy;
        Breakpoint::Data data(dummy, address);
        if ( Breakpoint::list().contains(data) ) return okExit(i18n("Breakpoint already set at %1").arg(toHexLabel(address, nbChars(NumberBase::Hex, address))));
        Breakpoint::list().append(data);
        Breakpoint::list().setAddress(data, address);
        Breakpoint::list().setState(data, Breakpoint::Active);
        return okExit(i18n("Breakpoint set at %1").arg(toHexLabel(address, nbChars(NumberBase::Hex, address))));
      }
      return errorExit(i18n("The first argument should be \"e\""), ARG_ERROR);
    }
    if ( words.count()==1 ) {
      uint nb = Breakpoint::list().count();
      if ( nb==0 ) return okExit(i18n("No breakpoint set"));
      log(Log::LineType::Normal, i18n("Breakpoints:"));
      uint nbc = 0;
      if (_device) nbc = _device->nbCharsAddress();
      else for (uint i=0; i<nb; i++) {
        Address address = Breakpoint::list().address(Breakpoint::list().data(i));
        nbc = QMAX(nbc, nbChars(NumberBase::Hex, address.toUInt()));
      }
      for (uint i=0; i<nb; i++) {
        Address address = Breakpoint::list().address(Breakpoint::list().data(i));
        log(Log::LineType::Normal, QString("  #%1: %2").arg(i).arg(toHexLabel(address, nbc)));
      }
      return OK;
    }
    if ( words.count()!=1 && words.count()!=3 ) return errorExit(i18n("This command takes no or two argument"), ARG_ERROR);
    return errorExit(i18n("Arguments not recognized"), ARG_ERROR);
  }
  if ( words[0]=="clear" ) {
    if ( words.count()!=2 ) return errorExit(i18n("This command takes one argument"), ARG_ERROR);
    if ( words[1]=="all" ) {
      Breakpoint::list().clear();
      return okExit(i18n("All breakpoints removed"));
    }
    bool ok;
    uint i = words[1].toUInt(&ok);
    if ( !ok ) return errorExit(i18n("Argument should be breakpoint index"), ARG_ERROR);
    if ( i>=Breakpoint::list().count() ) return errorExit(i18n("Breakpoint index too large"), ARG_ERROR);
    Breakpoint::Data data = Breakpoint::list().data(i);
    Address address = Breakpoint::list().address(data);
    Breakpoint::list().remove(data);
    return okExit(i18n("Breakpoint at %1 removed").arg(toHexLabelAbs(address)));
  }
  if ( words[0]=="raw-com" ) {
    if ( words.count()!=2 ) return errorExit(i18n("This command needs a commands filename"), ARG_ERROR);
  } else if ( words[0]=="program" || words[0]=="read" || words[0]=="verify" || words[0]=="erase" || words[0]=="blank_check" ) {
    uint hexi = 1;
    if ( words.count()>=2 && words[1]=="-r" ) {
      hexi = 3;
      if ( words.count()==2 ) return errorExit(i18n("You need to specify the range"), ARG_ERROR);
      ExitCode code = static_cast<Main *>(_main)->extractRange(words[2]);
      if ( code!=OK ) return code;
    }
    if ( data->properties & (InputHex|OutputHex) ) {
      if ( uint(words.count())>(hexi+1) ) return errorExit(i18n("Too many arguments"), ARG_ERROR);
      if ( uint(words.count())==(hexi+1) )_hexUrl = PURL::Url(runDirectory(), words[hexi]);
      if ( _hexUrl.isEmpty() ) return errorExit(i18n("This command needs an hex filename"), ARG_ERROR);
    } else if ( uint(words.count())>hexi ) return errorExit(i18n("Too many arguments"), ARG_ERROR);
  } else if ( words.count()!=1 ) return errorExit(i18n("This command takes no argument"), ARG_ERROR);

  ExitCode code = static_cast<Main *>(_main)->prepareCommand(words[0]);
  if ( code!=OK ) return code;
  if ( words[0]=="raw-com" ) return executeRawCommands(words[1]);
  return static_cast<Main *>(_main)->executeCommand(words[0]);
}

void CLI::Interactive::lineRead()
{
  QString s = _input.stripWhiteSpace();
  _input = QString::null;
  if ( processLine(s)==EXITING ) {
#if QT_VERSION<0x040000
    qApp->exit(OK);
#else
    QCoreApplication::exit(OK);
#endif
    return;
  }
  QTimer::singleShot(0, this, SLOT(displayPrompt()));
}

CLI::ExitCode CLI::Interactive::registerList()
{
  if ( _device==0 ) return errorExit(i18n("No device specified"), NOT_SUPPORTED_ERROR);
  if ( _device->group().name()!="pic" ) return errorExit(i18n("No register"), NOT_SUPPORTED_ERROR);
  const Pic::Data &data = static_cast<const Pic::Data &>(*_device);
  const Coff::Object *coff = Debugger::manager->coff();
  const Pic::RegistersData &rdata = data.registersData();
  log(Log::LineType::Normal, i18n("Special Function Registers:"));
  QValueVector<Pic::RegisterNameData> list = Pic::sfrList(data);
  for (uint i=0; i<uint(list.count()); i++) log(Log::LineType::Normal, QString("  %1: %2").arg(toHexLabel(list[i].data().address(), rdata.nbCharsAddress())).arg(list[i].label()));
  log(Log::LineType::Normal, i18n("General Purpose Registers:"));
  list = Pic::gprList(data, coff);
  for (uint i=0; i<uint(list.count()); i++) log(Log::LineType::Normal, QString("  %1: %2").arg(toHexLabel(list[i].data().address(), rdata.nbCharsAddress())).arg(list[i].label()));
  return OK;
}

CLI::ExitCode CLI::Interactive::variableList()
{
  if ( _device==0 ) return errorExit(i18n("No device specified"), NOT_SUPPORTED_ERROR);
  const Coff::Object *coff = Debugger::manager->coff();
  if ( coff==0 ) return errorExit(i18n("No COFF file specified"), NOT_SUPPORTED_ERROR);
  if ( _device->group().name()!="pic" ) return errorExit(i18n("No register"), NOT_SUPPORTED_ERROR);
  const Pic::Data &data = static_cast<const Pic::Data &>(*_device);
  const Pic::RegistersData &rdata = data.registersData();
  QValueVector<Pic::RegisterNameData> list = Pic::variableList(data, *coff);
  if ( list.count()==0 ) log(Log::LineType::Normal, i18n("No variable"));
  for (uint i=0; i<uint(list.count()); i++) log(Log::LineType::Normal, QString("  %1: %2").arg(toHexLabel(list[i].data().address(), rdata.nbCharsAddress())).arg(list[i].label()));
  return OK;
}

Address CLI::Interactive::findRegisterAddress(const QString &name)
{
  const Pic::Data &data = static_cast<const Pic::Data &>(*_device);
  const Coff::Object *coff = Debugger::manager->coff();
  const Pic::RegistersData &rdata = data.registersData();
  bool ok;
  Address address = fromAnyLabel(name, &ok);
  if (ok) {
    if ( address>rdata.addressFromIndex(rdata.nbRegisters()-1) ) return Address();
    return address;
  }
  QValueVector<Pic::RegisterNameData> sfrs = Pic::sfrList(data);
  for (uint i=0; i<uint(sfrs.count()); i++) if ( name.lower()==sfrs[i].label().lower() ) return sfrs[i].data().address();
  if ( coff==0 ) return Address();
  QMap<QString, Address> variables = coff->variables();
  if ( variables.contains(name) ) return variables[name];
  return Address();
}

CLI::ExitCode CLI::Interactive::executeRegister(const QString &name, const QString &value)
{
  if ( Debugger::manager->debugger()==0 ) return errorExit(i18n("You need to start the debugging session first (with \"start\")."), ARG_ERROR);
  const Pic::Data &data = static_cast<const Pic::Data &>(*_device);
  const Pic::RegistersData &rdata = data.registersData();
  uint nbChars = rdata.nbChars();
  bool ok;
  ulong v = fromAnyLabel(value, &ok);
  if ( !ok ) return errorExit(i18n("Number format not recognized"), ARG_ERROR);
  if ( v>maxValue(NumberBase::Hex, nbChars) ) return errorExit(i18n("The given value is too large (max: %1)").arg(toHexLabel(maxValue(NumberBase::Hex, nbChars), nbChars)), ARG_ERROR);
  Register::TypeData rtd;
  if ( name.lower()=="w" || name.lower()=="wreg" )
    rtd = static_cast<Debugger::PicBase *>(Debugger::manager->debugger())->deviceSpecific()->wregTypeData();
  else if ( name.lower()=="pc" )
    rtd = Debugger::manager->debugger()->pcTypeData();
  else {
    Address address = findRegisterAddress(name);
    if ( !address.isValid() ) return errorExit(i18n("Unknown register or variable name"), ARG_ERROR);
    rtd = Register::TypeData(address, rdata.nbChars());
  }
  if ( value.isEmpty() ) {
    if ( !Debugger::manager->readRegister(rtd) ) return ARG_ERROR;
    return okExit(i18n("%1 = %2").arg(name.upper()).arg(toHexLabel(Register::list().value(rtd), nbChars)));
  }
  return (Debugger::manager->writeRegister(rtd, v) ? OK : EXEC_ERROR);
}

CLI::ExitCode CLI::Interactive::executeRawCommands(const QString &filename)
{
  QFile file(filename);
  if ( !file.open(IO_ReadOnly) ) return errorExit(i18n("Could not open filename \"%1\"").arg(filename), ARG_ERROR);
  if ( Programmer::manager->programmer()==0 ) {
    Programmer::manager->createProgrammer(_device);
    if ( !Programmer::manager->programmer()->simpleConnectHardware() ) return EXEC_ERROR;
  }
  Programmer::Base *programmer = Programmer::manager->programmer();
  for (;;) {
    QString s;
#if QT_VERSION<0x040000
    if ( file.readLine(s, 1000)==-1 ) break;
#else
    char buffer[1000];
    if ( file.readLine(buffer, 1000)==-1 ) break;
    s += buffer;
#endif
    bool write = !s.startsWith(" ");
    s = s.stripWhiteSpace();
    if ( s.isEmpty() ) continue;
    if (write) {
      if ( !programmer->hardware()->rawWrite(s) ) return EXEC_ERROR;
    } else {
      QString rs;
      if ( !programmer->hardware()->rawRead(s.length(), rs) ) return EXEC_ERROR;
      if ( rs!=s ) log(Log::LineType::Warning, i18n("Read string is different than expected: %1 (%2)").arg(rs).arg(s));
    }
  }
  return okExit(i18n("End of command file reached."));
}
