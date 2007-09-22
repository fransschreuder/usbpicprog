/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "direct_prog_config.h"

//-----------------------------------------------------------------------------
struct ConstHardwareData
{
  Port::Type portType;
  Direct::HData data;
};

struct ConstStandardHardwareData
{
  ::HProgrammer::HardwareDataInfo info;
  ConstHardwareData data;
};

const ConstStandardHardwareData STANDARD_HARDWARE_DATA[] = {
  { { "Tait classic",           I18N_NOOP("Tait classic"),           0 },
    { Port::ParallelType, { {  -5, -4,  3,  2, 10, 25 },  0 } } },
  { { "Tait 7405/7406",         I18N_NOOP("Tait 7405/7406"),         0 },
    { Port::ParallelType, { {   5,  4, -3, -2,-10, 25 },  0 } } },
  { { "P16PRO40 classic",       I18N_NOOP("P16PRO40 classic"),       0 },
    { Port::ParallelType, { {   5,  4, -3, -2,-10, 25 },  0 } } },
  { { "P16PRO40 7407",          I18N_NOOP("P16PRO40 7407"),          0 },
    { Port::ParallelType, { {  -5, -4,  3,  2, 10, 25 },  0 } } },
  { { "P16PRO40-VPP40 classic", I18N_NOOP("P16PRO40-VPP40 classic"), 0 },
    { Port::ParallelType, { {   6,  4, -3, -2,-10, 25 },  0 } } },
  { { "P16PRO40-VPP40 7407",    I18N_NOOP("P16PRO40-VPP40 7407"),    0 },
    { Port::ParallelType, { {  -6, -4,  3,  2, 10, 25 },  0 } } },
  { { "EPIC+",                  I18N_NOOP("EPIC+"),                  I18N_NOOP("You must disconnect 7407 pin 2") },
    { Port::ParallelType, { {  -5, -4,  3,  2, 10, 25 },  0  } } },
  { { "JDM classic",            I18N_NOOP("JDM classic"),            0 },
    { Port::SerialType,   { {   3,  5,  7,  4,  8,  5 },  0  } } },
  { { "JDM classic (delay 10)", I18N_NOOP("JDM classic (delay 10)"), 0 },
    { Port::SerialType,   { {   3,  5,  7,  4,  8,  5 }, 10  } } },
  { { "JDM classic (delay 20)", I18N_NOOP("JDM classic (delay 20)"), 0 },
    { Port::SerialType,   { {   3,  5,  7,  4,  8,  5 }, 20  } } },
  { { "PIC Elmer",              I18N_NOOP("PIC Elmer"),              0 },
    { Port::SerialType,   { {  -3,  5, -7, -4, -8,  5 },  0 } } },
  { { "Velleman K8048",         I18N_NOOP("Velleman K8048"),         0 },
    { Port::SerialType,   { {  -3,  5, -7, -4, -8,  5 },  0 } } },

  //Added by Mirko Panciri 10/03/2004...
  //Visit http://www.pic-tools.com
  //I have tested only the "Asix Piccolo" version...
  //I think the lines is the same of "Asix Piccolo Grande"...
  //Who have "Asix Piccolo Grande" contact panciom@libero.it for info / result of test!!!
  { { "Asix Piccolo",           I18N_NOOP("Asix Piccolo"),           0 },
    { Port::ParallelType, { {  -6, -7, -5, -3,-10, -2 },  0 } } },
  { { "Asix Piccolo Grande",    I18N_NOOP("Asix Piccolo Grande"),    0 },
    { Port::ParallelType, { {  -6, -7, -5, -3,-10, -2 },  0 } } },

  { { "Propic2 Vpp-1",          I18N_NOOP("Propic2 Vpp-1"),          0 },
    { Port::ParallelType, { {  -5, -4,  3,  2, 10, 25 },  0 } } },
  { { "Propic2 Vpp-2",          I18N_NOOP("Propic2 Vpp-2"),          0 },
    { Port::ParallelType, { {  -6, -4,  3,  2, 10, 25 },  0 } } },
  { { "Propic2 Vpp-3",          I18N_NOOP("Propic2 Vpp-3"),          0 },
    { Port::ParallelType, { {  -7, -4,  3,  2, 10, 25 },  0 } } },
  { { "Myke's EL Cheapo",       I18N_NOOP("Myke's EL Cheapo"),       0 },
    { Port::ParallelType, { {  16, 25, -1,-17, 13, 25 },  0 } } },
  { { "EL Cheapo classic",      I18N_NOOP("EL Cheapo classic"),      I18N_NOOP("not tested") },
    { Port::ParallelType, { {  16, 25,  1, 17,-13, 25 },  0 } } },
  { { "Monty-Robot programmer", I18N_NOOP("Monty-Robot programmer"), 0 },
    { Port::ParallelType, { {  -5,  4,  2,  3, 10, 25 },  0 } } },
  { { "HOODMICRO",              I18N_NOOP("HOODMICRO"),              I18N_NOOP("htpp://k9spud.com/hoodmicro") },
    { Port::SerialType,   { {   4,  5,  7,  3,  8,  5 },  0 } } },

  { { 0, 0, 0 },
    { Port::SerialType,   { {   0,  0,  0,  0,  0,  0 },  0 } } }
};

