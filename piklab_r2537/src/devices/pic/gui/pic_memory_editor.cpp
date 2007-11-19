/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_memory_editor.h"

#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qscrollbar.h>
#include <qgrid.h>
#include <qhbox.h>
#include <qtooltip.h>
#include <qregexp.h>
#include <qcolor.h>
#include <qlayout.h>
#include <qpixmap.h>

#include <klocale.h>
#include <kpushbutton.h>
#include <kaction.h>

#include "common/common/misc.h"
#include "pic_config_editor.h"
#include "libgui/toplevel.h"
#include "libgui/main_global.h"
#include "devices/pic/prog/pic_prog.h"
#include "libgui/global_config.h"
#include "pic_hex_view.h"

//-----------------------------------------------------------------------------
Pic::MemoryEditorLegend::Data::Data(const QString &text, QWidget *parent)
{
  button = new PopupButton(text, parent);
  KActionCollection *ac = 0;
  KAction *a = new KAction(i18n("Go to start"), "top", 0, parent, SLOT(gotoStart()), ac);
  actions.append(a);
  button->appendAction(a);
  a = new KAction(i18n("Go to end"), "bottom", 0, parent, SLOT(gotoEnd()), ac);
  actions.append(a);
  button->appendAction(a);
  label = new QLabel(parent);
}

void Pic::MemoryEditorLegend::Data::setProtected(bool on)
{
  if (on) label->setPaletteBackgroundColor(MemoryEditorLegend::protectedColor());
  else label->unsetPalette();
}

bool Pic::MemoryEditorLegend::Data::hasAction(const KAction *action) const
{
  for (uint i=0; i<actions.count(); i++) if ( actions[i]==action ) return true;
  return false;
}

//-----------------------------------------------------------------------------
const char * const Pic::MemoryEditorLegend::BLOCK_COLORS[Protection::MAX_NB_BLOCKS] = {
  "#88FF88", "#88FFFF", "#FFFF88", "#FF88FF", "#0088FF", "#88FF00", "#00FF88", "#FF8800"
};

Pic::MemoryEditorLegend::MemoryEditorLegend(MemoryRangeType type, Memory &memory, QWidget *parent)
  : MemoryEditor(type, memory, parent, "memory_displayer_legend")
{
  QGridLayout *grid = new QGridLayout(_top);

  QWidget *w = new QWidget(this);
  w->setFixedWidth(20);
  w->setPaletteBackgroundColor(protectedColor());
  grid->addWidget(w, 0, 0);
  const Protection &protection = device().config().protection();
  QString s = (protection.family()==Protection::CodeGuard ? i18n("High Security") : i18n("Code protection"));
  QLabel *label = new QLabel(s, this);
  grid->addMultiCellWidget(label, 0,0, 1,2);
  grid->addRowSpacing(1, 10);
  uint row = 2;

  if ( type==MemoryRangeType::Code && protection.hasBootBlock() ) {
    w = new QWidget(this);
    w->setFixedWidth(20);
    w->setPaletteBackgroundColor(bootColor());
    grid->addWidget(w, row, 0);
    _boot = Data(protection.bootLabel(), this);
    grid->addWidget(_boot.button, row, 1);
    grid->addWidget(_boot.label, row, 2);
    row++;
  }

  uint nb = (type==MemoryRangeType::Code ? protection.nbBlocks() : 0);
  for (uint i=0; i<nb; i++) {
    w = new QWidget(this);
    w->setFixedWidth(20);
    w->setPaletteBackgroundColor(blockColor(i));
    grid->addWidget(w, row, 0);
    _blocks.append(Data(protection.blockLabel(i), this));
    grid->addWidget(_blocks[i].button, row, 1);
    grid->addWidget(_blocks[i].label, row, 2);
    row++;
  }
}

