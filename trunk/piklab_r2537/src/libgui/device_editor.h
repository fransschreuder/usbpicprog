/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DEVICE_EDITOR_H
#define DEVICE_EDITOR_H

#include "editor.h"
namespace Device { class Data; }

class DeviceEditor : public Editor
{
Q_OBJECT
public:
  DeviceEditor(const QString &title, const QString &tag, QWidget *parent, const char *name);
  DeviceEditor(QWidget *parent, const char *name);
  virtual PURL::FileType fileType() const { return PURL::Nb_FileTypes; }
  virtual PURL::Url url() const { return PURL::Url(); }
  virtual void setDevice(bool force = false);
  static QString guessDeviceFromFile(const PURL::Url &url);
  virtual bool save(const PURL::Url &) { return false; }
  virtual bool open(const PURL::Url &) { return true; }

public slots:
  virtual void statusChanged() { emit statusTextChanged("  "); }

protected:
  QString      _device;
  QWidget     *_widget, *_view;
  QVBoxLayout *_top, *_vbox;
  QLabel      *_labelDevice, *_labelWarning;

  void init();
  virtual QWidget *createView(const Device::Data *data, QWidget *parent) = 0;
  static PURL::Url findAsmFile(const PURL::Url &url);

private slots:
  void updateDevice() { setDevice(); }
};

#endif
