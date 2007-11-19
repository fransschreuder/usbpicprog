/***************************************************************************
 * Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit2v2_prog.h"

#include "devices/list/device_list.h"
#include "progs/base/prog_config.h"
#include "pickit2v2.h"

//----------------------------------------------------------------------------
Pickit2V2::Base::Base(const Programmer::Group &group, const Pic::Data *data)
  : Pickit2::Base(group, data)
{}

VersionData Pickit2V2::Base::firmwareVersion(Programmer::FirmwareVersionType type) const
{
  switch (type.type()) {
    case Programmer::FirmwareVersionType::Min: return VersionData(2, 10, 0);
    case Programmer::FirmwareVersionType::Recommended: return VersionData(2, 10, 0);
    case Programmer::FirmwareVersionType::Max: return VersionData(2, 10, 0);
    case Programmer::FirmwareVersionType::Nb_Types: break;
  }
  Q_ASSERT(false);
  return VersionData();
}

Pickit2V2::Hardware &Pickit2V2::Base::hardware()
{
  return static_cast<Hardware &>(*_hardware);
}

bool Pickit2V2::Base::identifyDevice()
{
  // ### REMOVE ME
  ushort status;
  if ( !hardware().readStatus(status) ) return false;

  double vdd = 3.3; // set 3.3V to check for fragile devices
  if ( !_targetSelfPowered ) {
    if ( !hardware().setVddVoltage(vdd, 0.85)) return false;
  }
  QString message = i18n("Unknown device");
  for (uint i=0; FAMILY_DATA[i].architecture!=Pic::Architecture::Nb_Types; i++) {
    const FamilyData &fdata = FAMILY_DATA[i];
    if ( fdata.readDevIdScript==0 ) {
      if ( device()->architecture()==fdata.architecture ) return true; // not autodetectable
      continue;
    }
    hardware().setVddOn(false);
    if ( fdata.vpp==0.0 ) hardware().setVppVoltage(vdd, 0.7);
    else hardware().setVppVoltage(fdata.vpp, 0.7);
    hardware().setVddOn(true);
    if ( !hardware().executeScript(fdata.progEntryScript) ) return false;
    if ( !hardware().executeScript(fdata.readDevIdScript) ) return false;
    if ( !hardware().port().command(UploadData) ) return false;
    Array data;
    if ( !hardware().port().receive(data) ) return false;
    if ( !hardware().executeScript(fdata.progExitScript) ) return false;
    uint rawId = (data[2]<<8) + data[1];
    if (fdata.progMemShift) rawId >>= 1;
    log(Log::DebugLevel::Normal, QString("Read id for family %1: %2").arg(fdata.architecture.key()).arg(toHexLabelAbs(rawId)));
    QMap<QString, Device::IdData> ids;
    ::Group::Base::ConstIterator it;
    for (it=group().begin(); it!=group().end(); ++it) {
      const Pic::Data *data = static_cast<const Pic::Data *>(it.data().data);
      if ( data->architecture()!=fdata.architecture ) continue;
      Device::IdData idata;
      if ( data->matchId(rawId, idata) ) ids[it.key()] = idata;
    }
    if ( ids.count()!=0 ) {
      log(Log::LineType::Information, i18n("Read id: %1").arg(device()->idNames(ids).join("; ")));
      if ( ids.contains(device()->name()) ) return true;
      message = i18n("Read id does not match the specified device name \"%1\".").arg(device()->name());
      break;
    }
  }
  if ( !askContinue(message) ) {
    logUserAbort();
    return false;
  }
  log(Log::LineType::Information, i18n("Continue with the specified device name: \"%1\"...").arg(device()->name()));
  return true;
}

bool Pickit2V2::Base::setTarget()
{
  if ( !identifyDevice() ) return false;
  return hardware().setTarget();
}

bool Pickit2V2::Base::selfTest(bool ask)
{
  ushort status;
  if ( !hardware().readStatus(status) ) return false;
  QString error;
  if ( status & VppError ) error += i18n("Vpp voltage level error; ");
  if ( status & VddError ) error += i18n("Vdd voltage level error; ");
  if ( error.isEmpty() ) return true;
  log(Log::LineType::Warning, i18n("Self-test failed: %1").arg(error));
  if ( ask && !askContinue(i18n("Self-test failed (%1). Do you want to continue anyway?").arg(error)) ) {
      logUserAbort();
      return false;
    }
  return true;
}

//----------------------------------------------------------------------------
Programmer::Hardware *Pickit2V2::Group::createHardware(Programmer::Base &base, const Programmer::HardwareDescription &) const
{
  return new Hardware(base);
}

Programmer::DeviceSpecific *Pickit2V2::Group::createDeviceSpecific(Programmer::Base &base) const
{
  return new DeviceSpecific(base);
}
