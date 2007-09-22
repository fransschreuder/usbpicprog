/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "main_global.h"

#include <qpopupmenu.h>

#include "toplevel.h"
#include "common/global/about.h"
#include "global_config.h"
#include "devices/list/device_list.h"
#include "project_manager.h"
#include "editor_manager.h"
#include "tools/list/compile_config.h"
#include "gui_prog_manager.h"

Main::State Main::_state = Main::Idle;
MainWindow *Main::_toplevel = 0;
EditorManager *Main::_editorManager = 0;
ProjectManager::View *Main::_projectManager = 0;
Breakpoint::View *Main::_breakpointsView = 0;
Register::WatchView *Main::_watchView = 0;
Compile::LogWidget *Main::_compileLog = 0;
Compile::Manager *Main::_compileManager = 0;
ConsoleView *Main::_consoleView = 0;

void Main::setState(State state)
{
  _state = state;
  _toplevel->updateGUI();
}

Programmer::Base *Main::programmer()
{
  return Programmer::manager->programmer();
}

const Programmer::Group &Main::programmerGroup()
{
  GlobalConfig gc;
  return gc.programmerGroup();
}

KAction *Main::action(const char* name)
{
  return _toplevel->KMainWindow::action(name);
}
KAction *Main::action(KStdAction::StdAction action)
{
  return _toplevel->KMainWindow::action(KStdAction::name(action));
}

QPopupMenu &Main::popup(const char *name)
{
  QPopupMenu *popup = static_cast<QPopupMenu *>(_toplevel->factory()->container(name, _toplevel));
  Q_ASSERT(popup);
  return *popup;
}

const Device::Data *Main::deviceData()
{
  QString name = device();
  if ( name==Device::AUTO_DATA.name )
    name = DeviceEditor::guessDeviceFromFile(_projectManager->projectUrl());
  return Device::lister().data(name);
}

Editor *Main::currentEditor() { return _editorManager->currentEditor(); }
Project *Main::project() { return _projectManager->project(); }
QString Main::device() { return Compile::Config::device(project()); }
const Tool::Group &Main::toolGroup() { return Compile::Config::toolGroup(project()); }
