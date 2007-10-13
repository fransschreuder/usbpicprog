/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include <qfile.h>
#include <qtextstream.h>

#include "xml_to_data/device_xml_to_data.h"
#include "common/common/misc.h"
#include "devices/mem24/base/mem24.h"

namespace Mem24
{

class XmlToData : public Device::XmlToData<Data>
{
private:
  virtual uint nbOutputFiles(uint) const { return 1; }
  virtual bool isIncluded(uint, uint) const { return true; }
  virtual QString namespaceName() const { return "Mem24"; }

virtual void processDevice(QDomElement device)
{
  Device::XmlToDataBase::processDevice(device);

  QDomElement e = findUniqueElement(device, "memory", "name", QString::null);
  bool ok;
  data()->_nbBytes = fromHexLabel(e.attribute("size"), &ok);
  if ( !ok ) qFatal("Missing or invalid size");
  data()->_nbBlocks = e.attribute("nb_blocks").toUInt(&ok);
  if ( !ok || data()->_nbBlocks==0 ) qFatal("Missing, zero, or invalid nb_blocks");
  if ( (data()->_nbBytes % data()->_nbBlocks)!=0 ) qFatal("nb_blocks should divide size");
  if ( data()->_nbBlocks>8 ) qFatal("nb_blocks is too large (>8)");
  data()->_nbBytesPage = e.attribute("page_size").toUInt(&ok);
  if ( !ok || data()->_nbBytesPage==0 ) qFatal("Missing, zero, or invalid page_size");
  if ( ((data()->_nbBytes/data()->_nbBlocks) % data()->_nbBytesPage)!=0 ) qFatal("page_size should divide size/nb_blocks");
  QStringList names;
  names.append(QString::null);
  checkTagNames(device, "memory", names);
}

virtual void checkPins(const QMap<QString, uint> &pinLabels) const
{
  if ( !pinLabels.contains("VCC") ) qFatal("No VDD pin specified");
  if ( !pinLabels.contains("VSS") ) qFatal("No VSS pin specified");
  QMap<QString, uint>::const_iterator it;
  for (it=pinLabels.begin(); it!=pinLabels.end(); ++it)
    if ( it.data()!=1 ) qFatal(QString("Duplicated pin %1").arg(it.key()));
}

}; // class

} // namespace

//-----------------------------------------------------------------------------
XML_MAIN(Mem24::XmlToData)
