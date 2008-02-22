/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2004 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CONFIG_CENTER_H
#define CONFIG_CENTER_H

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qwidgetstack.h>
#include <klocale.h>

#include "common/gui/dialog.h"
#include "progs/gui/prog_config_widget.h"
#include "global_config.h"
namespace DeviceChooser { class Button; }
class SelectDirectoryWidget;
class ToolsConfigWidget;

//----------------------------------------------------------------------------
BEGIN_DECLARE_CONFIG_WIDGET(BaseGlobalConfig, BaseGlobalConfigWidget)
END_DECLARE_CONFIG_WIDGET

class GlobalConfigWidget : public BaseGlobalConfigWidget
{
Q_OBJECT
public:
  GlobalConfigWidget();
  virtual QString title() const { return i18n("General"); }
  virtual QString header() const { return i18n("General Configuration"); }
  virtual QPixmap pixmap() const;
  virtual void loadConfig();

public slots:
  virtual void saveConfig();

private:
  QComboBox *_showDebug;
};

//----------------------------------------------------------------------------
class StandaloneConfigWidget : public ConfigWidget
{
  Q_OBJECT
public:
  StandaloneConfigWidget();
  virtual void loadConfig();
  virtual QString title() const { return i18n("Standalone File"); }
  virtual QString header() const { return i18n("Standalone File Compilation"); }
  virtual QPixmap pixmap() const;

public slots:
  virtual void saveConfig();

private:
  DeviceChooser::Button *_device;
  ToolsConfigWidget     *_tools;
};

//----------------------------------------------------------------------------
class ConfigCenter : public Dialog
{
Q_OBJECT
public:
  enum Type { General = 0, ProgSelect, ProgOptions, DebugOptions,
              Standalone, Nb_Types };
  ConfigCenter(Type showType, QWidget *parent);

public slots:
  virtual void slotOk();
  virtual void slotApply();

private:
  QWidget      *_pages[Nb_Types];
  ConfigWidget *_configWidgets[Nb_Types];

  static ConfigWidget *factory(Type type);
};

#endif
