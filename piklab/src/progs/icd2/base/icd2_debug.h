/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ICD2_DEBUG_H
#define ICD2_DEBUG_H

#include "icd2_prog.h"
#include "devices/pic/pic/pic_debug.h"

namespace Icd2
{
class DebuggerSpecific;

//-----------------------------------------------------------------------------
class DebugProgrammer : public ProgrammerBase
{
Q_OBJECT
public:
  DebugProgrammer(const ::Programmer::Group &group, const Pic::Data *data);
  bool readDebugExecutiveVersion();
  const VersionData &debugExecutiveVersion() const { return _debugExecutiveVersion; }

private:
  VersionData   _debugExecutiveVersion;
  Device::Array _deArray;
  uint          _deStart, _deEnd;

  virtual void clear();
  virtual bool internalSetupHardware();
  virtual bool doProgram(const Device::Memory &memory, const Device::MemoryRange &range);
  virtual bool programAll(const Pic::Memory &memory);
  virtual bool internalRead(Device::Memory *memory, const Device::MemoryRange &range, const ::Programmer::VerifyData *vdata);

  bool getDebugExecutive();
  bool writeDebugExecutive();
  Pic::Memory toDebugMemory(const Pic::Memory &memory, bool withDebugExecutive);
};

//-----------------------------------------------------------------------------
class Debugger : public ::Debugger::PicBase
{
public:
  Debugger(DebugProgrammer &programmer) : ::Debugger::PicBase(programmer) {}
  virtual bool setBreakpoints(const QValueList<Address> &addresses);
  Hardware *hardware() { return static_cast<Hardware *>(_programmer.hardware()); }
  DebugProgrammer &programmer() { return static_cast<DebugProgrammer &>(_programmer); }
  DebuggerSpecific *specific();
  bool waitForTargetMode(Pic::TargetMode mode);
  virtual bool readRegister(const Register::TypeData &data, BitValue &value);
  virtual bool writeRegister(const Register::TypeData &data, BitValue value);
  bool init(bool last);

protected:
  virtual bool internalInit();
  virtual bool internalRun();
  virtual bool internalStep();
  virtual bool softHalt(bool &success);
  virtual bool hardHalt();
  virtual bool internalReset();
  virtual bool updateState();

private:
  bool _initLast;
};

//-----------------------------------------------------------------------------
class DebuggerGroup : public Group
{
public:
  virtual QString name() const { return "icd2_debugger"; }
  virtual QString label() const { return i18n("ICD2 Debugger"); }
  virtual ::Programmer::Properties properties() const { return Group::properties() | ::Programmer::Debugger; }
  virtual uint maxNbBreakpoints(const Device::Data *) const { return 1; }

protected:
  virtual void addDevice(const QString &name, const Device::Data *data, ::Group::SupportType type);
  virtual ::Programmer::Base *createBase(const Device::Data *data) const { return new DebugProgrammer(*this, static_cast<const Pic::Data *>(data)); }
  virtual ::Debugger::Base *createDebuggerBase(::Programmer::Base &base) const { return new Debugger(static_cast<DebugProgrammer &>(base)); }
  virtual ::Debugger::Specific *createDebuggerSpecific(::Debugger::Base &base) const;
};

} // namespace

#endif
