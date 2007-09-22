/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DEBUG_MANAGER_H
#define DEBUG_MANAGER_H

#include <qtimer.h>

#include "common/global/log.h"
#include "common/global/purl.h"
#include "common/common/storage.h"
#include "devices/base/generic_device.h"
#include "devices/base/register.h"
#include "breakpoint.h"
#include "prog_manager.h"
#include "progs/base/generic_debug.h"
namespace Coff { class TextObject; }

namespace Debugger
{
class Manager : public QObject, public Log::Base, public GenericView
{
Q_OBJECT
public:
  Manager();
  virtual ~Manager();
  virtual PURL::Url coffUrl() const = 0;
  virtual const Programmer::Group *programmerGroup() const = 0;
  Programmer::Base *programmer() const { return Programmer::manager->programmer(); }
  virtual const Device::Data *deviceData() const = 0;
  Debugger::Base *debugger() const;
  virtual void updateDevice();
  bool prepareDebugging();
  bool isActive() const;
  bool init();
  const Coff::TextObject *coff() const { return _coff; }
  virtual void clear();
  Breakpoint::MarkType breakpointType(const Breakpoint::Data &data) const;
  bool run();
  bool halt();
  bool reset();
  bool step();
  bool readRegister(const Register::TypeData &data);
  bool writeRegister(const Register::TypeData &data, uint value);
  uint pc() const;

public slots:
  void clearBreakpoints();
  virtual bool update();
  void setRegisterWatched(const Register::TypeData &data, bool watched);
  bool readAllRegisters();
  void stopWatchAll();

signals:
  void statusChanged(const QString &text);
  void targetStateChanged();
  void actionMessage(const QString &text);

protected:
  Coff::TextObject *_coff;

  void freeActiveBreakpoint();
  bool checkBreakpoint(const Breakpoint::Data &bdata, bool onlyWarn, uint &address);
  bool updateRegisters();
  virtual bool internalInit();
  virtual Log::View *compileView() = 0;

private slots:
  void slotTimeout();

private:
  const Device::Data *_data;
  QTimer _timer;
  QValueList<Register::TypeData> _readRegisters;

  void computeBreakpointAddresses();
  QValueList<uint> activeBreakpointAddresses() const;
  void updateBreakpointsDisplay();
  bool checkState(bool *first = 0);
  virtual void updateView() { updateView(false); }
  virtual void updateView(bool gotoPC) = 0;
  bool updateRegister(const Register::TypeData &data);
};

extern Manager *manager;

} // namespace

#endif
