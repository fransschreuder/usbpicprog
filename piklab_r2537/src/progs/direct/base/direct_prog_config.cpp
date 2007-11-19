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
  PortType portType;
  Direct::HData data;
};

struct ConstStandardHardwareData
{
  ::Hardware::DataInfo info;
  ConstHardwareData data;
};

const ConstStandardHardwareData STANDARD_HARDWARE_DATA[] = {
  { { "Tait classic",           I18N_NOOP("Tait classic"),           0 },
    { PortType::Parallel, { {  -5, -4,  3,  2, 10, 25 },  0, Direct::Normal } } },
  { { "Tait 7405/7406",         I18N_NOOP("Tait 7405/7406"),         0 },
    { PortType::Parallel, { {   5,  4, -3, -2,-10, 25 },  0, Direct::Normal } } },
  { { "P16PRO40 classic",       I18N_NOOP("P16PRO40 classic"),       0 },
    { PortType::Parallel, { {   5,  4, -3, -2,-10, 25 },  0, Direct::Normal } } },
  { { "P16PRO40 7407",          I18N_NOOP("P16PRO40 7407"),          0 },
    { PortType::Parallel, { {  -5, -4,  3,  2, 10, 25 },  0, Direct::Normal } } },
  { { "P16PRO40-VPP40 classic", I18N_NOOP("P16PRO40-VPP40 classic"), 0 },
    { PortType::Parallel, { {   6,  4, -3, -2,-10, 25 },  0, Direct::Normal } } },
  { { "P16PRO40-VPP40 7407",    I18N_NOOP("P16PRO40-VPP40 7407"),    0 },
    { PortType::Parallel, { {  -6, -4,  3,  2, 10, 25 },  0, Direct::Normal } } },
  { { "EPIC+",                  I18N_NOOP("EPIC+"),
    I18N_NOOP("You must disconnect 7407 pin 2") },
    { PortType::Parallel, { {  -5, -4,  3,  2, 10, 25 },  0, Direct::Normal } } },
  { { "JDM classic",            I18N_NOOP("JDM classic"),            0 },
    { PortType::Serial,   { {   3,  5,  7,  4,  8,  5 },  0, Direct::Normal } } },
  { { "JDM classic (delay 10)", I18N_NOOP("JDM classic (delay 10)"), 0 },
    { PortType::Serial,   { {   3,  5,  7,  4,  8,  5 }, 10, Direct::Normal } } },
  { { "JDM classic (delay 20)", I18N_NOOP("JDM classic (delay 20)"), 0 },
    { PortType::Serial,   { {   3,  5,  7,  4,  8,  5 }, 20, Direct::Normal } } },
  { { "PIC Elmer",              I18N_NOOP("PIC Elmer"),              0 },
    { PortType::Serial,   { {  -3,  5, -7, -4, -8,  5 },  0, Direct::Normal } } },
  { { "Velleman K8048",         I18N_NOOP("Velleman K8048"),         0 },
    { PortType::Serial,   { {  -3,  5, -7, -4, -8,  5 },  0, Direct::Normal } } },
  { { "HOODMICRO",              I18N_NOOP("HOODMICRO"),
    I18N_NOOP("Webpage: <a href=\"htpp://k9spud.com/hoodmicro\">htpp://k9spud.com/hoodmicro</a>") },
    { PortType::Serial,   { {   4,  5,  7,  3,  8,  5 },  0, Direct::Normal } } },

  //Added by Mirko Panciri 10/03/2004...
  //Visit http://www.pic-tools.com
  //I have tested only the "Asix Piccolo" version...
  //I think the lines is the same of "Asix Piccolo Grande"...
  { { "Asix Piccolo",           I18N_NOOP("Asix Piccolo"),           0 },
    { PortType::Parallel, { {  -6, -7, -5, -3,-10, -2 },  0, Direct::Normal } } },
  { { "Asix Piccolo Grande",    I18N_NOOP("Asix Piccolo Grande"),    0 },
    { PortType::Parallel, { {  -6, -7, -5, -3,-10, -2 },  0, Direct::Normal } } },

  { { "Propic2 Vpp-1",          I18N_NOOP("Propic2 Vpp-1"),          0 },
    { PortType::Parallel, { {  -5, -4,  3,  2, 10, 25 },  0, Direct::Normal } } },
  { { "Propic2 Vpp-2",          I18N_NOOP("Propic2 Vpp-2"),          0 },
    { PortType::Parallel, { {  -6, -4,  3,  2, 10, 25 },  0, Direct::Normal } } },
  { { "Propic2 Vpp-3",          I18N_NOOP("Propic2 Vpp-3"),          0 },
    { PortType::Parallel, { {  -7, -4,  3,  2, 10, 25 },  0, Direct::Normal } } },
  { { "Myke's EL Cheapo",       I18N_NOOP("Myke's EL Cheapo"),       0 },
    { PortType::Parallel, { {  16, 25, -1,-17, 13, 25 },  0, Direct::Normal } } },
  { { "EL Cheapo classic",      I18N_NOOP("EL Cheapo classic"),      I18N_NOOP("Not tested.") },
    { PortType::Parallel, { {  16, 25,  1, 17,-13, 25 },  0, Direct::Normal } } },
  { { "Monty-Robot programmer", I18N_NOOP("Monty-Robot programmer"), 0 },
    { PortType::Parallel, { {  -5,  4,  2,  3, 10, 25 },  0, Direct::Normal } } },
  { { "EPE Toolkit mk3",        I18N_NOOP("EPE Toolkit mk3"),
    I18N_NOOP("This programmer pulses MCLR from 5V to 0V and then 12V to enter programming mode. It uses a multiplexer to switch between 5V and 12V (Vdd is here the multiplexer pin).<p>Webpage: <a href=\"http://www.epemag.wimborne.co.uk/1001.htm\">http://www.epemag.wimborne.co.uk/1001.htm</a>") },
    { PortType::Parallel, { {   5,  6,  3,  2, 10, 25 },  0, Direct::EPEToolkitMK3 } } },
  { { "Usbpicprog",              I18N_NOOP("Usbpicprog"),              I18N_NOOP("http://usbpicprog.sourceforge.net") },
    { PortType::USB,   { {   1,  7,  5,  3,  4,  8 },  0 } } },
  { { 0, 0, 0 },
    { PortType::Serial,   { {   0,  0,  0,  0,  0,  0 },  0, Direct::Normal } } }
};

