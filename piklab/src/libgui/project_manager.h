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

#include <qtable.h>
#include <kpopupmenu.h>

#include "common/global/purl.h"
#include "common/gui/misc_gui.h"
#include "common/gui/list_view.h"
class Project;
class Editor;
namespace Coff { class SectionParser; }

//-----------------------------------------------------------------------------
class PopupMenu : public KPopupMenu
{
Q_OBJECT
public:
  PopupMenu() : _index(1) {}
  void insertItem(const QString &icon, const QString &label, QObject *receiver = 0, const char *slot = 0);

private:
  uint _index; 
};

//-----------------------------------------------------------------------------
namespace ProjectManager
{
  enum Rtti { RootRtti = 1000, HeaderRtti, FileRtti, DeviceRtti, RegisterRtti, LinkerScriptRtti };
  enum Group { DeviceGroup = 0, HeaderGroup, LinkerScriptGroup, SourceGroup,
               LinkerObjectGroup, ViewGroup, GeneratedGroup, IncludedGroup, Nb_Groups };
  extern Group group(PURL::FileType type);
  enum FileOrigin { Intrinsic, Generated, Included };

class UrlItem : public KListViewItem
{
public:
  UrlItem(KListView *listview) : KListViewItem(listview) {}
  UrlItem(KListViewItem *item) : KListViewItem(item) {}
  virtual PURL::Url url() const { return _url; }

protected:
  PURL::Url _url;
};

class RootItem : public UrlItem
{
public:
  RootItem(KListView *listview);
  void set(const PURL::Url &url, bool standAlone);
  virtual int rtti() const { return RootRtti; }

private:
  bool _standalone;
};

class HeaderItem : public KListViewItem
{
public:
  HeaderItem(RootItem *item, Group group);
  virtual int rtti() const { return HeaderRtti; }
  Group group() const { return _group; }
  QListViewItem *lastChild() const;

private:
  static const char *GROUP_LABELS[Nb_Groups];
  Group _group;
};

class FileItem : public UrlItem
{
public:
  FileItem(HeaderItem *item, PURL::FileType type, const PURL::Url &url, bool external);
  virtual int rtti() const { return FileRtti; }
  PURL::FileType ftype() const { return _ftype; }

private:
  PURL::FileType _ftype;
  bool           _external;

  void set(const PURL::Url &url);
  virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align);
};

class RegisterItem : public KListViewItem
{
public:
  RegisterItem(HeaderItem *item);
  virtual int rtti() const { return RegisterRtti; }
};

class DeviceItem : public EditListViewItem
{
public:
  DeviceItem(HeaderItem *item);
  void updateText();
  virtual int rtti() const { return DeviceRtti; }

private:
  virtual QWidget *editWidgetFactory(int) const;
  virtual bool alwaysAcceptEdit(int) const { return true; }
};

class LinkerScriptItem : public UrlItem
{
public:
  LinkerScriptItem(HeaderItem *item);
  void set(const PURL::Url &url);
  virtual PURL::Url url() const;
  void init();
  virtual int rtti() const { return LinkerScriptRtti; }
};

//-----------------------------------------------------------------------------
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
