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

#include "progs/base/prog_config.h"

namespace HProgrammer
{
//-----------------------------------------------------------------------------
struct HardwareDataInfo
{
  const char *name, *label, *comment;
};

class HardwareData
{
public:
  HardwareData() : portType(Port::Nb_Types) {}
  virtual ~HardwareData() {}
  virtual void readConfig(GenericConfig &config);
  virtual void writeConfig(GenericConfig &config) const;
  virtual bool isEqual(const HardwareData &data) const;
  Port::Type portType;
};

//-----------------------------------------------------------------------------
class Config : public GenericConfig
{
public:
  Config(const char *group) : GenericConfig(group) {}
  virtual ~Config() {}
  QStringList hardwareNames(Port::Type type);
  virtual bool isStandardHardware(const QString &name) const { return standardHardwareData(name); }
  QString label(const QString &name) const;
  QString comment(const QString &name) const;
  HardwareData *hardwareData(const QString& name);
  void writeCustomHardware(const QString &name, const HardwareData &data);
  QString currentHardware(Port::Type type);
  void writeCurrentHardware(Port::Type type, const QString &name);
  void deleteCustomHardware(const QString &name);

protected:
  virtual QStringList standardHardwareNames(Port::Type type) const = 0;
  virtual const HardwareDataInfo *standardHardwareDataInfo(const QString &name) const = 0;
  virtual HardwareData *standardHardwareData(const QString &name) const = 0;
  virtual HardwareData *createHardwareData() const = 0;
};

} // namespace

#endif
