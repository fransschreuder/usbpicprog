/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROG_GROUP_UI_H
#define PROG_GROUP_UI_H

#include "common/gui/container.h"
#include "common/gui/dialog.h"
#include "progs/base/prog_group.h"
#include "progs/base/generic_prog.h"
#include "devices/pic/gui/pic_memory_editor.h"
class Container;
class PopupButton;

namespace Programmer
{
class ConfigWidget;
class Base;
class Group;

//----------------------------------------------------------------------------
class StandaloneMemoryCalibrationEditor : public Pic::MemoryCalibrationEditor
{
Q_OBJECT
public:
  StandaloneMemoryCalibrationEditor(const Pic::Memory &memory, QWidget *parent);
  virtual void init(bool first);
  virtual bool hasAction(Device::Action) const { return false; }

signals:
  void updateCalibration();
  void regenerate();
};

//----------------------------------------------------------------------------
class ButtonContainer : public ::ButtonContainer
{
Q_OBJECT
public:
  ButtonContainer(const QString &title, QObject *receiver, const char *updateSlot, QWidget *parent);
};

//----------------------------------------------------------------------------
class AdvancedDialog : public Dialog
{
Q_OBJECT
public:
  AdvancedDialog(Base &base, QWidget *parent, const char *name);
  virtual void updateDisplay();

protected:
  Base      &_base;
  Container *_programmerContainer, *_calibrationContainer;
  ButtonContainer *_firmwareContainer, *_voltagesContainer, *_selfTestContainer;
  Pic::MemoryCalibrationEditor *_calEditor;
  bool connectProgrammer();
  bool ensureConnected();

protected slots:
  void updateFirmware();
  void uploadFirmware();
  void updateVoltages();
  void updateSelfTest();
  void updateCalibration();
  virtual void regenerateCalibration();

private:
  QLabel *_firmwareLabel;
};

//----------------------------------------------------------------------------
class GroupUI : public ::Group::BaseGui
{
public:
  virtual ::Programmer::ConfigWidget *createConfigWidget(QWidget *parent) const = 0;
  virtual bool hasAdvancedDialog() const = 0;
  virtual AdvancedDialog *createAdvancedDialog(Base &base, QWidget *parent) const = 0;
};

inline const ::Programmer::GroupUI &groupui(const ::Programmer::Base &base) { return static_cast<const ::Programmer::GroupUI &>(*base.group().gui()); }

} // namespace

#endif
