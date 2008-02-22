/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "xml_to_data/prog_xml_to_data.h"
#include "progs/icd1/base/icd1_data.h"

//-----------------------------------------------------------------------------
namespace Icd1
{

class XmlToData : public Programmer::XmlToData<Data>
{
public:
  XmlToData() : Programmer::XmlToData<Data>("icd1", "Icd1") {}

private:
  virtual void parseData(QDomElement element, Data &data);
  virtual void outputData(const Data &data, QTextStream &s) const;
};

void Icd1::XmlToData::parseData(QDomElement element, Data &data)
{
  bool ok;
  data.part = fromHexLabel(element.attribute("family"), 2, &ok);
  if ( !ok ) qFatal("Missing or malformed family attribute");
}

void Icd1::XmlToData::outputData(const Data &data, QTextStream &s) const
{
  s << toHexLabel(data.part, 2);
}

} // namespace

//-----------------------------------------------------------------------------
XML_MAIN(Icd1::XmlToData)
