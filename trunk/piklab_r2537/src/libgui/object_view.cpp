/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "object_view.h"

#include <klocale.h>

#include "devices/base/device_group.h"
#include "devices/pic/pic/pic_memory.h"
#include "tools/gputils/gputils.h"
#include "hex_editor.h"
#include "coff/base/text_coff.h"
#include "coff/base/coff_archive.h"
#include "main_global.h"
#include "tools/list/compile_process.h"
#include "coff/base/disassembler.h"
#include "common/gui/misc_gui.h"

//-----------------------------------------------------------------------------
Coff::BaseEditor::BaseEditor(const PURL::Url &source, const Device::Data *data, QWidget *parent)
  : SimpleTextEditor(true, parent), _source(source), _created(0), _device(data)
{
  setReadOnly(true);
  _view->setDynWordWrap(false);
  setView(&Main::compileLog());
}

Coff::BaseEditor::~BaseEditor()
{
  delete _created;
}

//-----------------------------------------------------------------------------
Coff::CoffEditor::CoffEditor(const PURL::Url &source, const Device::Data &data, QWidget *parent)
  : BaseEditor(source, &data, parent), _provided(0)
{}

Coff::CoffEditor::CoffEditor(const TextObject &object, QWidget *parent)
  : BaseEditor(PURL::Url(), object.device(), parent), _provided(&object)
{}

bool Coff::CoffEditor::open(const PURL::Url &url)
{
  _url = url;
  if (_provided) return setText(_provided->disassembly());
  if ( _created==0 ) {
    _created = new TextObject(_device, _source);
    _created->parse(*this);
  }
  return setText(static_cast<Coff::TextObject *>(_created)->disassembly());
}

//-----------------------------------------------------------------------------
Coff::ObjectEditor::ObjectEditor(const PURL::Url &source, QWidget *parent)
  : BaseEditor(source, 0, parent)
{}

bool Coff::ObjectEditor::open(const PURL::Url &url)
{
  _url = url;
  if ( _created==0 ) {
    _created = new TextObject(0, _source);
    _created->parse(*this);
  }
  Log::KeyList keys = _created->information();
  keys.setTitle(i18n("Information:"));
  return setText(keys.text());
}


//-----------------------------------------------------------------------------
Coff::LibraryEditor::LibraryEditor(const PURL::Url &source, QWidget *parent)
  : BaseEditor(source, 0, parent)
{}

bool Coff::LibraryEditor::open(const PURL::Url &url)
{
  _url = url;
  if ( _created==0 ) {
    _created = new Archive(_source);
    _created->parse(*this);
  }
  Log::KeyList keys = _created->information();
  keys.setTitle(i18n("Information:"));
  return setText(keys.text());
}

//-----------------------------------------------------------------------------
DisassemblyEditor::DisassemblyEditor(const PURL::Url &source, const Device::Data &data, QWidget *parent)
  : SimpleTextEditor(true, parent), _source(source), _device(data), _editor(0)
{
  setReadOnly(true);
  _view->setDynWordWrap(false);
  setView(&Main::compileLog());
}

DisassemblyEditor::DisassemblyEditor(const HexEditor &e, const Device::Data &data, QWidget *parent)
  : SimpleTextEditor(true, parent), _device(data), _editor(&e)
{
  setReadOnly(true);
  _view->setDynWordWrap(false);
  setView(&Main::compileLog());
}

bool DisassemblyEditor::open(const PURL::Url &url)
{
  _url = url;
  if ( Main::toolGroup().name()!="sdcc" && Main::toolGroup().name()!="gputils" ) {
    MessageBox::sorry(i18n("Disassembly not supported for the selected toolchain."), Log::Show);
    return false;
  }
  Pic::Architecture arch = (_device.group().name()=="pic" ? static_cast<const Pic::Data &>(_device).architecture() : Pic::Architecture(Pic::Architecture::Nb_Types));
  if ( arch==Pic::Architecture::Nb_Types ) {
    MessageBox::sorry(i18n("Disassembly not supported for the selected device."), Log::Show);
    return false;
  }

  Device::Memory *memory = 0;
  if ( _editor==0 ) {
    log(Log::LineType::Information, i18n("Disassembling hex file: %1").arg(_source.pretty()));
    PURL::File file(_source, Main::compileLog());
    if ( !file.openForRead() ) return false;
    memory = _device.group().createMemory(_device);
    QStringList errors, warnings;
    Device::Memory::WarningTypes warningTypes;
    if ( !memory->load(file.stream(), errors, warningTypes, warnings) ) {
      delete memory;
      log(Log::LineType::Error, errors[0]);
      return false;
    }
    for (uint i=0; i<warnings.count(); i++) log(Log::LineType::Warning, warnings[i]);
  } else {
    log(Log::LineType::Information, i18n("Disassembling content of hex file editor."));
    memory = const_cast<Device::Memory *>(_editor->memory());
  }
  SourceLine::List list = GPUtils::disassemble(static_cast<const Pic::Memory &>(*memory));
  if ( _editor==0 ) delete memory;
  return setText(SourceLine::text(PURL::SourceFamily::Asm, list, 4));
}