//-----------------------------------------------------------------------------
void Direct::HardwareData::readConfig(GenericConfig &config)
{
  ::HProgrammer::HardwareData::readConfig(config);
  for (uint i=0; i<Nb_PinTypes; i++) data.pins[i] = config.readIntEntry(PIN_DATA[i].key);
  data.clockDelay = config.readIntEntry("clkdelay");
}

void Direct::HardwareData::writeConfig(GenericConfig &config) const
{
  ::HProgrammer::HardwareData::writeConfig(config);
  for (uint i=0; i<Nb_PinTypes; i++) config.writeEntry(PIN_DATA[i].key, data.pins[i]);
  config.writeEntry("clkdelay", data.clockDelay);
}

bool Direct::HardwareData::isEqual(const ::HProgrammer::HardwareData &cdata) const
{
  if ( !::HProgrammer::HardwareData::isEqual(cdata) ) return false;
  const HData &hdata = static_cast<const HardwareData &>(cdata).data;
  if ( data.clockDelay!=hdata.clockDelay ) return false;
  for (uint i=0; i<Nb_PinTypes; i++)
    if ( data.pins[i]!=hdata.pins[i] ) return false;
  return true;
}

//-----------------------------------------------------------------------------
QStringList Direct::Config::standardHardwareNames(Port::Type type) const
{
  QStringList names;
  for (uint i=0; STANDARD_HARDWARE_DATA[i].info.name; i++)
    if ( STANDARD_HARDWARE_DATA[i].data.portType==type ) names += STANDARD_HARDWARE_DATA[i].info.name;
  return names;
}

const HProgrammer::HardwareDataInfo *Direct::Config::standardHardwareDataInfo(const QString &name) const
{
  for (uint i=0; STANDARD_HARDWARE_DATA[i].info.name; i++) {
    const ConstStandardHardwareData &csdata = STANDARD_HARDWARE_DATA[i];
    if ( csdata.info.name==name) return &csdata.info;
  }
  return 0;
}

HProgrammer::HardwareData *Direct::Config::standardHardwareData(const QString &name) const
{
  for (uint i=0; STANDARD_HARDWARE_DATA[i].info.name; i++) {
    const ConstStandardHardwareData &csdata = STANDARD_HARDWARE_DATA[i];
    if ( csdata.info.name!=name) continue;
    HardwareData *data = new HardwareData;
    data->portType = csdata.data.portType;
    data->data = csdata.data.data;
    return data;
  }
  return 0;
}
