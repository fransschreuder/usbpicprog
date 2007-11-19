/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "device_info.h"

#include "devices/base/generic_device.h"
#include "progs/list/prog_list.h"
#include "tool_list.h"

QString Device::webpageHtml(const Device::Data &data)
{
  const Device::Documents &documents = data.documents();
  QString url = "http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&amp;nodeId=";
  if ( documents.webpage.isEmpty() ) url += "2044&amp;AllWords=" + data.name();
  else url += "1335&amp;dDocName=en" + documents.webpage;
  return "<a href=\"" + url + "\">" + i18n("Device Page") + "</a> ";
}

QString documentHtml(const QString &document, const QString &label)
{
  if ( document.isEmpty() ) return QString::null;
  return ", <a href=\"document://" + document + "\">" + label + "</a>";
}

QString Device::documentHtml(const Device::Data &data)
{
  QString s = webpageHtml(data);
  s += documentHtml(data.documents().datasheet, i18n("Datasheet"));
  s += documentHtml(data.documents().progsheet, i18n("Programming Specifications"));
  return htmlTableRow(i18n("Documents"), s);
}

QString Device::supportedHtmlInfo(const Device::Data &data)
{
  QString doc;
  doc += "<hr />\n";
  doc += "<table>\n";
  QString tools;
  Tool::Lister::ConstIterator tit;
  for (tit=Tool::lister().begin(); tit!=Tool::lister().end(); ++tit) {
    if ( !tit.data()->isSupported(data.name()) ) continue;
    if ( tit.data()->isCustom() ) continue;
    if ( !tools.isEmpty() ) tools += ", ";
    tools += tit.data()->label();
  }
  doc += htmlTableRow(i18n("Tools"), tools) + "\n";
  QString progs;
  Programmer::Lister::ConstIterator pit;
  for (pit=Programmer::lister().begin(); pit!=Programmer::lister().end(); ++pit) {
    if ( !pit.data()->isSupported(data.name()) ) continue;
    if ( !progs.isEmpty() ) progs += ", ";
    progs += pit.data()->label();
  }
  doc += htmlTableRow(i18n("Programmers"), progs) + "\n";
  doc += "</table>\n";

  return doc;
}
