/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "xml_to_data/prog_xml_to_data.h"

#include "progs/pickit2_bootloader/base/pickit2_bootloader_data.h"
#include "devices/base/device_group.h"
#include "devices/pic/base/pic.h"

//-----------------------------------------------------------------------------
namespace Pickit2Bootloader
{

class XmlToData : public Programmer::XmlToData<Data>
{
public:
  XmlToData() : Programmer::XmlToData<Data>("pickit2_bootloader", "Pickit2Bootloader") {}

private:
  virtual void parseData(QDomElement element, Data &data);
};

void Pickit2Bootloader::XmlToData::parseData(QDomElement, Data &)
{
  const Device::Data *ddata = Device::lister().data(currentDevice());
  if ( ddata->group().name()!="pic" ) qFatal("non-pic device not supported");
  const Pic::Data *pdata = static_cast<const Pic::Data *>(ddata);
  if ( !pdata->hasFeature(Pic::Feature::USB) ) qFatal("device does not have USB");
}

} // namespace

//-----------------------------------------------------------------------------
XML_MAIN(Pickit2Bootloader::XmlToData)
