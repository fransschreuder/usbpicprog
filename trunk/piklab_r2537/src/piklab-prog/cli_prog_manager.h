/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CLI_PROG_MANAGER_H
#define CLI_PROG_MANAGER_H

#include "progs/manager/prog_manager.h"
#include "cli_debug_manager.h"
#include "cmdline.h"

namespace Programmer
{
class CliManager : public Manager
{
Q_OBJECT
public:
  CliManager(QObject *parent) : Manager(parent), _state(Idle) {}
  Port::Description portDescription() const;
  virtual void createProgrammer(const Device::Data *data);

private:
  State _state;

  virtual const Group &group() const { return *CLI::_progGroup; }
  virtual void setState(State state) { _state = state; }
  virtual const Device::Data *device() const { return CLI::_device; }
  virtual bool internalInitProgramming(bool debugging);
  virtual bool isTargetSelfPowered() const;
};

} // namespace

#endif
