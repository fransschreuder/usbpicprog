/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BOOTLOADER_CONFIG_WIDGET
#define BOOTLOADER_CONFIG_WIDGET

#include <qcheckbox.h>
#include <kcombobox.h>
#include <knuminput.h>

#include "progs/gui/prog_group_ui.h"
#include "progs/gui/prog_config_widget.h"
#include "progs/bootloader/base/bootloader_data.h"

//-----------------------------------------------------------------------------
namespace TinyBootloader
{
class ConfigWidget: public GridConfigWidget
{
Q_OBJECT
public:
  ConfigWidget(QWidget *parent);
  virtual void loadConfig();
  virtual void saveConfig();

private:
  KIntNumInput *_timeout, *_retries;
  KComboBox    *_type, *_speed;
};
} // namespace

//-----------------------------------------------------------------------------
namespace Pickit2Bootloader
{
class ConfigWidget: public GridConfigWidget
{
Q_OBJECT
public:
  ConfigWidget(QWidget *parent) : GridConfigWidget(parent) {}
  virtual void loadConfig() {}
  virtual void saveConfig() {}
};
} // namespace

//-----------------------------------------------------------------------------
namespace PicdemBootloader
{
class ConfigWidget: public GridConfigWidget
{
Q_OBJECT
public:
  ConfigWidget(QWidget *parent);
  virtual void loadConfig();
  virtual void saveConfig();

private:
  HexWordEditor *_vendorId, *_productId;
};
} // namespace

namespace Bootloader
{
class Group;
  
//----------------------------------------------------------------------------
class GroupUI : public ::Programmer::GroupUI
{
public:
  virtual ::Programmer::ConfigWidget *createConfigWidget(QWidget *parent) const;
  virtual bool hasAdvancedDialog() const { return false; }
  virtual ::Programmer::AdvancedDialog *createAdvancedDialog(::Programmer::Base &, QWidget *) const { return 0; }
};

class ConfigWidget : public ::Programmer::ConfigWidget
{
Q_OBJECT
public:
  ConfigWidget(const Group &group, QWidget *parent);
  virtual void loadConfig();
  virtual void saveConfig();

private:
  QCheckBox      *_eeprom;
  ::ConfigWidget *_cwidget;

  ::ConfigWidget *createConfigWidget(Type type);
};

} // namespace

#endif
