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
class TextObject;

//-----------------------------------------------------------------------------
class BaseEditor : public SimpleTextEditor, public Log::Base
{
Q_OBJECT
public:
  BaseEditor(const PURL::Url &source, const Device::Data &data, Log::Base *log, QWidget *parent);
  BaseEditor(const Device::Data &data, QWidget *parent);
  virtual PURL::Url url() const { return _url; }

protected:
  PURL::Url           _source, _url;
  const Device::Data &_device;
  Log::Base          *_log;
};

//-----------------------------------------------------------------------------
class Editor : public BaseEditor
{
Q_OBJECT
public:
  Editor(const PURL::Url &source, const Device::Data &data, Log::Base &log, QWidget *parent);
  Editor(const TextObject &object, QWidget *parent);
  virtual ~Editor();
  virtual PURL::FileType fileType() const { return PURL::Coff; }
  virtual bool open(const PURL::Url &url);

private:
  TextObject         *_created;
  const TextObject   *_provided;
};

} // namespace

//-----------------------------------------------------------------------------
class DisassemblyEditor : public Coff::BaseEditor
{
Q_OBJECT
public:
  DisassemblyEditor(const PURL::Url &hexUrl, const Device::Data &data, QWidget *parent);
  DisassemblyEditor(const HexEditor &e, const Device::Data &data, QWidget *parent);
  virtual PURL::FileType fileType() const { return PURL::AsmGPAsm; }
  virtual bool open(const PURL::Url &url);

private:
  const HexEditor    *_editor;
};


#endif