void Pic::MemoryEditorLegend::updateDisplay()
{
  const Protection &protection = device().config().protection();
  Protection::Type ptype = (protection.family()==Protection::CodeGuard ? Protection::HighSecurity : Protection::ProgramProtected);
  uint nbChars = 2 * device().nbBytesAddress();
  if (_boot.label) {
    AddressRange r = memory().bootRange();
    if ( r.isEmpty() ) _boot.label->setText(i18n("not present"));
    else _boot.label->setText(QString("[%1:%2]").arg(toHex(r.start, nbChars)).arg(toHex(r.end, nbChars)));
    _boot.button->setEnabled(!r.isEmpty());
    _boot.setProtected(memory().isBootProtected(ptype));
  }
  for (uint i=0; i<_blocks.count(); i++) {
    AddressRange r = memory().blockRange(i);
    if ( r.isEmpty() ) _blocks[i].label->setText(i18n("not present"));
    else _blocks[i].label->setText(QString("[%1:%2]").arg(toHex(r.start, nbChars)).arg(toHex(r.end, nbChars)));
    _blocks[i].button->setEnabled(!r.isEmpty());
    _blocks[i].setProtected(memory().isBlockProtected(ptype, i));
  }
}

void Pic::MemoryEditorLegend::gotoStart()
{
  Address start = device().range(type()).start;
  const KAction *action = static_cast<const KAction *>(sender());
  if ( _boot.hasAction(action) ) {
    AddressRange r = memory().bootRange();
    emit setStartWord(r.start - start);
    return;
  }
  for (uint i=0; i<_blocks.count(); i++) {
    if ( _blocks[i].hasAction(action) ) {
      AddressRange r = memory().blockRange(i);
      emit setStartWord(r.start - start);
      return;
    }
  }
  Q_ASSERT(false);
}

void Pic::MemoryEditorLegend::gotoEnd()
{
  Address start = device().range(type()).start;
  const KAction *action = static_cast<const KAction *>(sender());
  if ( _boot.hasAction(action) ) {
    AddressRange r = memory().bootRange();
    emit setEndWord(r.end - start);
    return;
  }
  for (uint i=0; i<_blocks.count(); i++) {
    if ( _blocks[i].hasAction(action) ) {
      AddressRange r = memory().blockRange(i);
      emit setEndWord(r.end - start);
      return;
    }
  }
  Q_ASSERT(false);
}


//-----------------------------------------------------------------------------
Pic::HexWordEditor::HexWordEditor(MemoryRangeType type, Memory &memory, QWidget *parent)
  : Device::HexWordEditor(memory, memory.device().nbCharsWord(type), parent),
    MemoryCaster(type, memory)
{}

void Pic::HexWordEditor::setWord(BitValue value)
{
  if ( type()==MemoryRangeType::Config ) {
    const Config::Word &cword = device().config()._words[_offset];
    value |= cword.usedMask().complementInMask(device().mask(MemoryRangeType::Config));
  }
  memory().setWord(type(), _offset, value);
}

//-----------------------------------------------------------------------------
Pic::MemoryRangeEditor::MemoryRangeEditor(MemoryRangeType type, Memory &memory,
                                          uint nbLines, uint nbCols,
                                          uint wordOffset, int nbWords, QWidget *parent)
  : Device::MemoryRangeEditor(memory, nbLines, nbCols, wordOffset, nbWords, parent, "pic_memory_range_editor"),
    MemoryCaster(type, memory), _legend(0)
{
  if ( type==MemoryRangeType::Code ) _blockRanges.resize(memory.device().config().protection().nbBlocks());
}

void Pic::MemoryRangeEditor::addLegend(QVBoxLayout *vbox)
{
  if ( type()==MemoryRangeType::Code || type()==MemoryRangeType::Eeprom ) {
    _legend = new MemoryEditorLegend(type(), memory(), this);
    connect(_legend, SIGNAL(setStartWord(int)), SLOT(setStartWord(int)));
    connect(_legend, SIGNAL(setEndWord(int)), SLOT(setEndWord(int)));
    vbox->addWidget(_legend);
  }
}

bool Pic::MemoryRangeEditor::isRangeReadOnly() const
{
  return ( (type().data().properties & ReadOnly) || type()==MemoryRangeType::CalBackup );
}

