/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DEVICE_GUI_H
#define DEVICE_GUI_H

#include <qpushbutton.h>
#include <qlayout.h>
#include <qcombobox.h>
class QListViewItem;
class QCheckBox;
#include <ktextbrowser.h>
class KListView;

#include "common/gui/misc_gui.h"
#include "common/gui/dialog.h"
#include "device_editor.h"
#include "devices/base/generic_device.h"
namespace Programmer { class Group; }
namespace Tool { class Group; }

namespace DeviceChooser
{
  enum Type { Choose, ChooseWithAuto };
  class View;
  extern QString htmlInfo(const Device::Data &data, const QString &deviceHref,
                          const QString &imagePrefix, QMimeSourceFactory *msf);

//-----------------------------------------------------------------------------
class Dialog : public ::Dialog
{
Q_OBJECT
public:
  Dialog(const QString &device, Type type, QWidget *parent);
  virtual ~Dialog();

  enum ListType { FamilyTree = 0, Flat, Nb_ListTypes };
  QString device() const;

private slots:
  void listDoubleClicked(QListViewItem *item);
  void currentChanged(QListViewItem *item);
  void deviceChange(const QString &device);
  void updateList();
  void resetFilters();

private:
  bool _withAuto;
  static const char * const LIST_TYPE_LABELS[Nb_ListTypes];
  QComboBox  *_listTypeCombo, *_programmerCombo, *_toolCombo, *_memoryCombo;
  KListView  *_listView;
  View       *_deviceView;

  void updateList(const QString &device);
  const Programmer::Group *programmerGroup() const;
  const Tool::Group *toolGroup() const;
  Device::MemoryTechnology memoryTechnology() const;
};

//-----------------------------------------------------------------------------
class ComboBox : public QComboBox
{
Q_OBJECT
public:
  ComboBox(bool withAuto, QWidget *parent);
  void setDevice(const QString &device, const Device::Data *data = 0);
  QString device() const;
  bool withAuto() const { return _withAuto; }

private:
  bool _withAuto;
};

//-----------------------------------------------------------------------------
class Button : public QWidget
{
Q_OBJECT
public:
  Button(bool withAuto, QWidget *parent);
  void setDevice(const QString &device) { _combo->setDevice(device); }
  QString device() const { return _combo->device(); }

signals:
  void changed();

private slots:
  void chooseDevice();

private:
  ComboBox *_combo;
};

//-----------------------------------------------------------------------------
class Browser : public KTextBrowser
{
Q_OBJECT
public:
  Browser(QWidget *parent);

signals:
  void deviceChanged(const QString &device);

public slots:
  virtual void setSource(const QString &name);
};

//-----------------------------------------------------------------------------
class View : public QWidget
{
Q_OBJECT
public:
  View(QWidget *parent);
  virtual ~View();

  void clear();
  void setText(const QString &text);
  void setDevice(const QString &name);

signals:
  void deviceChanged(const QString &device);

private:
  QMimeSourceFactory *_msf;
  Browser *_browser;
};

//-----------------------------------------------------------------------------
class Editor : public DeviceEditor
{
Q_OBJECT
public:
  Editor(const QString &title, const QString &tag, QWidget *parent);
  virtual bool isModified() const { return false; }
  virtual bool isReadOnly() const { return true; }
  virtual void addGui() {}
  virtual void removeGui() {}
  virtual void setFocus() {}

signals:
  void deviceChanged(const QString &device);

private:
  virtual QWidget *createView(const Device::Data *data, QWidget *parent);
  virtual void setModifiedInternal(bool) {}
  virtual void setReadOnlyInternal(bool) {}
};

} // namespace

#endif
