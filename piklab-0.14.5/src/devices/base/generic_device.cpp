/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "generic_device.h"

#include "common/global/global.h"
#include "device_group.h"
#include "register.h"

//-----------------------------------------------------------------------------
const Device::StatusData Device::STATUS_DATA[Nb_Status] = {
  { "IP",       I18N_NOOP("In Production")                  },
  { "Future",   I18N_NOOP("Future Product")                 },
  { "NR",       I18N_NOOP("Not Recommended for New Design") },
  { "EOL",      I18N_NOOP("End Of Life"  )                  },
  { "?",        I18N_NOOP("Unknown")                        },
  { "Mature",   I18N_NOOP("Mature")                         }
};

const Device::MemoryTechnologyData Device::MEMORY_TECHNOLOGY_DATA[Nb_MemoryTechnologies] = {
  { "EEE",     I18N_NOOP("Enhanced Flash") },
  { "EE",      I18N_NOOP("Standard Flash") },
  { "EPROM",   I18N_NOOP("EPROM (OTP)")    },
  { "ROM",     I18N_NOOP("ROM")            },
  { "ROMLESS", I18N_NOOP("ROM-less")       }
};

const uint Device::INVALID = 0xFFFFFFFFU;

const Device::FrequencyRange::Data Device::FrequencyRange::TYPE_DATA[Nb_Types] = {
  { "commercial", I18N_NOOP("Commercial") },
  { "industrial", I18N_NOOP("Industrial") },
  { "extended",   I18N_NOOP("Extended")   }
};

const Device::FrequencyRange::Data Device::FrequencyRange::SPECIAL_DATA[Nb_Specials] = {
  { "",            I18N_NOOP("Normal")      },
  { "low power",   I18N_NOOP("Low Power")   },
  { "low voltage", I18N_NOOP("Low Voltage") }
};

const Device::Package::TypeData Device::Package::TYPE_DATA[] = {
  { "pdip",  I18N_NOOP("PDIP"),   Bar,    {  8, 14, 18, 20, 28, 40,  0,  0,   0 } },
  { "sdip",  I18N_NOOP("SDIP"),   Bar,    {  0,  0,  0,  0, 28,  0,  0,  0,   0 } }, // skinny
  { "spdip", I18N_NOOP("SDIP"),   Bar,    {  0,  0,  0,  0,  0,  0, 64,  0,   0 } }, // shrink
  { "sot23", I18N_NOOP("SOT-23"), Bar,    {  5,  6,  0,  0,  0,  0,  0,  0,   0 } },
  { "msop",  I18N_NOOP("MSOP"),   Bar,    {  8,  0,  0,  0,  0,  0,  0,  0,   0 } },
  { "ssop",  I18N_NOOP("SSOP"),   Bar,    {  0,  0,  0, 20, 28,  0,  0,  0,   0 } },
  { "tssop", I18N_NOOP("TSSOP"),  Bar,    {  8, 14,  0,  0,  0,  0,  0,  0,   0 } },
  { "soic",  I18N_NOOP("SOIC"),   Bar,    {  8, 14, 18, 20, 28,  0,  0,  0,   0 } },
  { "tqfp",  I18N_NOOP("TQFP"),   Square, {  0,  0,  0,  0,  0, 44, 64, 80, 100 } },
  { "mqfp",  I18N_NOOP("MQFP"),   Square, {  0,  0,  0,  0,  0, 44,  0,  0,   0 } },
  { "dfns",  I18N_NOOP("DFN-S"),  Bar,    {  8,  0,  0,  0,  0,  0,  0,  0,   0 } },
  { "qfn",   I18N_NOOP("QFN"),    Square, {  0, 16,  0, 20, 28, 44,  0,  0,   0 } },
  { "plcc",  I18N_NOOP("PLCC"),   Square, {  0,  0,  0,  0,  0, 44, 68, 84,   0 } },
  { "mlf",   I18N_NOOP("MLF"),    Square, {  0,  0,  0,  0, 28,  0,  0,  0,   0 } },
  {     0,                  0,    Square, {  0,  0,  0,  0,  0,  0,  0,  0,   0 } }
};

//-----------------------------------------------------------------------------
double Device::FrequencyRange::vddMin() const
{
  double vdd = 0.0;
  for (uint i=0; i<uint(vdds.count()); i++) {
    if ( i==0 ) vdd = vdds[i].yMin();
    vdd = qMin(vdd, vdds[i].yMin());
  }
  return vdd;
}

