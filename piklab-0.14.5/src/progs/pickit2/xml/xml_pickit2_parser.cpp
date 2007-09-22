/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "xml_to_data/prog_xml_to_data.h"
#include "progs/pickit2/base/pickit2_data.h"

namespace Pickit2
{

class XmlToData : public Programmer::XmlToData<Data>
{
public:
  XmlToData() : Programmer::XmlToData<Data>("pickit2", "Pickit2") {}

private:
  virtual void parseData(QDomElement element, Data &data);
  virtual void outputData(const Data &data, QTextStream &s) const;
  virtual void outputFunctions(QTextStream &s) const;
};

void Pickit2::XmlToData::parseData(QDomElement element, Data &data)
{
  QString s = element.attribute("entry");
  if ( s.length()!=1 || (s[0]!='O' && s[0]!='P') ) qFatal("Invalid or missing entry mode");
  data.entryMode = s[0].latin1();
  s = element.attribute("pmode");
  if ( s.length()!=1 || (s[0]!='0' && s[0]!='1' && s[0]!='4' && s[0]!='n') )
    qFatal("Invalid or missing program mode");
  data.progMode = s[0].latin1();
  s = element.attribute("pwidth");
  if ( data.progMode!='n' ) {
    if ( s.length()!=0 ) qFatal("Program width should not be defined");
    data.progWidth = 0;
  } else {
    bool ok;
    data.progWidth = s.toUInt(&ok);
    if ( !ok ) qFatal("Invalid or missing program width");
  }
  s = element.attribute("regen");
  if ( s.isEmpty() || s=="false" ) data.regenerateOsccal = false;
  else if ( s=="true" ) data.regenerateOsccal = true;
  else qFatal("Invalid regen value");
}

void Pickit2::XmlToData::outputData(const Data &data, QTextStream &s) const
{
  s <<  "'" << data.entryMode << "', '" << data.progMode;
  s << "', " << data.progWidth << ", " << (data.regenerateOsccal ? "true" : "false");
}

void Pickit2::XmlToData::outputFunctions(QTextStream &s) const
{
  Programmer::XmlToData<Data>::outputFunctions(s);
  s << "::Programmer::DeviceSpecific *Group::createDeviceSpecific(::Programmer::Base &base) const" << endl;
  s << "{" << endl;
  s << "  uint i = family(static_cast< ::Pickit2::Base &>(base).device()->name());" << endl;
  s << "  switch(i) {" << endl;
  for (uint i=0; i<uint(families().count()); i++) {
    s << "    case " + QString::number(i) + ": return new " + families()[i] + "(base);" << endl;
  }
  s << "  }" << endl;
  s << "  Q_ASSERT(false);" << endl;
  s << "  return 0;" << endl;
  s << "}" << endl;
}

} // namespace

//-----------------------------------------------------------------------------
XML_MAIN(Pickit2::XmlToData)
