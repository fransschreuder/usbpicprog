/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003 Alain Gibaud <alain.gibaud@free.fr>                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "hex_editor.h"

#include <qgroupbox.h>
#include <qhgroupbox.h>
#include <qregexp.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qtimer.h>

#include <klocale.h>
#include <ktempfile.h>

#include "devices/base/device_group.h"
#include "devices/gui/device_group_ui.h"
#include "devices/gui/hex_view.h"
#include "toplevel.h"
#include "common/global/pfile.h"
#include "main_global.h"

//-----------------------------------------------------------------------------
HexEditorPart::HexEditorPart(HexEditor *editor)
 : KParts::ReadWritePart(editor, "hex_editor_part")
{
  setXMLFile("hexeditorpartui.rc");

  (void)KStdAction::save(editor, SLOT(save()), actionCollection());
  (void)KStdAction::saveAs(editor, SLOT(saveAs()), actionCollection());
  (void)new KToggleAction(i18n("Read Only Mode"), 0, 0, editor, SLOT(toggleReadOnly()), actionCollection(), "tools_toggle_write_lock");
}

void HexEditorPart::setReadWrite(bool rw)
{
  KParts::ReadWritePart::setReadWrite(rw);
  static_cast<KToggleAction *>(action("tools_toggle_write_lock"))->setChecked(!rw);
}

//-----------------------------------------------------------------------------
HexEditor::HexEditor(const QString &name, QWidget *parent)
  : DeviceEditor(name, QString::null, parent, "hex_editor")
{
  init();
}

HexEditor::HexEditor(QWidget *parent)
  : DeviceEditor(parent, "hex_editor")
{
  init();
}

void HexEditor::init()
{
  _modified = false;
  _memory = 0;
  _originalMemory = 0;
  _top->addStretch(1);
  _part = new HexEditorPart(this);
  setReadOnly(false);
}

void HexEditor::clear()
{
  delete _memory;
  _memory = 0;
  delete _originalMemory;
  _originalMemory = 0;
}

QWidget *HexEditor::createView(const Device::Data *data, QWidget *parent)
{
  clear();
  if (data) {
    _originalMemory = data->group().createMemory(*data);
    _memory = data->group().createMemory(*data);
  }
  if ( data==0 ) return new QWidget(parent);
  Device::HexView *hv = Device::groupui(*data).createHexView(*this, parent);
  connect(hv, SIGNAL(modified()), SLOT(slotModified()));
  _dirty = true;
  QTimer::singleShot(0, this, SLOT(simpleLoad()));
  return hv;
}

bool HexEditor::simpleLoad()
{
  if ( !_dirty ) return true;
  _dirty = false;
  if (_memory) {
    QStringList warnings;
    if ( _memory->fromHexBuffer(_hexBuffer, warnings)!=Device::Memory::NoWarning ) {
      _labelWarning->setText(i18n("<b>Warning:</b> hex file seems to be incompatible with the selected device %1:<br>%2")
                             .arg(_memory->device().name()).arg(warnings.join("<br>")));
      _labelWarning->show();
    } else _labelWarning->hide();
    display();
  }
  return true;
}

void HexEditor::setReadOnlyInternal(bool readOnly)
{
  _part->setReadWrite(!readOnly);
  if (_memory) static_cast<Device::HexView *>(_view)->setReadOnly(readOnly);
}

void HexEditor::addGui()
{
  Main::toplevel().guiFactory()->addClient(_part);
}

void HexEditor::removeGui()
{
  Main::toplevel().guiFactory()->removeClient(_part);
}

bool HexEditor::open(const PURL::Url &url)
{
  _url = url;
  PURL::File file(url, &Main::compileLog());
  if ( !file.openForRead() ) return false;
  QStringList errors;
  if ( !_hexBuffer.load(file.stream(), errors) ) {
    MessageBox::detailedSorry(i18n("Error(s) reading hex file."), errors.join("\n"), Log::Show);
    return false;
  }
  _dirty = true;
  return simpleLoad();
}

bool HexEditor::save(const PURL::Url &url)
{
  return save(url, i18n("File URL: \"%1\".").arg(url.pretty()));
}

bool HexEditor::save(const PURL::Url &url, const QString &fileErrorString)
{
  PURL::File file(url, &Main::compileLog());
  if ( !file.openForWrite() ) return false;
  if ( !_memory->save(file.stream(), HexBuffer::IHX32) ) {
    MessageBox::detailedSorry(i18n("Error while writing file \"%1\".").arg(url.pretty()), fileErrorString, Log::Show);
    return false;
  }
  _originalMemory->copyFrom(*_memory);
  _url = url;
  return file.close();
}

void HexEditor::display()
{
  _modified = false;
  if (_memory) {
    _originalMemory->copyFrom(*_memory);
    static_cast<Device::HexView *>(_view)->display(_memory);
    static_cast<Device::HexView *>(_view)->setReadOnly(isReadOnly());
    static_cast<Device::HexView *>(_view)->updateDisplay();
  }
  statusChanged();
}

void HexEditor::memoryRead()
{
  display();
  emit guiChanged();
}

void HexEditor::slotModified()
{
  static_cast<Device::HexView *>(_view)->updateDisplay();
  _modified = true;
  statusChanged();
  emit guiChanged();
}

void HexEditor::statusChanged()
{
  QString s;
  if (_memory) {
    uint cs = static_cast<Device::HexView *>(_view)->checksum();
    s = i18n("Checksum: %1").arg(toHexLabel(cs, 4));
  }
  emit statusTextChanged(s);
}
