/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "gpsim_debug.h"

#include <signal.h>
#include <qregexp.h>

#include "devices/list/device_list.h"
#include "devices/pic/pic/pic_debug.h"
#include "devices/pic/base/pic_register.h"
#include "coff/base/coff.h"

//----------------------------------------------------------------------------
GPSim::Debugger::Debugger(Programmer &programmer)
  : ::Debugger::PicBase(programmer), _nbBreakpoints(0)
{}

bool GPSim::Debugger::internalInit()
{
  if ( !hardware()->execute("processor pic" + device()->name().lower(), true) ) return false;
  if ( _inputType==PURL::Cod ) return hardware()->execute("load s " + _filename, true);
  Q_ASSERT( _inputType==PURL::Hex );
  return hardware()->execute("load h " + _filename, true);
}

bool GPSim::Debugger::internalRun()
{
  return hardware()->execute("run", false);
}

bool GPSim::Debugger::hardHalt()
{
  log(Log::Warning, i18n("Failed to halt target: kill process."));
  _programmer.disconnectHardware();
  return true;
}

bool GPSim::Debugger::softHalt(bool &success)
{
  success = hardware()->signal(SIGINT, true);
  return true;
}

bool GPSim::Debugger::internalStep()
{
  return hardware()->execute("step", true);
}

bool GPSim::Debugger::setBreakpoints(const QValueList<Address> &list)
{
  for (uint i=0; i<_nbBreakpoints; i++)
    if ( !hardware()->execute("clear " + QString::number(i), true) ) return false;
  for (uint i=0; i<uint(list.count()); i++)
    if ( !hardware()->execute("break e 0x" + toHex(list[i], nbChars(list[i].toUInt())), true) ) return false;
  _nbBreakpoints = list.count();
  return true;
}

bool GPSim::Debugger::internalReset()
{
  if ( _programmer.state()==::Programmer::Running && !halt() ) return false;
  return hardware()->execute("reset", true);
}

bool GPSim::Debugger::updateState()
{
  if ( hardware()->isReady() ) _programmer.setState(::Programmer::Halted);
  else _programmer.setState(::Programmer::Running);
  return true;
}

bool GPSim::Debugger::findRegExp(const QStringList &lines, const QString &pattern,
                                 const QString &label, QString &value) const
{
  QRegExp rexp(pattern);
  uint i = 0;
  for (; i<uint(lines.count()); i++) {
    int offset = 0;
    for (;;) {
      offset = rexp.search(lines[i], offset, QRegExp::CaretAtOffset);
      if ( offset==-1 || rexp.cap(1)==label ) break;
      offset += rexp.cap(0).length();
    }
    if ( offset!=-1 ) break;
  }
  if ( i==uint(lines.count()) ) return false;
  value = rexp.cap(2);
  return true;
}

bool GPSim::Debugger::readWreg(BitValue &value)
{
  // #### only known for version 4 and 11
  if ( hardware()->version()<=VersionData(0, 21, 7) || hardware()->version()>=VersionData(0, 22, 0) )
    return getRegister("W", value);
  QStringList lines;
  if ( !hardware()->execute("dump s", true, &lines) ) return false;
  QString s;
  if ( !findRegExp(lines, "^\\s*[0-9A-Fa-f]+\\s+(\\w+)\\s*=\\s*([0-9A-Fa-f]+)", "W", s) ) {
    log(Log::Error, i18n("Error reading register \"W\""));
    return false;
  }
  value = fromHex(s, 0);
  return true;
}

bool GPSim::Debugger::getRegister(const QString &name, BitValue &value)
{
  QStringList lines;
  QRegExp r;
  if ( hardware()->version()<VersionData(0, 22, 0) ) {
    if ( !hardware()->execute("x " + name, true, &lines) ) return false;
    r.setPattern("\\w+\\s*[][\\w]+\\s*=\\s*(?:0x|)([0-9A-Fa-f]+)(?:\\W.*|)");
  } else {
    if ( !hardware()->execute(name, true, &lines) ) return false;
    r.setPattern("[^=]*=\\s*(?:0x|\\$)([0-9A-Fa-f]+)(?:\\W.*|)");
  }
  uint i = 0;
  for (; i<uint(lines.count()); i++)
    if ( r.exactMatch(lines[i]) ) break;
  if ( i==uint(lines.count()) ) {
    log(Log::Error, i18n("Error reading register \"%1\"").arg(name));
    return false;
  }
  value = fromHex(r.cap(1), 0);
  return true;
}

bool GPSim::Debugger::getRegister(Address address, BitValue &value)
{
  const Pic::RegistersData &rdata = device()->registersData();
  QString name = toHex(address, rdata.nbCharsAddress());
  if ( hardware()->version()<VersionData(0, 22, 0) ) return getRegister("0x" + name, value);
  return getRegister(QString("ramData[$%1]").arg(name), value);
}

bool GPSim::Debugger::readRegister(const Register::TypeData &data, BitValue &value)
{
  if ( data.type()==Register::Special ) {
    if ( data.name()=="WREG" ) return readWreg(value);
    if ( data.name()=="PC" ) return getRegister("pc", value);
    Q_ASSERT(false);
    return true;
  }
  QString name = device()->registersData().sfrNames[data.address()];
  if ( name=="WREG" ) return readWreg(value);
  if ( !name.isEmpty() ) return getRegister(name.lower(), value);
  return getRegister(data.address(), value);
}

