/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef REGISTER_EDITOR_H
#define REGISTER_EDITOR_H

#include "device_editor.h"
#include "devices/gui/register_view.h"

namespace Register
{
class View;

//-----------------------------------------------------------------------------
class MainView : public DeviceEditor
{
Q_OBJECT
public:
  MainView(const QString &title, const QString &tag, QWidget *parent = 0);
  virtual bool isModified() const { return false; }
  virtual bool isReadOnly() const { return true; }
  virtual void addGui() {}
  virtual void removeGui() {}
  virtual void setFocus() {}
  virtual void setDevice(bool force = false);
  Register::View *view() { return static_cast<Register::View *>(_view); }

private:
  bool _debugging;
  virtual QWidget *createView(const Device::Data *, QWidget *parent);
  virtual void setModifiedInternal(bool) {}
  virtual void setReadOnlyInternal(bool) {}
};

} // namespace

#endif
