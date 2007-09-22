/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "gpsim_group_ui.h"

#include <qtimer.h>

#include "progs/base/prog_group.h"
#include "progs/gpsim/gpsim.h"

//----------------------------------------------------------------------------
GPSim::ConfigWidget::ConfigWidget(const ::Programmer::Group &group, QWidget *parent, const char *name)
  : ::Programmer::ConfigWidget(group, parent, name)
{
  uint row = _topLayout->numRows();
  QLabel *label = new QLabel(i18n("Status:"), this);
  _topLayout->addWidget(label, row, 0);
  _status = new QLabel(this);
  _topLayout->addWidget(_status, row, 1);
  _topLayout->setColStretch(2, 1);

  QTimer::singleShot(0, this, SLOT(updateStatus()));
}

void GPSim::ConfigWidget::updateStatus()
{
  VersionData version;
  ProcessManager manager(0);
  if ( !manager.start() ) _status->setText(i18n("Could not start \"gpsim\""));
  else if ( !manager.getVersion(version) || !version.isValid() ) _status->setText(i18n("Could not detect \"gpsim\" version"));
  else _status->setText(i18n("Found version \"%1\"").arg(version.pretty()));
}

//----------------------------------------------------------------------------
::Programmer::ConfigWidget *GPSim::GroupUI::createConfigWidget(QWidget *parent) const
{
  return new ConfigWidget(static_cast<const ::Programmer::Group &>(group()), parent, "gpsim_config_widget");
}
