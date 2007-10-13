/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *   Copyright (C) Brian C. Lane                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DIRECT_H
#define DIRECT_H

#include "devices/pic/pic/pic_prog.h"

namespace Direct
{
  enum State { Low = 0, High = 1, Off = Low, On = High };
  enum PinType { Vpp = 0, Vdd, Clock, DataOut, DataIn, DataRW, Nb_PinTypes };
  struct PinData {
    const char *label, *offLabel, *onLabel, *key, *comment;
    Port::IODir dir;
    bool canBeGround;
    const char *testComment;
  };
  extern const PinData PIN_DATA[Nb_PinTypes];

//-----------------------------------------------------------------------------
class HardwareData;
struct HData
{
  int pins[Nb_PinTypes];
  uint clockDelay;
};

class Hardware : public ::Programmer::PicHardware
{
public:
  static Hardware *create(Port::Base *port, const Device::Data &device, const HData &data);

public:
  Hardware(::Programmer::Base &base, Port::Base *port, const HardwareData &data);
  bool readBit();
  void setPin(PinType type, bool on);
  void setRead() { setPin(DataRW, true); }
  void setWrite() { setPin(DataRW, false); }

  uint nbPins(Port::IODir dir) const;
  QString pinLabelForIndex(Port::IODir dir, uint i) const;
  Port::IODir ioTypeForPin(int pin) const;
  uint pinForIndex(Port::IODir dir, uint i) const;
  uint indexForPin(Port::IODir dir, int pin) const;
  bool isGroundPin(int pin) const;

  // hardware test --- please use it for a newly
  // designed/constructed programmer board
  // because pin assignation is hard coded in this
  // routine, you might have to edit it. ++Gib:
  //static int hardware_test();
  // timing test --- please use it to ensure that
  // the program meets the  timing specifications
  //static int timing_test();

private:
  HData _data;

  virtual bool internalConnectHardware();

  friend class Programmer;
};
extern bool operator !=(const HData &d1, const HData &d2);

class SerialHardware : public Hardware
{
public:
  SerialHardware(::Programmer::Base &base, const QString &portDevice, const HardwareData &data);
};

class ParallelHardware : public Hardware
{
public:
  ParallelHardware(::Programmer::Base &base, const QString &portDevice, const HardwareData &data);
};

} // namespace

#endif
