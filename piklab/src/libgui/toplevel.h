/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TOPLEVEL_H
#define TOPLEVEL_H

#include <qlabel.h>
#include <qprogressbar.h>
#include <kdockwidget.h>

#include "config_center.h"
#include "tools/list/compile.h"
#include "toplevel_ui.h"
namespace Programmer { class Group; }
namespace Tool { class Group; }
class ConfigGenerator;
class HexEditor;
namespace Process { class Base; }

class MainWindow : public KDockMainWindow
{
Q_OBJECT
public:
  MainWindow();
  virtual ~MainWindow();
  void setDevice(const QString &device);

public slots:
  void newSourceFile();
  void newProject();
  void openProject();
  void buildProject();
  void cleanBuild();
  void configure(ConfigCenter::Type showType = ConfigCenter::General);
  void configureProject();
  void showDeviceInfo();
  void runPikloops();
  void runKfind();
  void configGenerator();
  void templateGenerator();
  void updateGUI();

private slots:
  void newHexFile();
  bool openFile();
  void openRecentFile(const KURL &url);

  void compileFile();
  void compileSuccess();
  void compileFailure();
  void updateFile(const Compile::FileData &data);
  void stopBuild();

  void erase();
  void blankCheck();
  void program();
  void verify();
  void read();
  void setTotalSteps(uint total);
  void addToTotalSteps(uint add);
  void addProgress(uint steps);

  void applyToolbarSettings();
  void configureToolbar();
  void configureToolchains();
  void configureProgrammer() { configure(ConfigCenter::ProgSelect); }
  void selectProgrammer(const Programmer::Group &group);
  void selectTool(const Tool::Group &group);

  void updateToolViewsActions();
  void resetDockLayout();
  void toggleToolView(QWidget *widget);

  void pikloopsDone();
  void kfindDone();

  void openRecentProject(const KURL &url);
  void closeProject();

  void initialLoading();

signals:
  void stateChanged();

private:
  Log::Widget            *_programLog;
  QLabel                 *_actionStatus, *_debugStatus, *_editorStatus;
  ProgrammerStatusWidget *_programmerStatus;
  ToolStatusWidget       *_toolStatus;
  QProgressBar           *_actionProgress;
  ConfigGenerator        *_configGenerator;
  ::Process::Base        *_pikloopsProcess, *_kfindProcess;
  bool _forceProgramAfterBuild;

  class DockPosition {
  public:
    DockPosition() {}
    DockPosition(const QString &pparent) : parent(pparent), pos(KDockWidget::DockCenter), space(0) {}
    DockPosition(KDockWidget::DockPosition ppos, uint pspace) : parent("main_dock_widget"), pos(ppos), space(pspace) {}
    QString parent;
    KDockWidget::DockPosition pos;
    uint space;
  };
  class DockData {
  public:
    ViewMenuAction *action;
    KDockWidget    *dock;
    QString         title;
    DockPosition    position;
  };
  QValueList<DockData> _docks;
  KDockWidget         *_mainDock;

  void saveSettings();
  HexEditor *getHexEditor();
  virtual bool queryClose();
  void stopOperations();
  void cleanBuild(bool singleFile);
  virtual void keyPressEvent(QKeyEvent *e);
  void readDockConfig();
  KDockWidget *createDock(const QString &name, const QPixmap &icon, const QString &title, const DockPosition &position);
  void initDockPosition(const DockData &ddata);
};

#endif
