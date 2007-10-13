/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "xml_to_data/prog_xml_to_data.h"
#include "devices/pic/base/pic.h"
#include "progs/direct/base/direct_data.h"

namespace Direct
{
class XmlToData : public ::Programmer::XmlToData<Data>
{
public:
  XmlToData() : ::Programmer::XmlToData<Data>("direct", "Direct") {}

private:
  virtual void parseData(QDomElement element, Data &data);
  virtual void includes(QTextStream &s) const;
  virtual void outputFunctions(QTextStream &s) const;
};

void Direct::XmlToData::parseData(QDomElement element, Data &)
{
  QString s = element.attribute("pwidth");
  const Device::Data *d = Device::lister().data(currentDevice());
  if ( d==0 ) qFatal("Invalid device name");
}

void Direct::XmlToData::includes(QTextStream &s) const
{
  Programmer::XmlToData<Data>::includes(s);
  s << "#include \"direct_baseline.h\"" << endl;
  s << "#include \"direct_16F.h\"" << endl;
  s << "#include \"direct_18F.h\"" << endl;
}

void Direct::XmlToData::outputFunctions(QTextStream &s) const
{
  Programmer::XmlToData<Data>::outputFunctions(s);
  s << "::Programmer::DeviceSpecific *Direct::Group::createDeviceSpecific(::Programmer::Base &base) const" << endl;
  s << "{" << endl;
  s << "  uint i = family(static_cast<PicBase &>(base).device()->name());" << endl;
  s << "  switch(i) {" << endl;
  for (uint i=0; i<uint(families().count()); i++) {
    s << "    case " + QString::number(i) + ": return new P" + families()[i] + "(base);" << endl;
  }
  s << "  }" << endl;
  s << "  Q_ASSERT(false);" << endl;
  s << "  return 0;" << endl;
  s << "}" << endl;
}

} // namespace

//-----------------------------------------------------------------------------
XML_MAIN(Direct::XmlToData)
