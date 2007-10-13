/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2004 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TOOLS_CONFIG_WIDGET_H
#define TOOLS_CONFIG_WIDGET_H

#include <qcombobox.h>
#include <qtabwidget.h>
#include <qvaluevector.h>

#include "progs/gui/config_widget.h"
#include "common/gui/dialog.h"
#include "common/gui/key_gui.h"
class Project;
class ToolConfigWidget;

//----------------------------------------------------------------------------
class HelpDialog : public Dialog
{
Q_OBJECT
public:
  HelpDialog(QWidget *parent);
};

//----------------------------------------------------------------------------
class ToolsConfigWidget : public GridConfigWidget
{
Q_OBJECT
public:
  ToolsConfigWidget(Project *project, QWidget *parent);
  virtual void loadConfig();

public slots:
  virtual void saveConfig();
  virtual void displayHelp();

private slots:
  void toolChanged();

private:
  Project    *_project;
  KeyComboBox  *_tool;
  QTabWidget *_tabWidget;
  QValueVector<KeyWidgetStack *> _stacks;
  QGuardedPtr<Dialog> _helpDialog;
};

#endif
