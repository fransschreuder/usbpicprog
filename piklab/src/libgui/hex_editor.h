/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef HEX_EDITOR_H
#define HEX_EDITOR_H

#include <kparts/part.h>

#include "device_editor.h"
#include "devices/base/hex_buffer.h"
#include "devices/base/generic_memory.h"

//-----------------------------------------------------------------------------
class HexEditor;

class HexEditorPart : public KParts::ReadWritePart
{
Q_OBJECT
public:
  HexEditorPart(HexEditor *editor);
  virtual void setReadWrite(bool readWrite);

private:
  virtual bool openFile() { return true; }
  virtual bool saveFile() { return true; }
};

//-----------------------------------------------------------------------------
class HexEditor : public DeviceEditor
{
Q_OBJECT
public:
  HexEditor(const QString &name, QWidget *parent);
  HexEditor(QWidget *parent);
  virtual bool isModified() const { return _modified; }
  virtual bool isReadOnly() const { return !_part->isReadWrite(); }
  virtual ~HexEditor() { clear(); }
  virtual PURL::FileType fileType() const { return PURL::Hex; }
  virtual PURL::Url url() const { return _url; }
  virtual bool save(const PURL::Url &url);
  virtual bool open(const PURL::Url &url);
  Device::Memory *memory() { return _memory; }
  const Device::Memory *memory() const { return _memory; }
  const Device::Memory *originalMemory() const { return _originalMemory; }
  void memoryRead();
  virtual void addGui();
  virtual void removeGui();
  virtual void setFocus() {}

public slots:
  virtual void statusChanged();

private slots:
  void slotModified();
  bool simpleLoad();

private:
  Device::Memory *_originalMemory, *_memory;
  HexBuffer       _hexBuffer;
  bool            _modified, _dirty;
  HexEditorPart  *_part;
  PURL::Url       _url;

  virtual void dropEvent(QDropEvent *e) { emit dropEventPass(e); }
  bool save(const PURL::Url &url, const QString &fileErrorString);
  bool verifyDeviceType();
  virtual QWidget *createView(const Device::Data *data, QWidget *parent);
  virtual void setModifiedInternal(bool modified) { _modified = modified; }
  virtual void setReadOnlyInternal(bool readOnly);
  void display();
  void init();
  void clear();
};

#endif
