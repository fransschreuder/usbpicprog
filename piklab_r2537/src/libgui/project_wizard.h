/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROJECT_WIZARD_H
#define PROJECT_WIZARD_H

#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <kwizard.h>
#include <klineedit.h>
#include <kcombobox.h>

#include "common/global/purl.h"
#include "common/gui/editlistbox.h"
#include "common/gui/key_gui.h"
namespace PURL { class DirectoryWidget; }
namespace DeviceChooser { class Button; }
namespace Tool { class Group; }
class Project;

//-----------------------------------------------------------------------------
class FileListItem : public KListViewItem
{
public:
  FileListItem(KListView *view);
  void setToolGroup(const Tool::Group &group) { _group = &group; }
  bool copy() const { return _copy; }
  void toggle();
  PURL::FileGroup fileGroup() const;
  virtual const QPixmap *pixmap(int column) const;

private:
  const Tool::Group *_group;
  QPixmap _pixmap;
  bool    _copy;
};

class FileListBox : public EditListBox
{
Q_OBJECT
public:
  FileListBox(QWidget *parent);
  void setDirectory(const PURL::Directory &directory) { _directory = directory; }
  void setToolGroup(const Tool::Group &group);

protected slots:
  virtual void addItem();
  virtual void clicked(QListViewItem *item, const QPoint &point, int column);

private:
  PURL::Directory _directory;

  virtual uint textColumn() const { return 1; }
  virtual QListViewItem *createItem();
};

//-----------------------------------------------------------------------------
class ProjectWizard : public KWizard
{
Q_OBJECT
public:
  ProjectWizard(QWidget *parent);
  PURL::Url url() const;
  Project *project() const { return _project; }

protected slots:
  void buttonClicked(int id);
  virtual void next();
  virtual void done(int r);

private:
  QWidget   *_first, *_second, *_third;
  KLineEdit *_name;
  PURL::DirectoryWidget *_directory;
  DeviceChooser::Button *_device;
  KeyComboBox   *_toolchain;
  QButtonGroup  *_bgroup;
  QRadioButton  *_templateButton, *_addButton;
  FileListBox   *_files;
  Project       *_project;

  QString device() const;
  const Tool::Group &toolchain() const;
};

#endif
