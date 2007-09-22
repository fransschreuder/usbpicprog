/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_config_word_editor.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <klocale.h>

#include "common/common/misc.h"
#include "common/gui/misc_gui.h"

//----------------------------------------------------------------------------
Pic::ConfigWordComboBox::ConfigWordComboBox(QWidget *parent)
  : QComboBox(parent)
{}

uint Pic::ConfigWordComboBox::index() const
{
  if ( isValid() ) return _map[currentItem()];
  if ( currentItem()==0 ) return _invalidIndex;
  return _map[currentItem()-1];
}

void Pic::ConfigWordComboBox::setItem(uint i)
{
  if ( !isValid() ) removeItem(0);
  for (uint l=0; l<_map.count(); l++)
    if ( _map[l]==i ) setCurrentItem(l);
}

void Pic::ConfigWordComboBox::setInvalidItem(uint i, const QString &label)
{
  if ( !isValid() ) changeItem(label, 0);
  else insertItem(label, 0);
  setCurrentItem(0);
  _invalidIndex = i;
}

//----------------------------------------------------------------------------
Pic::ConfigWordDialog::ConfigWordDialog(const Memory &memory, uint ci, QWidget *parent)
  : Dialog(parent, "config_word_dialog", true, i18n("Config Word Details"), Close, Close, false)
{
  uint nbChars = memory.device().nbCharsWord(MemoryConfig);
  const Config::Word &cword = memory.device().config()._words[ci];

  QGridLayout *grid = new QGridLayout(mainWidget(), 0, 0, 10, 10);
  uint row = 0;
  QLabel *label = new QLabel(i18n("Name:"), mainWidget());
  grid->addWidget(label, row, 0);
  label = new QLabel(cword.name, mainWidget());
  grid->addWidget(label, row, 1);
  row++;
  label = new QLabel(i18n("Index:"), mainWidget());
  grid->addWidget(label, row, 0);
  label = new QLabel(QString::number(ci), mainWidget());
  grid->addWidget(label, row, 1);
  row++;
  label = new QLabel(i18n("Raw Value:"), mainWidget());
  grid->addWidget(label, row, 0);
  label = new QLabel(toHexLabel(memory.word(MemoryConfig, ci), nbChars), mainWidget());
  grid->addWidget(label, row, 1);
  row++;
  label = new QLabel(i18n("Value:"), mainWidget());
  grid->addWidget(label, row, 0);
  label = new QLabel(toHexLabel(memory.normalizedWord(MemoryConfig, ci), nbChars), mainWidget());
  grid->addWidget(label, row, 1);
  row++;
  label = new QLabel(i18n("Raw Blank Value:"), mainWidget());
  grid->addWidget(label, row, 0);
  label = new QLabel(toHexLabel(cword.bvalue, nbChars), mainWidget());
  grid->addWidget(label, row, 1);
  row++;
  label = new QLabel(i18n("Used Mask:"), mainWidget());
  grid->addWidget(label, row, 0);
  label = new QLabel(toHexLabel(cword.usedMask(), nbChars), mainWidget());
  grid->addWidget(label, row, 1);
  row++;
  label = new QLabel(i18n("Write Mask:"), mainWidget());
  grid->addWidget(label, row, 0);
  label = new QLabel(toHexLabel(cword.wmask, nbChars), mainWidget());
  grid->addWidget(label, row, 1);
  row++;
  label = new QLabel(i18n("Protected Mask:"), mainWidget());
  grid->addWidget(label, row, 0);
  label = new QLabel(toHexLabel(cword.pmask, nbChars), mainWidget());
  grid->addWidget(label, row, 1);
  row++;
  label = new QLabel(i18n("Checksum Mask:"), mainWidget());
  grid->addWidget(label, row, 0);
  label = new QLabel(toHexLabel(cword.cmask, nbChars), mainWidget());
  grid->addWidget(label, row, 1);
  row++;
}

