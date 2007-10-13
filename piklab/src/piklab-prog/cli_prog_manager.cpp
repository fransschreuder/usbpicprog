/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "cli_prog_manager.h"

#include "progs/base/generic_prog.h"
#include "progs/base/prog_group.h"
#include "progs/base/prog_config.h"
#include "progs/base/hardware_config.h"
#include "progs/base/generic_debug.h"

//----------------------------------------------------------------------------
Port::Description Programmer::CliManager::portDescription() const
{
  Log::Base *log = const_cast<Log::Base *>(static_cast<const Log::Base *>(this));
  if ( CLI::_port.isEmpty() ) {
    log->log(Log::Information, i18n("Using port from configuration file."));
    ::Programmer::GroupConfig config(group());
    return config.portDescription();
  }
  if ( CLI::_port=="usb" ) return Port::Description(Port::USBType, QString::null);
  Port::Type type = Port::findType(CLI::_port);
  if ( type==Port::Nb_Types ) {
    log->log(Log::Warning, i18n("Could not find device \"%1\" as serial or parallel port. Will try to open as serial port.").arg(CLI::_port));
    type = Port::SerialType;
  }
  return Port::Description(type, CLI::_port);
}

bool Programmer::CliManager::isTargetSelfPowered() const
{
  bool targetSelfPowered = ::Programmer::Manager::isTargetSelfPowered();
  if ( !CLI::_targetSelfPowered.isEmpty() )
    targetSelfPowered = ( CLI::_targetSelfPowered=="true" || CLI::_targetSelfPowered=="1" );
  return targetSelfPowered;
}

void Programmer::CliManager::createProgrammer(const Device::Data *data)
{
  HardwareDescription hd;
  hd.port = portDescription();
  ::Hardware::Config *config = CLI::_progGroup->hardwareConfig();
  if (config) {
    if ( CLI::_hardware.isEmpty() ) hd.name = config->currentHardware(hd.port.type);
    else hd.name = CLI::_hardware;
  }
  delete config;
  Manager::createProgrammer(data, hd);
  if ( !CLI::_firmwareDir.isEmpty() ) _programmer->setFirmwareDirectory(CLI::_firmwareDir.path());
}

bool Programmer::CliManager::internalInitProgramming(bool debugging)
{
  if ( !Manager::internalInitProgramming(debugging) ) return false;
  if (debugging) {
    if ( !_programmer->isInDebugging() ) {
      if ( ::Debugger::manager->coff()==0 && !::Debugger::manager->init() ) log(Log::Warning, i18n("Starting debug session without COFF file (no source file information)."));
      if ( !group().isSoftware() ) log(Log::Warning, i18n("Starting debugging session with device memory in an unknown state. You may want to reprogram the device."));
    }
  } else if ( _programmer->isInDebugging() && !::Debugger::manager->halt() ) return false;
  return true;
}
