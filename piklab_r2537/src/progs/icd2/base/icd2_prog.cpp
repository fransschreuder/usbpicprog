/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "icd2_prog.h"

#include "common/global/pfile.h"
#include "progs/base/prog_config.h"
#include "devices/list/device_list.h"
#include "icd2_serial.h"
#include "icd2_usb.h"
#include "icd2_data.h"

//-----------------------------------------------------------------------------
void Icd2::ProgrammerBase::clear()
{
  Icd::ProgrammerBase::clear();
  _firmwareId = 0;
  _testData = TestData();
}

bool Icd2::ProgrammerBase::readFirmwareVersion()
{
  if ( !hardware()->setup() ) return false;
  if ( !Icd::ProgrammerBase::readFirmwareVersion() ) return false;
  _firmwareId = hardware()->getFirmwareId();
  return !hasError();
}

bool Icd2::ProgrammerBase::internalSetupHardware()
{
  ::Programmer::Config config;
  if ( !_targetSelfPowered && device()->architecture()==Pic::Architecture::P30F
       && !askContinue(i18n("It is not recommended to power dsPICs from ICD. Continue anyway?")) ) {
    logUserAbort();
    return false;
  }
  return Icd::ProgrammerBase::internalSetupHardware();
}

bool Icd2::ProgrammerBase::selfTest(bool ask)
{
  log(Log::DebugLevel::Normal, "  Self-test");
  if ( !hardware()->selfTest(_testData) ) return false;
  if ( !_testData.pass() ) {
    QString s;
    for (uint i=0; i<TestData::Nb_VoltageTypes; i++) {
      if ( i!=0 ) s += "; ";
      s += _testData.pretty(TestData::VoltageType(i));
    }
    log(Log::LineType::Warning, i18n("Self-test failed: %1").arg(s));
    if ( ask && !askContinue(i18n("Self-test failed (%1). Do you want to continue anyway?").arg(s)) ) {
      logUserAbort();
      return false;
    }
  }
  return !hasError();
}

VersionData Icd2::ProgrammerBase::firmwareVersion(::Programmer::FirmwareVersionType type) const
{
  const FirmwareVersionData *vd = (type==::Programmer::FirmwareVersionType::Min ? &MIN_VERSION_DATA : &MAX_VERSION_DATA);
  const Version &v = vd->version[_firmwareId-1];
  return VersionData(v.major, v.minor, v.dot);
}

VersionData Icd2::ProgrammerBase::mplabVersion(::Programmer::FirmwareVersionType type) const
{
  const FirmwareVersionData *vd = (type==::Programmer::FirmwareVersionType::Min ? &MIN_VERSION_DATA : &MAX_VERSION_DATA);
  return VersionData(vd->mplabMajor, vd->mplabMinor, 0);
}

bool Icd2::ProgrammerBase::setupFirmware()
{
  const FamilyData &fdata = FAMILY_DATA[family(device()->name())];
  log(Log::DebugLevel::Normal, QString("  Firmware id is %1 and we want %2").arg(_firmwareId).arg(fdata.efid));
  if ( fdata.efid==_firmwareId ) return true;
  log(Log::LineType::Information, i18n("  Incorrect firmware loaded."));

  // find firmware file
  PURL::Directory dir = firmwareDirectory();
  if ( dir.isEmpty() ) return false;
  QString nameFilter = "ICD" + QString::number(fdata.efid).rightJustify(2, '0') + "??????.hex";
  QStringList files = dir.files(nameFilter);
  if ( files.isEmpty() ) {
    log(Log::LineType::Error, i18n("Could not find firmware file \"%1\" in directory \"%2\".").arg(nameFilter).arg(dir.path()));
    return false;
  }

  // upload hex file
  PURL::Url url(dir, files[files.count()-1]);
  log(Log::DebugLevel::Normal, QString("  Firmware file: %1").arg(url.pretty()));
  Log::StringView sview;
  PURL::File file(url, sview);
  if ( !file.openForRead() ) {
    log(Log::LineType::Error, i18n("Could not open firmware file \"%1\".").arg(url.pretty()));
    return false;
  }
  if ( !doUploadFirmware(file) ) return false;

  // check firmware
  if ( !readFirmwareVersion() ) return false;
  if ( fdata.efid!=_firmwareId ) {
    log(Log::LineType::Error, i18n("Firmware still incorrect after uploading."));
    return false;
  }
  log(Log::LineType::Information, i18n("  Firmware succesfully uploaded."));
  return true;
}

//-----------------------------------------------------------------------------
Icd2::Programmer::Programmer(const ::Programmer::Group &group, const Pic::Data *data)
  : Icd2::ProgrammerBase(group, data, "icd2_programmer")
{}

//----------------------------------------------------------------------------
Programmer::Properties Icd2::Group::properties() const
{
   return ::Programmer::Programmer | ::Programmer::HasFirmware | ::Programmer::CanUploadFirmware
     | ::Programmer::NeedDeviceSpecificFirmware | ::Programmer::CanReleaseReset
     | ::Programmer::HasSelfTest | ::Programmer::CanReadMemory | ::Programmer::HasConnectedState;
}

bool Icd2::Group::canReadVoltage(Pic::VoltageType type) const
{
  return ( type==Pic::ProgrammerVpp || type==Pic::TargetVdd || type==Pic::TargetVpp );
}

Programmer::Hardware *Icd2::Group::createHardware(::Programmer::Base &base, const ::Programmer::HardwareDescription &hd) const
{
  if ( hd.port.type==PortType::Serial ) return new SerialHardware(base, hd.port.device);
  return new USBHardware(base);
}

Programmer::DeviceSpecific *Icd2::Group::createDeviceSpecific(::Programmer::Base &base) const
{
  return new Icd::DeviceSpecific(base);
}
