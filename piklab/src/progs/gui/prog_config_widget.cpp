/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "prog_config_widget.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <kurlrequester.h>

#include "progs/base/prog_config.h"
#include "progs/base/prog_group.h"

Programmer::ConfigWidget::ConfigWidget(const Group &group, QWidget *parent, const char *name)
  : GridConfigWidget(parent, name), _group(group)
{
  if ( group.properties() & NeedDeviceSpecificFirmware ) {
    uint row = _topLayout->numRows();
    QLabel *label = new QLabel(i18n("Firmware directory:"), this);
    _topLayout->addWidget(label, row, 0);
    _firmwareDir = new KURLRequester(this);
    _firmwareDir->setMode(KFile::Directory | KFile::ExistingOnly);
    _topLayout->addWidget(_firmwareDir, row, 1);
  } else _firmwareDir = 0;
}

void Programmer::ConfigWidget::loadConfig()
{
  GroupConfig config(_group);
  if (_firmwareDir) _firmwareDir->setURL(config.firmwareDirectory());
}

void Programmer::ConfigWidget::saveConfig()
{
  GroupConfig config(_group);
  if (_firmwareDir) config.writeFirmwareDirectory(_firmwareDir->url());
}

bool Programmer::ConfigWidget::setPort(const HardwareDescription &hd)
{
  return _group.checkConnection(hd);
}
