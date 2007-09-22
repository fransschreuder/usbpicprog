/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "device_info.h"

#include <klocale.h>

#include "devices/base/generic_device.h"
#include "progs/list/prog_list.h"
#include "tool_list.h"

QString Device::supportedHtmlInfo(const Device::Data &data)
{
  QString doc;
  doc += "<hr />\n";
  doc += "<table>\n";
  QString tools;
  for (uint i=0; i<Tool::lister().nbGroups(); i++) {
    if ( !Tool::lister().group(i).isSupported(data.name()) ) continue;
    if ( !tools.isEmpty() ) tools += ", ";
    tools += Tool::lister().group(i).label();
  }
  doc += htmlTableRow(i18n("Tools"), tools) + "\n";
  QString progs;
  for (uint i=0; i<Programmer::lister().nbGroups(); i++) {
    if ( !Programmer::lister().group(i).isSupported(data.name()) ) continue;
    if ( !progs.isEmpty() ) progs += ", ";
    progs += Programmer::lister().group(i).label();
  }
  doc += htmlTableRow(i18n("Programmers"), progs) + "\n";
  doc += "</table>\n";

  return doc;
}
