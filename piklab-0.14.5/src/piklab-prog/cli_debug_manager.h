/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CLI_DEBUG_MANAGER_H
#define CLI_DEBUG_MANAGER_H

#include "progs/manager/debug_manager.h"
#include "cmdline.h"

namespace Debugger
{
class CliManager : public Manager
{
Q_OBJECT
public:
  CliManager() {}
  virtual PURL::Url coffUrl() const { return CLI::_coffUrl; }

private:
  virtual const Programmer::Group *programmerGroup() const { return CLI::_progGroup; }
  virtual const Device::Data *deviceData() const { return CLI::_device; }
  virtual Log::View *compileView() { return view(); }
  virtual void updateView(bool);
};

} // namespace

#endif
