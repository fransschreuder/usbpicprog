/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

#include "common/global/generic_config.h"
#include "common/port/port.h"

namespace Hardware
{
//-----------------------------------------------------------------------------
struct DataInfo
{
  const char *name, *label, *comment;
};

class Data
{
public:
  Data() : portType(PortType::Nb_Types) {}
  virtual ~Data() {}
  virtual void readConfig(GenericConfig &config);
  virtual void writeConfig(GenericConfig &config) const;
  virtual bool isEqual(const Data &data) const;
  PortType portType;
  QString  name;
};

//-----------------------------------------------------------------------------
class Config : public GenericConfig
{
public:
  Config(const char *group) : GenericConfig(group) {}
  virtual ~Config() {}
  QStringList hardwareNames(PortType type);
  bool isStandardHardware(const QString &name) const { return standardHardwareData(name); }
  QString label(const QString &name) const;
  QString comment(const QString &name) const;
  void writeCustomHardware(const QString &name, const Hardware::Data &data);
  QString currentHardware(PortType type);
  void writeCurrentHardware(PortType type, const QString &name);
  void deleteCustomHardware(const QString &name);
  Hardware::Data *hardwareData(const QString& name) const;

protected:
  virtual QStringList standardHardwareNames(PortType type) const = 0;
  virtual Hardware::Data *standardHardwareData(const QString &name) const = 0;
  virtual const DataInfo *standardHardwareDataInfo(const QString &name) const = 0;
  virtual Hardware::Data *createHardwareData() const = 0;
};

} // namespace

#endif