double Device::FrequencyRange::vddMax() const
{
  double vdd = 0.0;
  for (uint i=0; i<uint(vdds.count()); i++) {
    if ( i==0 ) vdd = vdds[i].yMax();
    vdd = qMax(vdd, vdds[i].yMax());
  }
  return vdd;
}

//-----------------------------------------------------------------------------
Device::Array::Array(const Array &array)
  : _data(array.size())
{
  for (uint i=0; i<array.size(); i++) _data[i] = array[i];
}

Device::Array &Device::Array::operator +=(const Array &a)
{
  uint s = _data.size();
  _data.resize(s + a.size());
  for (uint i=0; i<a.size(); i++) _data[s+i] = a[i];
  return *this;
}

void Device::Array::append(uint v)
{
  uint s = _data.size();
  _data.resize(s+1);
  _data[s] = v;
}

Device::Array Device::Array::mid(uint index, int s) const
{
  Q_ASSERT( index<size() && s<=int(size()-index) && s>=-1);
  Array array(s==-1 ? size()-index : s);
  for (uint i=0; i<array.size(); i++) array[i] = _data[index+i];
  return array;
}

Device::Array &Device::Array::operator =(const Array &array)
{
  _data.resize(array.size());
  for (uint i=0; i<array.size(); i++) _data[i] = array[i];
  return *this;
}

//-----------------------------------------------------------------------------
Device::Data::~Data()
{
  delete _registersData;
}

double Device::Data::vddMin() const
{
  double vdd = 0.0;
  for (uint i=0; i<uint(_frequencyRanges.count()); i++) {
    if ( i==0 ) vdd = _frequencyRanges[i].vddMin();
    vdd = qMin(vdd, _frequencyRanges[i].vddMin());
  }
  return vdd;
}

double Device::Data::vddMax() const
{
  double vdd = 0.0;
  for (uint i=0; i<uint(_frequencyRanges.count()); i++) {
    if ( i==0 ) vdd = _frequencyRanges[i].vddMax();
    vdd = qMax(vdd, _frequencyRanges[i].vddMax());
  }
  return vdd;
}

//-----------------------------------------------------------------------------
QDataStream &Device::operator <<(QDataStream &s, const RangeBox::Value &rbv)
{
  s << rbv.x << rbv.yMin << rbv.yMax;
  return s;
}
QDataStream &Device::operator >>(QDataStream &s, RangeBox::Value &rbv)
{
  s >> rbv.x >> rbv.yMin >> rbv.yMax;
  return s;
}

QDataStream &Device::operator <<(QDataStream &s, const RangeBox &rb)
{
  s << rb.start << rb.end << rb.osc << rb.mode << rb.special;
  return s;
}
QDataStream &Device::operator >>(QDataStream &s, RangeBox &rb)
{
  s >> rb.start >> rb.end >> rb.osc >> rb.mode >> rb.special;
  return s;
}

QDataStream &Device::operator <<(QDataStream &s, const FrequencyRange &frange)
{
  s << Q_UINT8(frange.type) << Q_UINT8(frange.special) << frange.vdds;
  return s;
}
QDataStream &Device::operator >>(QDataStream &s, FrequencyRange &frange)
{
  Q_UINT8 ftype, fspecial;
  s >> ftype >> fspecial >> frange.vdds;
  frange.type = FrequencyRange::Type(ftype);
  frange.special = FrequencyRange::Special(fspecial);
  return s;
}

QDataStream &Device::operator <<(QDataStream &s, const Package &package)
{
  s << package.types << package.pins;
  return s;
}
QDataStream &Device::operator >>(QDataStream &s, Package &package)
{
  s >> package.types >> package.pins;
  return s;
}

QDataStream &Device::operator <<(QDataStream &s, const Data &data)
{
  s << data._name << data._document << data._alternatives << Q_UINT8(data._status)
    << data._frequencyRanges << Q_UINT8(data._memoryTechnology)
    << data._packages;
  return s;
}
QDataStream &Device::operator >>(QDataStream &s, Data &data)
{
  Q_UINT8 status, memoryTechnology;
  s >> data._name >> data._document >> data._alternatives >> status
    >> data._frequencyRanges >> memoryTechnology
    >> data._packages;
  data._status = Status(status);
  data._memoryTechnology = MemoryTechnology(memoryTechnology);
  return s;
}
