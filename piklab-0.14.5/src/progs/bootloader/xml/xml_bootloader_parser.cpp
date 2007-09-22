/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "xml_to_data/prog_xml_to_data.h"

#include "progs/bootloader/base/bootloader_data.h"
#include "devices/base/device_group.h"
#include "devices/pic/base/pic.h"

//-----------------------------------------------------------------------------
namespace Bootloader
{

class XmlToData : public Programmer::XmlToData<Data>
{
public:
  XmlToData() : Programmer::XmlToData<Data>("bootloader", "Bootloader") {}

private:
  virtual void parseData(QDomElement element, Data &data);
  virtual void outputData(const Data &data, QTextStream &s) const;
  virtual void outputFunctions(QTextStream &s) const;
};

void Bootloader::XmlToData::parseData(QDomElement element, Data &data)
{
  const Device::Data *ddata = Device::lister().data(currentDevice());
  if ( ddata->group().name()!="pic" ) qFatal("non-pic device not supported");
  const Pic::Data *pdata = static_cast<const Pic::Data *>(ddata);
  if ( !pdata->hasUSART() && !pdata->hasUSB() ) qFatal("device does not have USART or USB");
  for (uint i=0; i<Nb_Types; i++) {
    data.supported[i] = element.hasAttribute(TYPE_DATA[i].tag);
    if ( !data.supported[i] ) continue;
    bool ok;
    data.ids[i] = fromHexLabel(element.attribute(TYPE_DATA[i].tag), 2, &ok);
    if ( !ok ) qFatal(QString("Invalid tag %1").arg(TYPE_DATA[i].tag));
  }
}

void Bootloader::XmlToData::outputData(const Data &data, QTextStream &s) const
{
  s << "{ ";
  for (uint i=0; i<Nb_Types; i++) {
    if ( i!=0 ) s << ", ";
    s << data.ids[i];
  }
  s << " }, { ";
  for (uint i=0; i<Nb_Types; i++) {
    if ( i!=0 ) s << ", ";
    s << (data.supported[i] ? "true" : "false");
  }
  s << " }";
}

void Bootloader::XmlToData::outputFunctions(QTextStream &s) const
{
  s << "void Group::initSupported()" << endl;
  s << "{" << endl;
  s << "  for (uint i=0; DATA_LIST[i]; i++) {" << endl;
  s << "    if ( !DATA_LIST[i]->data.supported[_type] ) continue;" << endl;
  s << "    ::Group::DeviceData data;" << endl;
  s << "    data.supportType = ::Group::Untested;" << endl;
  s << "    data.data = Device::lister().data(DATA_LIST[i]->name);" << endl;
  s << "    addDevice(data);" << endl;
  s << "  }" << endl;
  s << "}" << endl;
  s << endl;
}

} // namespace

//-----------------------------------------------------------------------------
XML_MAIN(Bootloader::XmlToData)