void Pic::MemoryRangeEditor::updateDisplay()
{
  const Protection &protection = device().config().protection();
  if ( type()==MemoryRangeType::Code ) {
    if ( protection.hasBootBlock() ) _bootRange = memory().bootRange();
    for (uint k=0; k<_blockRanges.count(); k++)
      _blockRanges[k] = memory().blockRange(k);
  }
  Protection::Type ptype = (protection.family()==Protection::CodeGuard ? Protection::HighSecurity : Protection::ProgramProtected);
  _codeProtected = memory().protectedRanges(ptype, type());
  Device::MemoryRangeEditor::updateDisplay();
  if (_legend) _legend->updateDisplay();
}

void Pic::MemoryRangeEditor::updateAddressColor(uint i, Address address)
{
  if ( _codeProtected.contains(address) )
    _addresses[i]->setPaletteBackgroundColor(MemoryEditorLegend::protectedColor());
  else _addresses[i]->unsetPalette();
  _blocks[i]->unsetPalette();
  if ( type()==MemoryRangeType::Code ) {
    if ( _bootRange.contains(address) ) _blocks[i]->setPaletteBackgroundColor(MemoryEditorLegend::bootColor());
    else for (uint k=0; k<_blockRanges.count(); k++) {
      if ( !_blockRanges[k].contains(address) ) continue;
      _blocks[i]->setPaletteBackgroundColor(MemoryEditorLegend::blockColor(k));
      break;
    }
  }
}

Device::HexWordEditor *Pic::MemoryRangeEditor::createHexWordEditor(QWidget *parent)
{
  return new HexWordEditor(type(), memory(), parent);
}

//-----------------------------------------------------------------------------
Pic::MemoryTypeEditor::MemoryTypeEditor(const HexView *hexview, MemoryRangeType type, Memory &memory, QWidget *parent, const char *name)
  : Device::MemoryTypeEditor(hexview, memory, parent, name), MemoryCaster(type, memory)
{}

void Pic::MemoryTypeEditor::init(bool first)
{
  Device::MemoryTypeEditor::init(first);
  _title->setText(i18n(type().label()));

  uint nbChars = device().nbCharsWord(type());
  const MemoryRangeData &mr = device().range(type());
  QString add;
  if ( type()==MemoryRangeType::UserId ) add = i18n(" - read mask: %1").arg(toHexLabel(mr.rmask, nbChars));
  if ( type()==MemoryRangeType::Cal && _hexview ) add = i18n(" - not programmed by default");
  QString comment = i18n("%1-bit words - mask: %2")
                   .arg(device().nbBitsWord(type())).arg(toHexLabel(device().mask(type()), nbChars));
  _comment->setText(comment + add);
}

bool Pic::MemoryTypeEditor::internalDoAction(Device::Action action)
{
  Programmer::PicBase *prog = static_cast<Programmer::PicBase *>(Main::programmer());
  switch (action) {
    case Device::Clear: memory().clear(type()); return true;
    case Device::Zero:  memory().fill(type(), 0);  return true;
    case Device::ChecksumCheck : memory().checksumCheckFill(); return true;
    case Device::Reload: {
      const Memory *omemory = static_cast<const Memory *>(originalMemory());
      Q_ASSERT(omemory);
      memory().copyFrom(type(), *omemory); return true;
    }
    case Device::Program:
      prog->programSingle(type(), memory());
      return false;
    case Device::Verify:
      prog->verifySingle(type(), memory());
      return false;
    case Device::Read:
      return prog->readSingle(type(), memory());
    case Device::Erase:
      prog->eraseSingle(type());
      return false;
    case Device::BlankCheck:
      prog->blankCheckSingle(type());
      return false;
    case Device::Nb_Actions: break;
  }
  Q_ASSERT(false);
  return false;
}

//-----------------------------------------------------------------------------
Pic::MemoryTypeRangeEditor::MemoryTypeRangeEditor(const HexView *hexview, MemoryRangeType type, Memory &memory, QWidget *parent)
  : MemoryTypeEditor(hexview, type, memory, parent, "pic_memory_type_range_editor"), _mre(0)
{}

void Pic::MemoryTypeRangeEditor::init(bool first)
{
  MemoryTypeEditor::init(first);
  uint nbLines = 0;
  if ( type()==MemoryRangeType::Code ) nbLines = 16;
  else if ( type()==MemoryRangeType::Eeprom ) nbLines = 8;
  else nbLines = (device().nbWords(type())/8>1 ? 2 : 1);
  _mre = new MemoryRangeEditor(type(), memory(), nbLines, 8, 0, -1, this);
  addEditor(_mre);
  _top->addWidget(_mre);
  _mre->init();
}

