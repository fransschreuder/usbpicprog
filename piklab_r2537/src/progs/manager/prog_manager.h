/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROG_MANAGER_H
#define PROG_MANAGER_H

#include <qobject.h>

#include "common/global/log.h"
namespace Device { class Data; class Memory; class MemoryRange; }
namespace Port { class Description; }

namespace Programmer
{
class Base;
class Group;
class HardwareDescription;

class Manager : public QObject, public Log::Base
{
Q_OBJECT
public:
  Manager(QObject *parent);
  virtual ~Manager();
  ::Programmer::Base *programmer() { return _programmer; }
  virtual void createProgrammer(const Device::Data *data) = 0;
  bool initProgramming(bool debugging);
  void endProgramming();
  void clear();
  void stop();
  bool program(const Device::Memory &memory, const Device::MemoryRange &range);
  bool verify(const Device::Memory &memory, const Device::MemoryRange &range);
  bool read(Device::Memory &memory, const Device::MemoryRange &range);
  bool erase(const Device::MemoryRange &range);
  bool blankCheck(const Device::MemoryRange &range);
  bool setDevicePower(bool on);
  enum State { Idle, Programming };
  virtual void setState(State state) = 0;

public slots:
  bool connectDevice();
  bool disconnectDevice();
  bool run();
  bool halt();
  bool restart();
  bool step();

signals:
  void actionMessage(const QString &message);
  void showProgress(bool show);
  void setTotalProgress(uint steps);
  void setProgress(uint steps);

protected:
  ::Programmer::Base *_programmer;

  virtual const Group &group() const = 0;
  virtual bool internalInitProgramming(bool debugging);
  virtual const Device::Data *device() const = 0;
  virtual bool isTargetSelfPowered() const;
  virtual void createProgrammer(const Device::Data *data, const HardwareDescription &hd);
};

extern Manager *manager;

} // namespace

#endif
