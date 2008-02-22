/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROJECT_MANAGER_H
#define PROJECT_MANAGER_H

#include "common/global/purl.h"
#include "common/gui/misc_gui.h"
#include "common/gui/list_view.h"
#include "project_manager_ui.h"
class Project;
class Editor;
namespace Coff { class SectionParser; }

namespace ProjectManager
{

class View : public ListView
{
Q_OBJECT
public:
  View(QWidget *parent);
  virtual ~View();

  bool editProject();
  bool newProject();
  bool openProject();
  bool openProject(const PURL::Url &url);
  void closeProject();
  Project *project() const { return _project; }

  void setStandalone(const PURL::Url &url, PURL::FileType type);
  PURL::Url projectUrl() const { return _rootItem->url(); }
  PURL::Url standaloneGenerator(const PURL::Url &url, PURL::FileType &type) const;
  PURL::Url linkerScriptUrl() const;
  PURL::Url selectedUrl() const;
  void removeFile(const PURL::Url &url);
  void select(const Editor *e);
  void insertFile(const PURL::Url &url);
  bool contains(const PURL::Url &url) const { return findFileItem(url); }
  void addExternalFile(const PURL::Url &url, FileOrigin fileOrigin);
  bool isExternalFile(const PURL::Url &url) const;
  void removeExternalFiles();

  bool isModified() const { return _modified; }
  void setModified(bool modified) { _modified = modified; }
  bool needsRecompile() const;

public slots:
  void insertSourceFiles();
  void insertObjectFiles();
  void insertCurrentFile();
  void updateGUI();

private slots:
  void init();
  void contextMenu(QListViewItem *item, const QPoint &pos, int column);
  void clicked(int button, QListViewItem *item, const QPoint &pos, int column);
  void renamed(QListViewItem *item, int column, const QString &text);
  void modified(const PURL::Url &url);
  void filesReordered();

signals:
  void guiChanged();

private:
  Project          *_project;
  RootItem         *_rootItem;
  DeviceItem       *_deviceItem;
  LinkerScriptItem *_linkerScriptItem;
  class ProjectData {
  public:
    PURL::FileType type;
    QMap<PURL::Url, FileOrigin> externals;
  };
  QMap<PURL::Url, ProjectData> _standaloneData;
  ProjectData _projectData;
  bool _modified;

  HeaderItem *findHeaderItem(Group group) const;
  HeaderItem *headerItem(Group group);
  FileItem *findFileItem(const PURL::Url &url) const;
  FileItem *findFileItem(PURL::FileType type) const;
  QListViewItem *findItem(const QString &name) const;
  virtual QDragObject *dragObject();
  virtual bool acceptDrag(QDropEvent* e) const;
  virtual QString tooltip(QListViewItem *item, int col) const;
  void addExternalFiles();
  void rightClicked(QListViewItem *item, const QPoint &pos);
  void leftClicked(QListViewItem *item);
  void addFile(const PURL::Url &url, PURL::FileType type, FileOrigin origin);
  void setProject(Project *project);
  void initListView();
  ProjectData &projectData();
  const ProjectData &projectData() const;
};

} // namespace

#endif
