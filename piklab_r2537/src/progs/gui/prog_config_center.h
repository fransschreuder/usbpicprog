/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROG_CONFIG_CENTER_H
#define PROG_CONFIG_CENTER_H

#include <qcheckbox.h>

#include "common/gui/config_widget.h"
#include "common/gui/key_gui.h"
#include "progs/base/prog_config.h"
class Container;
class PortSelector;

namespace Programmer
{
BEGIN_DECLARE_CONFIG_WIDGET(Config, OptionsConfigWidget)
  virtual QString header() const { return i18n("Programmer Options"); }
  virtual QString title() const { return i18n("Programmer Options"); }
  virtual QPixmap pixmap() const;
END_DECLARE_CONFIG_WIDGET

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
