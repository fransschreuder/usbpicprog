/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef USBPICPROG_PROG_CONFIG_H
#define USBPICPROG_PROG_CONFIG_H

#include "usbpicprog.h"
#include "progs/base/hardware_config.h"

namespace Usbpicprog
{

class HardwareData : public ::HProgrammer::HardwareData
{
public:
  virtual void readConfig(GenericConfig &config);
  virtual void writeConfig(GenericConfig &config) const;
  virtual bool isEqual(const ::HProgrammer::HardwareData &data) const;
  HData data;
};

class Config : public ::HProgrammer::Config
{
public:
  Config() : ::HProgrammer::Config("usbpicprog_programmer") {}

protected:
  virtual QStringList standardHardwareNames(Port::Type type) const;
  virtual const ::HProgrammer::HardwareDataInfo *standardHardwareDataInfo(const QString &name) const;
  virtual ::HProgrammer::HardwareData *standardHardwareData(const QString &name) const;
  virtual ::HProgrammer::HardwareData *createHardwareData() const { return new HardwareData; }
};

} //namespace

#endif