//-----------------------------------------------------------------------------
void Direct::HardwareData::readConfig(GenericConfig &config)
{
  ::Hardware::Data::readConfig(config);
  for (uint i=0; i<Nb_PinTypes; i++) data.pins[i] = config.readIntEntry(PIN_DATA[i].key);
  data.clockDelay = config.readIntEntry("clkdelay");
}

void Direct::HardwareData::writeConfig(GenericConfig &config) const
{
  ::Hardware::Data::writeConfig(config);
  for (uint i=0; i<Nb_PinTypes; i++) config.writeEntry(PIN_DATA[i].key, data.pins[i]);
  config.writeEntry("clkdelay", data.clockDelay);
}

bool Direct::HardwareData::isEqual(const ::Hardware::Data &cdata) const
{
  if ( !::Hardware::Data::isEqual(cdata) ) return false;
  const HData &hdata = static_cast<const HardwareData &>(cdata).data;
  if ( data.clockDelay!=hdata.clockDelay ) return false;
  for (uint i=0; i<Nb_PinTypes; i++)
    if ( data.pins[i]!=hdata.pins[i] ) return false;
  return true;
}

//-----------------------------------------------------------------------------
QStringList Direct::Config::standardHardwareNames(PortType type) const
{
  QStringList names;
  for (uint i=0; STANDARD_HARDWARE_DATA[i].info.name; i++)
    if ( STANDARD_HARDWARE_DATA[i].data.portType==type ) names += STANDARD_HARDWARE_DATA[i].info.name;
  return names;
}

const Hardware::DataInfo *Direct::Config::standardHardwareDataInfo(const QString &name) const
{
  for (uint i=0; STANDARD_HARDWARE_DATA[i].info.name; i++) {
    const ConstStandardHardwareData &csdata = STANDARD_HARDWARE_DATA[i];
    if ( csdata.info.name==name) return &csdata.info;
  }
  return 0;
}

Hardware::Data *Direct::Config::standardHardwareData(const QString &name) const
{
  for (uint i=0; STANDARD_HARDWARE_DATA[i].info.name; i++) {
    const ConstStandardHardwareData &csdata = STANDARD_HARDWARE_DATA[i];
    if ( csdata.info.name!=name) continue;
    HardwareData *data = new HardwareData;
    data->name = csdata.info.name;
    data->portType = csdata.data.portType;
    data->data = csdata.data.data;
    return data;
  }
  return 0;
}
