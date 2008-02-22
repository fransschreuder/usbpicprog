/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DIRECT_PROG_CONFIG_H
#define DIRECT_PROG_CONFIG_H

#include "direct.h"
#include "progs/base/hardware_config.h"

namespace Direct
{

class HardwareData : public ::Hardware::Data
{
public:
  virtual void readConfig(GenericConfig &config);
  virtual void writeConfig(GenericConfig &config) const;
  virtual bool isEqual(const ::Hardware::Data &data) const;
  HData data;
};

class Config : public ::Hardware::Config
{
public:
  Config() : ::Hardware::Config("direct_programmer") {}

protected:
  virtual QStringList standardHardwareNames(PortType type) const;
  virtual const ::Hardware::DataInfo *standardHardwareDataInfo(const QString &name) const;
  virtual ::Hardware::Data *standardHardwareData(const QString &name) const;
  virtual ::Hardware::Data *createHardwareData() const { return new HardwareData; }
};

} //namespace

#endif
