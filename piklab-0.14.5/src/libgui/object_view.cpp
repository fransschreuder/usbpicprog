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
#include "main_global.h"
#include "tools/list/compile.h"
#include "coff/base/disassembler.h"
#include "common/gui/misc_gui.h"

//-----------------------------------------------------------------------------
Coff::BaseEditor::BaseEditor(const PURL::Url &source, const Device::Data &data, Log::Base *log, QWidget *parent)
  : SimpleTextEditor(true, parent), _source(source), _device(data), _log(log)
{
  setReadOnly(true);
  _view->setDynWordWrap(false);
  setView(&Main::compileLog());
}

Coff::BaseEditor::BaseEditor(const Device::Data &data, QWidget *parent)
  : SimpleTextEditor(true, parent), _device(data), _log(0)
{
  setReadOnly(true);
  _view->setDynWordWrap(false);
  setView(&Main::compileLog());
}

//-----------------------------------------------------------------------------
Coff::Editor::Editor(const PURL::Url &source, const Device::Data &data, Log::Base &log, QWidget *parent)
  : BaseEditor(source, data, &log, parent), _created(0), _provided(0)
{}

Coff::Editor::Editor(const TextObject &object, QWidget *parent)
  : BaseEditor(object.device(), parent), _created(0), _provided(&object)
{}

Coff::Editor::~Editor()
{
  delete _created;
}

bool Coff::Editor::open(const PURL::Url &url)
{
  _url = url;
  if (_provided) return setText(_provided->text());
  if ( _created==0 ) _created = new TextObject(_device, _source, *_log, 0);
  return setText(_created->text());
}

//-----------------------------------------------------------------------------
DisassemblyEditor::DisassemblyEditor(const PURL::Url &source, const Device::Data &data, QWidget *parent)
  : Coff::BaseEditor(source, data, 0, parent), _editor(0)
{}

DisassemblyEditor::DisassemblyEditor(const HexEditor &e, const Device::Data &data, QWidget *parent)
  : Coff::BaseEditor(data, parent), _editor(&e)
{}

bool DisassemblyEditor::open(const PURL::Url &url)
{
  _url = url;
  if ( Main::toolGroup().name()!="sdcc" && Main::toolGroup().name()!="gputils" ) {
    MessageBox::sorry(i18n("Disassembly not supported for the selected toolchain."), Log::Show);
    return false;
  }
  Pic::Architecture arch = (_device.group().name()=="pic" ? static_cast<const Pic::Data &>(_device).architecture() : Pic::Nb_Architectures);
  if ( arch==Pic::Nb_Architectures ) {
    MessageBox::sorry(i18n("Disassembly not supported for the selected device."), Log::Show);
    return false;
  }

  Device::Memory *memory = 0;
  if ( _editor==0 ) {
    log(Log::Information, i18n("Disassembling hex file: %1").arg(_source.pretty()));
    PURL::File file(_source, &Main::compileLog());
    if ( !file.openForRead() ) return false;
    memory = _device.group().createMemory(_device);
    QStringList errors, warnings;
    Device::Memory::WarningTypes warningTypes;
    if ( !memory->load(file.stream(), errors, warningTypes, warnings) ) {
      delete memory;
      log(Log::Error, errors[0]);
      return false;
    }
    for (uint i=0; i<warnings.count(); i++) log(Log::Warning, warnings[i]);
  } else {
    log(Log::Information, i18n("Disassembling content of hex file editor."));
    memory = const_cast<Device::Memory *>(_editor->memory());
  }
  SourceLine::List list = GPUtils::disassemble(static_cast<const Pic::Memory &>(*memory));
  if ( _editor==0 ) delete memory;
  return setText(SourceLine::text(PURL::Asm, list, 4));
}