//-----------------------------------------------------------------------------
Pic::MemoryUserIdEditor::MemoryUserIdEditor(const HexView *hexview, Memory &memory, QWidget *parent)
  : MemoryTypeRangeEditor(hexview, MemoryRangeType::UserId, memory, parent), _saveReadOnly(false)
{}

void Pic::MemoryUserIdEditor::init(bool first)
{
  MemoryTypeRangeEditor::init(first);
  _setToChecksum = new KToggleAction(i18n("Set to unprotected checksum"), 0, 0,
    this, SLOT(toggleSetToChecksum()), Main::toplevel().actionCollection());
  addAction(_setToChecksum);
  if ( readConfigEntry(BaseGlobalConfig::UserIdSetToChecksum).toBool() && memory().isClear(MemoryRangeType::UserId) ) {
    _setToChecksum->activate();
    toggleSetToChecksum();
  }
}

void Pic::MemoryUserIdEditor::toggleSetToChecksum()
{
  if ( _setToChecksum->isChecked() ) {
    _mre->setComment(i18n("Set to unprotected checksum"));
    emit modified();
  } else _mre->setComment(QString::null);
  setReadOnly(_saveReadOnly);
}

void Pic::MemoryUserIdEditor::updateDisplay()
{
  if ( _setToChecksum->isChecked() ) memory().setUserIdToUnprotectedChecksum();
  MemoryTypeRangeEditor::updateDisplay();
}

void Pic::MemoryUserIdEditor::setReadOnly(bool readOnly)
{
  _saveReadOnly = readOnly;
  MemoryTypeRangeEditor::setReadOnly(readOnly || _setToChecksum->isChecked());
}

//-----------------------------------------------------------------------------
Pic::MemoryCalibrationEditor::MemoryCalibrationEditor(const HexView *hexview, Memory &memory, QWidget *parent)
  : MemoryTypeEditor(hexview, MemoryRangeType::Cal, memory, parent, "pic_memory_calibration_editor")
{}

void Pic::MemoryCalibrationEditor::init(bool first)
{
  MemoryTypeEditor::init(first);
  MemoryRangeEditor *mre = new MemoryRangeEditor(MemoryRangeType::Cal, memory(), 1, 8, 0, -1, this);
  addEditor(mre);
  _top->addWidget(mre);
  mre->init();
  if ( device().isReadable(MemoryRangeType::CalBackup) ) {
    mre = new MemoryRangeEditor(MemoryRangeType::CalBackup, memory(), 1, 8, 0, -1, this);
    addEditor(mre);
    _top->addWidget(mre);
    mre->init();
    mre->setComment(i18n("(backup)"));
  }
}

bool Pic::MemoryCalibrationEditor::hasAction(Device::Action action) const
{
  return ( action==Device::Read || action==Device::Verify || action==Device::Program );
}

bool Pic::MemoryCalibrationEditor::internalDoAction(Device::Action action)
{
  Programmer::PicBase *prog = static_cast<Programmer::PicBase *>(Main::programmer());
  switch (action) {
    case Device::Reload: {
      const Memory *omemory = static_cast<const Memory *>(originalMemory());
      Q_ASSERT(omemory);
      memory().copyFrom(MemoryRangeType::Cal, *omemory);
      memory().copyFrom(MemoryRangeType::CalBackup, *omemory);
      return true;
    }
    case Device::Program:
      if ( prog->programCalibration(memory().arrayForWriting(Pic::MemoryRangeType::Cal)) )
        return prog->readSingle(MemoryRangeType::Cal, memory());
      return false;
    case Device::Verify:
      prog->verifySingle(MemoryRangeType::Cal, memory());
      return false;
    case Device::Read:
      return prog->readSingle(MemoryRangeType::Cal, memory());
    case Device::Clear:
    case Device::Zero:
    case Device::ChecksumCheck:
    case Device::Erase:
    case Device::BlankCheck:
    case Device::Nb_Actions: break;
  }
  Q_ASSERT(false);
  return false;
}
