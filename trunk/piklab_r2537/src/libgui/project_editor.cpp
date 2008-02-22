/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "project_editor.h"

#include <qlabel.h>
#include <qlayout.h>
#include <klocale.h>

#include "project.h"
#include "tools/list/compile_config.h"
#include "device_gui.h"

ProjectEditor::ProjectEditor(Project &project, QWidget *parent)
  : Dialog(parent, "project_options", true, i18n("Project Options"), Ok|Cancel, Ok, false),
    _project(project)
{
  QGridLayout *top = new QGridLayout(mainWidget(), 0, 0, 10, 10);

// global
  QLabel *label = new QLabel(i18n("Description"), mainWidget());
  top->addWidget(label, 0, 0);
  _description = new QTextEdit(mainWidget());
  _description->setText(_project.description());
  top->addMultiCellWidget(_description, 0,0, 1,2);
  label = new QLabel(i18n("Version"), mainWidget());
  top->addWidget(label, 1, 0);
  _version = new QLineEdit(mainWidget());
  _version->setText(_project.version());
  top->addWidget(_version, 1, 1);
  label = new QLabel(i18n("Device"), mainWidget());
  top->addWidget(label, 2, 0);
  _device = new DeviceChooser::Button(false, mainWidget());
  _device->setDevice(Compile::Config::device(&_project));
  top->addWidget(_device, 2, 1);

  _tools = new ToolsConfigWidget(&project, mainWidget());
  _tools->loadConfig();
  top->addMultiCellWidget(_tools, 3,3, 0,2);

  top->setRowStretch(3, 1);
  top->setColStretch(2, 1);
}

void ProjectEditor::slotOk()
{
  _project.setDescription(_description->text());
  _project.setVersion(_version->text());
  Compile::Config::setDevice(&_project, _device->device());
  _tools->saveConfig();
  accept();
}
