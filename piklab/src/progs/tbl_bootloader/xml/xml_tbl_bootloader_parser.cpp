/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "xml_to_data/prog_xml_to_data.h"

#include "progs/tbl_bootloader/base/tbl_bootloader_data.h"
#include "devices/base/device_group.h"
#include "devices/pic/base/pic.h"

//-----------------------------------------------------------------------------
namespace TinyBootloader
{

class XmlToData : public Programmer::XmlToData<Data>
{
public:
  XmlToData() : Programmer::XmlToData<Data>("tbl_bootloader", "TinyBootloader") {}

private:
  virtual void parseData(QDomElement element, Data &data);
  virtual void outputData(const Data &data, QTextStream &s) const;
};

void TinyBootloader::XmlToData::parseData(QDomElement element, Data &data)
{
  const Device::Data *ddata = Device::lister().data(currentDevice());
  if ( ddata->group().name()!="pic" ) qFatal("non-pic device not supported");
  const Pic::Data *pdata = static_cast<const Pic::Data *>(ddata);
  if ( !pdata->hasFeature(Pic::Feature::USART) ) qFatal("device does not have USART");
  bool ok;
  data.id = fromHexLabel(element.attribute("id"), 2, &ok);
  if ( !ok ) qFatal("Invalid \"id\" tag");
}

void TinyBootloader::XmlToData::outputData(const Data &data, QTextStream &s) const
{
  s << data.id;
}

} // namespace

//-----------------------------------------------------------------------------
XML_MAIN(TinyBootloader::XmlToData)