//----------------------------------------------------------------------------
Pic::ConfigWordEditor::ConfigWordEditor(Memory &memory, uint ci, bool withWordEditor, QWidget *parent)
  : MemoryEditor(MemoryConfig, memory, parent, "pic_config_word_editor"), _configIndex(ci)
{
  if (withWordEditor) {
    QHBoxLayout *hbox = new QHBoxLayout(_top);
    _mdb = new MemoryRangeEditor(MemoryConfig, memory, 1, 1, ci, 1, this);
    _mdb->init();
    connect(_mdb, SIGNAL(modified()), SIGNAL(modified()));
    connect(_mdb, SIGNAL(modified()), SLOT(updateDisplay()));
    hbox->addWidget(_mdb);
    KPushButton *button = new KPushButton(i18n("Details..."), this);
    button->setFixedHeight(button->sizeHint().height());
    connect(button, SIGNAL(clicked()), SLOT(showDialog()));
    hbox->addWidget(button);
    hbox->addStretch(1);
  } else _mdb = 0;

  QGridLayout *grid = new QGridLayout(_top);
  grid->setColStretch(2, 1);
  const Config::Word &cword = device().config()._words[ci];
  _combos.resize(cword.masks.count());
  uint nbChars = device().nbCharsWord(MemoryConfig);
  for (uint k=0; k<_combos.count(); k++) {
    const Config::Mask &cmask = cword.masks[k];
    QLabel *label = new QLabel(Config::maskLabel(cmask.name) + ":", this);
    grid->addWidget(label, k, 0);
    label = new QLabel(cmask.name, this);
    grid->addWidget(label, k, 1);
    _combos[k] = new ConfigWordComboBox(this);
    for (uint i=0; i<cmask.values.count(); i++) {
      if ( !cmask.values[i].isValid() ) continue;
      QString label = Config::valueLabel(cmask.name, cmask.values[i].name);
      label += " (" + toHexLabel(cmask.values[i].value, nbChars) + ")";
      _combos[k]->appendItem(label, i);
    }
    connect(_combos[k], SIGNAL(activated(int)), SLOT(slotModified()));
    grid->addWidget(_combos[k], k, 2);
  }
}

void Pic::ConfigWordEditor::setReadOnly(bool readOnly)
{
  if (_mdb) _mdb->setReadOnly(readOnly);
  const Config::Word &cword = device().config()._words[_configIndex];
  for (uint k=0; k<_combos.count(); k++) {
    const Config::Mask &cmask = cword.masks[k];
    _combos[k]->setEnabled(!readOnly && !(cmask.value & cword.pmask) && cmask.values.count()!=1);
  }
}

void Pic::ConfigWordEditor::slotModified()
{
  uint v = memory().word(MemoryConfig, _configIndex);
  //qDebug("BinWordEditor::slotModified %i: %s", _configIndex, toHex(v, 4).data());
  for (uint k=0; k<_combos.count(); k++) {
    const Config::Mask &cmask = device().config()._words[_configIndex].masks[k];
    v &= ~cmask.value; // clear masked bits
    v |= cmask.values[_combos[k]->index()].value; // set value
  }
  memory().setWord(MemoryConfig, _configIndex, v);
  //qDebug("  now: %s", toHex(v, 4).data());
  if (_mdb) _mdb->updateDisplay();
  emit modified();
}

void Pic::ConfigWordEditor::updateDisplay()
{
  uint v = memory().word(MemoryConfig, _configIndex);
  uint nbChars = device().nbCharsWord(MemoryConfig);
  //qDebug("BinWordEditor::updateDisplay %i: %s", _configIndex, toHex(v, 4).data());
  for (uint k=0; k<_combos.count(); k++) {
    const Config::Mask &cmask = device().config()._words[_configIndex].masks[k];
    for (int i=cmask.values.count()-1; i>=0; i--) {
      if ( (v & cmask.values[i].value)==cmask.values[i].value ) {
        if ( cmask.values[i].isValid() ) _combos[k]->setItem(i);
        else {
          QString label = i18n("<invalid>") + " (" + toHexLabel(cmask.values[i].value, nbChars) + ")";
          _combos[k]->setInvalidItem(i, label);
        }
        break;
      }
    }
  }
  if (_mdb) _mdb->updateDisplay();
}

void Pic::ConfigWordEditor::showDialog()
{
  ConfigWordDialog dialog(memory(), _configIndex, this);
  dialog.exec();
}
