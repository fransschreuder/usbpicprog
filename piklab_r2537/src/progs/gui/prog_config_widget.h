/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROG_CONFIG_WIDGET
#define PROG_CONFIG_WIDGET

class QCheckBox;
class KURLRequester;

#include "common/gui/config_widget.h"
#include "common/port/port.h"

namespace Programmer
{
class Group;
class HardwareDescription;

class ConfigWidget: public GridConfigWidget
{
Q_OBJECT
public:
  ConfigWidget(const Group &group, QWidget *parent, const char *name);
  virtual void loadConfig();
  const Group &group() const { return _group; }
  virtual void saveConfig();
  virtual bool setPort(const HardwareDescription &hd);

signals:
  void updatePortStatus(bool ok);

protected:
  const Group &_group;

private:
  KURLRequester *_firmwareDir;
};

} // namespace

#endif
