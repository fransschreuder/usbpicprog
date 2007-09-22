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

#include "tools/base/tool_config_widget.h"
#include "common/gui/dialog.h"
namespace DeviceChooser { class Button; }
class SelectDirectoryWidget;
class ToolsConfigWidget;

//----------------------------------------------------------------------------
class GeneralConfigWidget : public GridConfigWidget
{
  Q_OBJECT
public:
  GeneralConfigWidget();
  virtual void loadConfig();

  virtual QString title() const { return i18n("General"); }
  virtual QString header() const { return i18n("General Configuration"); }
  virtual QPixmap pixmap() const;

public slots:
  virtual void saveConfig();

private:
  QCheckBox *_programAfterBuild, *_userIdSetToChecksum;
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
  enum Type { General = 0, ProgSelect, ProgOptions, Standalone, Nb_Types };
  ConfigCenter(Type showType, QWidget *parent);

public slots:
  virtual void slotOk();
  virtual void slotApply();

private:
  QWidget      *_pages[Nb_Types];
  ConfigWidget *_configWidgets[Nb_Types];

  static ConfigWidget *factory(Type type);
};

//----------------------------------------------------------------------------
class ToolchainsConfigCenter : public TreeListDialog
{
Q_OBJECT
public:
  ToolchainsConfigCenter(const Tool::Group &group, QWidget *parent);

public slots:
  virtual void slotOk();
  virtual void slotApply();
  virtual void slotUser1();
  virtual void slotUser2();

private slots:
  void aboutToShowPageSlot(QWidget *page);

private:
  QMap<QWidget *, ToolchainConfigWidget *> _pages;
};

#endif
