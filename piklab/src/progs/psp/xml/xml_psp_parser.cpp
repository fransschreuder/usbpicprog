/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "xml_to_data/prog_xml_to_data.h"
#include "progs/psp/base/psp_data.h"

//-----------------------------------------------------------------------------
namespace Psp
{

class XmlToData : public Programmer::XmlToData<Data>
{
public:
  XmlToData() : Programmer::XmlToData<Data>("psp", "Psp") {}

private:
  virtual void parseData(QDomElement element, Data &data);
  virtual void outputData(const Data &data, QTextStream &s) const;
};

void Psp::XmlToData::parseData(QDomElement element, Data &data)
{
  bool ok;
  data.algorithm = element.attribute("algorithm").toUInt(&ok);
  if ( !ok ) qFatal("Missing or invalid algorithm");
  data.overprogram = element.attribute("overprogram").toUInt(&ok);
  if ( !ok ) qFatal("Missing or invalid overprogram");
  data.tries = element.attribute("tries").toUInt(&ok);
  if ( !ok ) qFatal("Missing or invalid tries");
}

void Psp::XmlToData::outputData(const Data &data, QTextStream &s) const
{
  s << data.algorithm << ", " << data.overprogram << ", " << data.tries;
}

} // namespace

//-----------------------------------------------------------------------------
XML_MAIN(Psp::XmlToData)
