/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef OBJECT_VIEW_H
#define OBJECT_VIEW_H

#include "text_editor.h"
namespace Device { class Data; }
class HexEditor;

namespace Coff
{
class Base;
class TextObject;

//-----------------------------------------------------------------------------
class BaseEditor : public SimpleTextEditor, public Log::Base
{
Q_OBJECT
public:
  BaseEditor(const PURL::Url &source, const Device::Data *data, QWidget *parent);
  virtual ~BaseEditor();
  virtual PURL::Url url() const { return _url; }

protected:
  PURL::Url           _source, _url;
  Coff::Base         *_created;
  const Device::Data *_device;
};

//-----------------------------------------------------------------------------
class CoffEditor : public BaseEditor
{
Q_OBJECT
public:
  CoffEditor(const PURL::Url &source, const Device::Data &data, QWidget *parent);
  CoffEditor(const TextObject &object, QWidget *parent);
  virtual PURL::FileType fileType() const { return PURL::Coff; }
  virtual bool open(const PURL::Url &url);

private:
  const TextObject *_provided;
};

//-----------------------------------------------------------------------------
class ObjectEditor : public BaseEditor
{
Q_OBJECT
public:
  ObjectEditor(const PURL::Url &source, QWidget *parent);
  virtual PURL::FileType fileType() const { return PURL::Unknown; }
  virtual bool open(const PURL::Url &url);
};

//-----------------------------------------------------------------------------
class LibraryEditor : public BaseEditor
{
Q_OBJECT
public:
  LibraryEditor(const PURL::Url &source, QWidget *parent);
  virtual PURL::FileType fileType() const { return PURL::Unknown; }
  virtual bool open(const PURL::Url &url);
};

} // namespace

//-----------------------------------------------------------------------------
class DisassemblyEditor : public SimpleTextEditor, public Log::Base
{
Q_OBJECT
public:
  DisassemblyEditor(const PURL::Url &hexUrl, const Device::Data &data, QWidget *parent);
  DisassemblyEditor(const HexEditor &e, const Device::Data &data, QWidget *parent);
  virtual PURL::FileType fileType() const { return PURL::AsmGPAsm; }
  virtual bool open(const PURL::Url &url);
  virtual PURL::Url url() const { return _url; }

private:
  PURL::Url           _source, _url;
  const Device::Data &_device;
  const HexEditor    *_editor;
};


#endif
