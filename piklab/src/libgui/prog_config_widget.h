/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROGRAMMER_CONFIG_WIDGET_H
#define PROGRAMMER_CONFIG_WIDGET_H

class QCheckBox;
#include <klocale.h>

#include "progs/gui/config_widget.h"
#include "common/gui/key_gui.h"
class Container;
class PortSelector;

namespace Programmer
{
class ConfigWidget;

//----------------------------------------------------------------------------
class OptionsConfigWidget : public ::ConfigWidget
{
Q_OBJECT
public:
  OptionsConfigWidget();
  virtual void loadConfig();
  virtual QString header() const { return i18n("Programmer Options"); }
  virtual QString title() const { return i18n("Programmer Options"); }
  virtual QPixmap pixmap() const;

public slots:
  virtual void saveConfig();

private:
  QCheckBox *_programAll, *_powerDown, *_targetSelfPowered, *_verifyProgrammed,
            *_blankCheckAfterErase, *_preserveEeprom, *_programEeprom;
};

//----------------------------------------------------------------------------
class SelectConfigWidget : public ::ConfigWidget
{
Q_OBJECT
public:
  SelectConfigWidget();
  virtual void loadConfig() {}
  virtual QString header() const { return i18n("Programmer Selection"); }
  virtual QString title() const { return i18n("Programmer Selection"); }
  virtual QPixmap pixmap() const;

public slots:
  virtual void saveConfig();

private slots:
  void programmerChanged();
  void portChanged();

private:
  KeyComboBox    *_combo;
  PortSelector   *_portSelector;
  KeyWidgetStack *_stack;
  Container      *_portSelectorContainer;
  QTabWidget     *_tabWidget;
};

} // namespace

#endif