bool GPSim::Debugger::setRegister(const QString &name, BitValue value)
{
  if ( hardware()->version()<VersionData(0, 22, 0) ) {
    log(Log::Warning, i18n("Writing registers is not supported by this version of gpsim"));
    return true;
  }
  const Pic::RegistersData &rdata = device()->registersData();
  QString s = QString("%1 = %2").arg(name).arg(toHexLabel(value, rdata.nbChars()));
  return hardware()->execute(s, true);
}

bool GPSim::Debugger::setRegister(Address address, BitValue value)
{
  const Pic::RegistersData &rdata = device()->registersData();
  QString s = QString("ramData[$%1]").arg(toHex(address, rdata.nbCharsAddress()));
  return setRegister(s, value);
}

bool GPSim::Debugger::writeRegister(const Register::TypeData &data, BitValue value)
{
  if ( data.type()==Register::Special ) {
    if ( data.name()=="WREG" ) return writeWreg(value);
    if ( data.name()=="PC" ) {
      log(Log::Warning, i18n("Writing PC is not supported by gpsim"));
      return true;
    }
    Q_ASSERT(false);
    return false;
  }
  const Pic::RegistersData &rdata = device()->registersData();
  QString name = rdata.sfrNames[data.address()];
  if ( !name.isEmpty() ) return setRegister(name.lower(), value);
  return setRegister(data.address(), value);
}

bool GPSim::Debugger::writeWreg(BitValue value)
{
  return setRegister("W", value);
}

bool GPSim::Debugger::updatePortStatus(uint index, QMap<uint, Device::PortBitData> &bits)
{
  for (uint i=0; i<Device::MAX_NB_PORT_BITS; i++) {
    if ( !device()->registersData().hasPortBit(index, i) ) continue;
    QString name = device()->registersData().portName(index).lower() + QString::number(i);
    QStringList lines;
    if ( !hardware()->execute("symbol " + name, true, &lines) ) return false;
    QString pattern = "^(\\w+)=([^\\s])+\\s*", value;
    if ( !findRegExp(lines, pattern, "bitState", value) || value.length()!=1 ) {
      log(Log::Error, i18n("Error reading state of IO bit: %1").arg(name));
      return false;
    }
    switch (value[0].latin1()) {
      case 'H':
      case '1': bits[i].state = Device::High; break;
      case 'L':
      case '0': bits[i].state = Device::Low; break;
      case 'W': bits[i].state = Device::WeakPullUp; break;
      case 'w': bits[i].state = Device::WeakPullDown; break;
      case 'Z': bits[i].state = Device::HighImpedance; break;
      case 'X': bits[i].state = Device::Unknown; break;
      default:
        bits[i].state = Device::Unknown;
        log(Log::Warning, i18n("Unknown state for IO bit: %1 (%2)").arg(name).arg(value));
        break;
    }
    if ( !findRegExp(lines, pattern, "Driving", value) || value.length()!=1 ) {
      log(Log::Error, i18n("Error reading driving state of IO bit: %1").arg(name));
      return false;
    }
    bits[i].driving = ( value[0]=='1' );
    if (bits[i].driving) {
      if ( !findRegExp(lines, pattern, "drivingState", value) || value.length()!=1 ) {
        log(Log::Error, i18n("Error reading driving state of IO bit: %1").arg(name));
        return false;
      }
      bits[i].drivingState = (value[0]=='0' ? Device::IoLow : Device::IoHigh);
      bits[i].drivenState = Device::IoUnknown;
    } else {
      if ( !findRegExp(lines, pattern, "drivenState", value) || value.length()!=1 ) {
        log(Log::Error, i18n("Error reading driven state of IO bit: %1").arg(name));
        return false;
      }
      bits[i].drivenState = (value[0]=='0' ? Device::IoLow : Device::IoHigh);
      bits[i].drivingState = Device::IoUnknown;
    }
  }
  return true;
}

//----------------------------------------------------------------------------
QString GPSim::Group::statusLabel() const
{
  return i18n("GPSim (4MHz)"); // #### FIXME: add config
}

void GPSim::Group::initSupported()
{
  ProcessManager manager(0);
  if ( !manager.start() ) return;
  VersionData version;
  if ( !manager.getVersion(version) ) return;
  bool oldGpsim = ( version<VersionData(0, 21, 11) );
  if ( !manager.sendCommand("processor list", true) ) return;
  QStringList devices = QStringList::split(" ", manager.process().sout().join(" "));
  for (uint i=0; i<uint(devices.count()); i++) {
    QString s = devices[i].upper();
    if ( s.startsWith("PIC") ) s = s.mid(3);
    const Pic::Data *data = static_cast<const Pic::Data *>(Device::lister().data(s));
    if (data) {
      if ( data->architecture()==Pic::Architecture::P18F && oldGpsim ) continue;
      addDevice(data->name(), data, ::Group::Tested);
    }
  }
}

Programmer::Hardware *GPSim::Group::createHardware(::Programmer::Base &base, const ::Programmer::HardwareDescription &) const
{
  return new Hardware(base);
}

Programmer::DeviceSpecific *GPSim::Group::createDeviceSpecific(::Programmer::Base &base) const
{
  return new DeviceSpecific(base);
}
