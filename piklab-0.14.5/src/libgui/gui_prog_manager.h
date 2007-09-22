/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GUI_PROG_MANAGER_H
#define GUI_PROG_MANAGER_H

#include "progs/manager/prog_manager.h"
#include "libgui/main_global.h"

namespace Programmer
{
class GuiManager : public Manager
{
Q_OBJECT
public:
  GuiManager(QObject *parent) : Manager(parent) {}

public slots:
  void toggleDevicePower();
  void showAdvancedDialog();

private:
  virtual const Group &group() const { return Main::programmerGroup(); }
  virtual bool internalInitProgramming(bool debugging);
  virtual void setState(State state) { Main::setState(state==Idle ? Main::Idle : Main::Programming); }
  virtual const Device::Data *device() const { return Main::deviceData(); }
  virtual Port::Description portDescription() const;
};

} // namespace

#endif
