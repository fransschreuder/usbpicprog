/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "mem24.h"

QString Mem24::Data::informationUrl() const
{
  QString url = "http://microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=";
  if ( document().isEmpty() )
   url += "2044&AllWords=" + name();
  else url += "1335&dDocName=en" + document();
  return url;
}

QString Mem24::Data::informationHtml() const
{
  QString tmp = i18n("%1 bytes").arg(formatNumber(nbBytes()));
  return htmlTableRow(i18n("Memory Size"), tmp);
}

QDataStream &Mem24::operator <<(QDataStream &s, const Data &data)
{
  s << static_cast<const Device::Data &>(data);
  s << data._nbBytes << data._nbBlocks << data._nbBytesPage;
  return s;
}
QDataStream &Mem24::operator >>(QDataStream &s, Data &data)
{
  s >> static_cast<Device::Data &>(data);
  s >> data._nbBytes >> data._nbBlocks >> data._nbBytesPage;
  return s;
}
