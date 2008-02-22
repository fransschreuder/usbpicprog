/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROJECT_MANAGER_UI_H
#define PROJECT_MANAGER_UI_H

#include <kpopupmenu.h>

#include "common/global/purl.h"
#include "common/gui/misc_gui.h"
#include "common/gui/list_view.h"

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

} // namespace

#endif
