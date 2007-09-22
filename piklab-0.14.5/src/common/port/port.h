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

#include "common/global/global.h"
#include <qstringlist.h>

namespace Port
{
//-----------------------------------------------------------------------------
extern void msleep(uint ms); // in milliseconds
extern void usleep(uint us); // in microseconds

//-----------------------------------------------------------------------------
  enum Type { SerialType = 0, ParallelType, USBType, Nb_Types };
  class Description {
  public:
    Description() : type(Nb_Types) {}
    Description(Type ptype, const QString &pdevice) : type(ptype), device(pdevice) {}
    Type type;
    QString device;
  };
  struct Data {
    const char *label, *key;
    bool withDevice;
  };
  extern const Data DATA[Nb_Types];
  enum IODir { NoIO = 0, In = 1, Out = 2 };
  extern const char * const IO_DIR_NAMES[3];
  Q_DECLARE_FLAGS(IODirs, IODir)
  Q_DECLARE_OPERATORS_FOR_FLAGS(IODirs)
  extern QStringList probedDeviceList(Type type);
  extern Type findType(const QString &device);
  extern bool isAvailable(Type type);
  struct PinData {
    uint pin;
    const char *label;
  };
  enum LogicType { PositiveLogic, NegativeLogic };
} // namespace

#endif
