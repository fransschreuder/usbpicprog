/***************************************************************************
                          ProjectEditorWid.cpp  -  description
                             -------------------
    begin                : lun déc 8 2003
    copyright            : (C) 2003 by Alain Gibaud
    email                : alain.gibaud@univ-valenciennes.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef PROJECT_EDITOR_H
#define PROJECT_EDITOR_H

#include <qtextedit.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qwidgetstack.h>

#include "tools/list/tools_config_widget.h"
#include "common/gui/misc_gui.h"
class Project;
namespace DeviceChooser { class Button; }

class ProjectEditor : public Dialog
{
Q_OBJECT
public:
  ProjectEditor(Project &project, QWidget *parent);

private slots:
  virtual void slotOk();

private:
  Project &_project;
  QTextEdit *_description;
  QLineEdit *_version;
  DeviceChooser::Button *_device;
  ToolsConfigWidget *_tools;
};

#endif
