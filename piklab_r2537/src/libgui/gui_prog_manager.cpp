/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "gui_prog_manager.h"

#include <kaction.h>

#include "progs/base/generic_prog.h"
#include "progs/base/prog_group.h"
#include "progs/base/prog_config.h"
#include "progs/base/hardware_config.h"
#include "progs/base/generic_debug.h"
#include "progs/gui/prog_group_ui.h"
#include "toplevel.h"
#include "project.h"
#include "project_manager.h"
#include "progs/manager/debug_manager.h"

//----------------------------------------------------------------------------
bool Programmer::GuiManager::internalInitProgramming(bool debugging)
{
  if ( !Manager::internalInitProgramming(debugging) ) return false;
  if ( !debugging && _programmer->isActive() ) {
    if ( group().isDebugger() && !askContinue(i18n("The selected operation will stop the debugging session. Continue anyway?")) ) return false;
    if ( !halt() ) return false;
  }
  if ( debugging && !_programmer->isActive() ) {
    if ( ::Debugger::manager->coff()==0 && !::Debugger::manager->init() ) {
      sorry(i18n("Cannot start debugging without a COFF file. Please compile the project."));
      return false;
    }
    if ( !group().isSoftware() ) {
      if ( !askContinue(i18n("The device memory is in an unknown state. You may want to reprogram the device. Continue anyway?")) )
        return false;
    }
  }
  return true;
}

void Programmer::GuiManager::toggleDevicePower()
{
  bool on = static_cast<KToggleAction *>(Main::action("prog_power"))->isChecked();
  setDevicePower(on);
}

void Programmer::GuiManager::showAdvancedDialog()
{
  const ::Programmer::GroupUI *groupui = static_cast<const ::Programmer::GroupUI *>(group().gui());
  const Device::Data *data = Main::deviceData();
  if ( data && !group().isSupported(data->name()) ) data = 0;
  createProgrammer(data);
  ::Programmer::AdvancedDialog *dialog = groupui->createAdvancedDialog(*_programmer, &Main::toplevel());
  Q_ASSERT(dialog);
  dialog->updateDisplay();
  dialog->exec();
  delete dialog;
}

void Programmer::GuiManager::createProgrammer(const Device::Data *data)
{
  HardwareDescription hd;
  hd.port = GroupConfig::portDescription(group());
  ::Hardware::Config *hconfig = group().hardwareConfig();
  if (hconfig) hd.name = hconfig->currentHardware(hd.port.type);
  delete hconfig;
  Manager::createProgrammer(data, hd);
}
