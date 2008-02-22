/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PORT_H
#define PORT_H

#include <qstringlist.h>

#include "common/global/global.h"
#include "common/common/key_enum.h"

//-----------------------------------------------------------------------------
struct PortTypeData {
  const char *label, *key;
  bool withDevice;
};

BEGIN_DECLARE_ENUM(PortType)
  Serial = 0, Parallel, USB
END_DECLARE_ENUM(PortType, PortTypeData)

namespace Port
{
//-----------------------------------------------------------------------------
extern void msleep(uint ms); // in milliseconds
extern void usleep(uint us); // in microseconds

//-----------------------------------------------------------------------------
  class Description {
  public:
    Description() : type(PortType::Nb_Types) {}
    Description(PortType ptype, const QString &pdevice) : type(ptype), device(pdevice) {}
    PortType type;
    QString device;
  };

  enum IODir { NoIO = 0, In = 1, Out = 2 };
  extern const char * const IO_DIR_NAMES[3];
  Q_DECLARE_FLAGS(IODirs, IODir)
  Q_DECLARE_OPERATORS_FOR_FLAGS(IODirs)
  extern QStringList probedDeviceList(PortType type);
  extern PortType findType(const QString &device);
  extern bool isAvailable(PortType type);
  struct PinData {
    uint pin;
    const char *label;
  };
  enum LogicType { PositiveLogic, NegativeLogic };
} // namespace

#endif
