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

#include "common/gui/key_gui.h"
#include "common/gui/dialog.h"
#include "device_editor.h"
#include "devices/pic/base/pic.h"
namespace Programmer { class Group; }
namespace Tool { class Group; }

namespace DeviceChooser
{

enum Type { Choose, ChooseWithAuto };
class View;

BEGIN_DECLARE_ENUM(ListType)
  FamilyTree = 0, Flat
END_DECLARE_ENUM(ListType, EnumData)

//-----------------------------------------------------------------------------
class Config : public GenericConfig
{
public:
  Config() : GenericConfig("device_chooser") {}
  const Programmer::Group *programmerGroup();
  void writeProgrammerGroup(const Programmer::Group *group);
  const Tool::Group *toolGroup();
  void writeToolGroup(const Tool::Group *group);
};

//-----------------------------------------------------------------------------
template <typename EnumType>
class EnumComboBox
{
public:
  EnumComboBox(const QString &key, QWidget *parent) : _key(key) {
    _combo = new QComboBox(parent);
    FOR_EACH(EnumType, type) _combo->insertItem(i18n(type.label()));
    Config config;
    EnumType type = config.readEnumEntry<EnumType>(key);
    if ( type!=EnumType::Nb_Types ) _combo->setCurrentItem(type);
  }
  EnumComboBox(const QString &emptyLabel, const QString &key, QWidget *parent) : _key(key) {
    _combo = new QComboBox(parent);
    _combo->insertItem(emptyLabel);
    FOR_EACH(EnumType, type) _combo->insertItem(i18n(type.label()));
    Config config;
    EnumType type = config.readEnumEntry<EnumType>(key);
    if ( type!=EnumType::Nb_Types ) _combo->setCurrentItem(type+1);
  }
  QComboBox *combo() { return _combo; }
  EnumType value() const {
    if ( _combo->count()==EnumType::Nb_Types ) return _combo->currentItem();
    if ( _combo->currentItem()==0 ) return EnumType::Nb_Types;
    return _combo->currentItem()-1;
  }
  void reset() { _combo->setCurrentItem(0); }
  void writeConfig() {
    Config config;
    config.writeEnumEntry<EnumType>(_key, value());
  }

private:
  QString    _key;
  QComboBox *_combo;
};

//-----------------------------------------------------------------------------
class Dialog : public ::Dialog
{
Q_OBJECT
public:
  Dialog(const QString &device, Type type, QWidget *parent);
  virtual ~Dialog();

  QString device() const;

private slots:
  void listDoubleClicked(QListViewItem *item);
  void currentChanged(QListViewItem *item);
  void deviceChange(const QString &device);
  void updateList();
  void resetFilters();

private:
  bool _withAuto;
  KeyComboBox *_programmerCombo, *_toolCombo;
  EnumComboBox<ListType>                 *_listTypeCombo;
  EnumComboBox<Device::MemoryTechnology> *_memoryCombo;
  EnumComboBox<Device::Status>           *_statusCombo;
  EnumComboBox<Pic::Feature>             *_featureCombo;
  KListView   *_listView;
  View        *_deviceView;

  void updateList(const QString &device);
  const Programmer::Group *programmerGroup() const;
  const Tool::Group *toolGroup() const;
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
class View : public TabWidget
{
Q_OBJECT
public:
  View(QWidget *parent);
  void clear();
  void setText(const QString &text);
  void setDevice(const QString &name, bool cannotChangeDevice);

signals:
  void deviceChanged(const QString &device);

private:
  QMimeSourceFactory _msf;
  Browser *_info, *_vfg, *_pins;
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
