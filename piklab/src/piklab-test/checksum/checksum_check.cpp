/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "checksum_check.h"

#include "devices/base/device_group.h"

//----------------------------------------------------------------------------
bool ChecksumCheck::skip(const Device::Data &data) const
{
  return ( data.group().name()!="pic" );
}

bool ChecksumCheck::init(const Device::Data &data)
{
  _memory = static_cast<Pic::Memory *>(data.group().createMemory(data));
  return true;
}

void ChecksumCheck::cleanup(const Device::Data &)
{
  delete _memory;
  _memory = 0;
}

void ChecksumCheck::setProtection(const Pic::Data &data, const Pic::Checksum::Data &cdata,
                                  const QString &maskName, const QString &valueName)
{
  const Pic::Protection &protection = data.config().protection();
  if ( !maskName.isEmpty() && !valueName.isEmpty() ) _memory->setConfigValue(maskName, valueName);
  if ( !valueName.isEmpty() ) _memory->setUserIdToUnprotectedChecksum();
  for (uint i=0; i<cdata.protectedMaskNames.count(); i++) {
    QString pmName = cdata.protectedMaskNames[i];
    const Pic::Config::Mask *mask = data.config().findMask(pmName, 0);
    for (int k=mask->values.count()-1; k>=0; k--) {
      if ( mask->values[k].name.isEmpty() ) continue;
      if ( protection.isNoneProtectedValueName(mask->values[k].name) ) continue;
      _memory->setConfigValue(pmName, mask->values[k].name);
      break;
    }
  }
  if ( !cdata.bbsize.isEmpty() ) _memory->setConfigValue(protection.bootSizeMaskName(), cdata.bbsize);
}

bool ChecksumCheck::checkChecksum(BitValue checksum, const QString &label)
{
  BitValue c = _memory->checksum();
  if ( c!=checksum ) TEST_FAILED(QString("%1 %2/%3").arg(label).arg(toHexLabel(c, 4)).arg(toHexLabel(checksum, 4)))
  return true;
}

void ChecksumCheck::checkChecksum(const Pic::Data &pdata, const QString &maskName, const QString &valueName, bool &ok)
{
  if ( !pdata.checksums().contains(valueName) ) {
    const Pic::Config::Mask *mask = pdata.config().findMask(maskName);
    QString label = valueName + (mask ? "/" + mask->name : QString::null);
    printf("Missing checksum for \"%s\"", label.latin1());
    return;
  }
  const Pic::Checksum::Data &cdata = pdata.checksums()[valueName];
  _memory->clear();
  setProtection(pdata, cdata, maskName, valueName);
  if ( !checkChecksum(cdata.blankChecksum, maskName + ":" + valueName + "/" + "blank") ) ok = false;
  _memory->checksumCheckFill();
  setProtection(pdata, cdata, maskName, valueName);
  if ( !checkChecksum(cdata.checkChecksum, maskName + ":" + valueName + "/" + "check") ) ok = false;
}

bool ChecksumCheck::execute(const Device::Data &data)
{
  const Pic::Data &pdata = static_cast<const Pic::Data &>(data);
  if ( data.name()=="18C601" || data.name()=="18C801" ) TEST_PASSED;
  if ( pdata.checksums().isEmpty() ) TEST_FAILED("No checksum data");
  bool ok = true;
  const Pic::Protection &protection = pdata.config().protection();
  switch ( protection.family() ) {
    case Pic::Protection::NoProtection:
      checkChecksum(pdata, QString::null, QString::null, ok);
      break;
    case Pic::Protection::BasicProtection: {
      QString maskName = protection.maskName(Pic::Protection::ProgramProtected, Pic::MemoryRangeType::Code);
      const Pic::Config::Mask *mask = pdata.config().findMask(maskName);
      Q_ASSERT(mask);
      for (uint i=0; i<mask->values.count(); i++) {
        QString valueName = mask->values[i].name;
        if ( valueName.isEmpty() ) continue; // invalid value
        checkChecksum(pdata, maskName, valueName, ok);
      }
      break;
    }
    case Pic::Protection::CodeGuard:
      checkChecksum(pdata, "GSSEC", "Off", ok);
      checkChecksum(pdata, "GSSEC", "High Security", ok);
      break;
    case Pic::Protection::BlockProtection: {
      const QMap<QString, Pic::Checksum::Data> &cmap = pdata.checksums();
      QMap<QString, Pic::Checksum::Data>::const_iterator it;
      for (it=cmap.begin(); it!=cmap.end(); ++it)
        checkChecksum(pdata, QString::null, it.key(), ok);
      break;
    }
    case Pic::Protection::Nb_Families: Q_ASSERT(false); break;
  }

  if ( !ok ) return false;
  TEST_PASSED
}

//----------------------------------------------------------------------------
TEST_MAIN(ChecksumCheck)
