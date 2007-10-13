/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GENERIC_DEBUG_H
#define GENERIC_DEBUG_H

#include "common/common/purl_base.h"
#include "common/global/global.h"
#include "common/global/log.h"
#include "devices/base/register.h"
namespace Programmer { class Base; }

namespace Debugger
{
class DeviceSpecific;
class Specific;
class Manager;

//----------------------------------------------------------------------------
class Base : public Log::Base
{
public:
  Base(Programmer::Base &programmer);
  virtual ~Base();
  void init(DeviceSpecific *deviceSpecific, Specific *specific);
  const Device::Data *device() const;
  Manager &debugManager();
  void setupInput(PURL::FileType type, const QString &directory, const QString &filename);
  QString directory() const { return _directory; }
  bool init();
  bool update();
  bool reset();
  bool run();
  bool halt();
  bool step();
  QString statusString() const;
  virtual bool setBreakpoints(const QValueList<Address> &addresses) = 0;
  BitValue pc() const;
  Register::TypeData pcTypeData() const;
  virtual bool readRegister(const Register::TypeData &data, BitValue &value) = 0;
  virtual bool writeRegister(const Register::TypeData &data, BitValue value) = 0;
  virtual bool updatePortStatus(uint index, QMap<uint, Device::PortBitData> &bits) = 0;

protected:
  Programmer::Base &_programmer;
  DeviceSpecific   *_deviceSpecific;
  Specific         *_specific;
  PURL::FileType    _inputType;
  QString           _directory, _filename;

  virtual bool internalInit() = 0;
  virtual bool internalRun() = 0;
  virtual bool softHalt(bool &success) = 0;
  virtual bool hardHalt() = 0;
  virtual bool internalStep() = 0;
  virtual bool internalReset() = 0;
  virtual bool updateState() = 0;
};

//----------------------------------------------------------------------------
class DeviceSpecific : public Log::Base
{
public:
  DeviceSpecific(Debugger::Base &base) : Log::Base(base), _base(base) {}
  virtual bool updateStatus() = 0;
  virtual QString statusString() const = 0;

protected:
  Debugger::Base &_base;
};

//----------------------------------------------------------------------------
class Specific : public Log::Base
{
public:
  Specific(Debugger::Base &base) : Log::Base(base), _base(base) {}

protected:
  Debugger::Base &_base;
};

} // namespace

#endif
