/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "memory_editor.h"

#include <qlabel.h>
#include <qscrollbar.h>
#include <qlayout.h>
#include <qgrid.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <klocale.h>
#include <kpushbutton.h>
#include <kaction.h>

#include "common/common/misc.h"
#include "hex_word_editor.h"
#include "device_group_ui.h"
#include "libgui/toplevel.h"
#include "libgui/main_global.h"
#include "hex_view.h"
#include "libgui/gui_prog_manager.h"

//-----------------------------------------------------------------------------
Device::MemoryEditor::MemoryEditor(Device::Memory *memory, QWidget *parent, const char *name)
  : QFrame(parent, name), _memory(memory)
{
  _top = new QVBoxLayout(this, 5, 10);
}

//-----------------------------------------------------------------------------
Device::MemoryRangeEditor::MemoryRangeEditor(Device::Memory &memory,
                    uint nbLines, uint nbCols, uint offset, int nb,
                    QWidget *parent, const char *name)
  : MemoryEditor(&memory, parent, name),
    _nbLines(nbLines), _nbCols(nbCols), _offset(offset), _nb(nb)
{}

void Device::MemoryRangeEditor::init()
{
  if ( _nb==-1 ) _nb = nbWords();
  uint totalNbLines = _nb / _nbCols;
  if ( (_nb % _nbCols)!=0 ) totalNbLines++;

  QHBoxLayout *hbox = new QHBoxLayout(_top);

  QVBoxLayout *vbox = new QVBoxLayout(hbox);
  QFrame *frame = new QFrame(this);
  frame->setFrameStyle(QFrame::Panel | QFrame::Raised);
  frame->setMargin(5);
  vbox->addWidget(frame);
  vbox->addStretch(1);
  QHBoxLayout *fbox = new QHBoxLayout(frame, 5, 5);
  QGrid *grid = new QGrid(3+_nbCols, QGrid::Horizontal, frame, "memory_range_editor_grid");
  fbox->addWidget(grid);
  grid->setSpacing(0);
  grid->setMargin(3);
  QFont f("courier", font().pointSize());
  grid->setFont(f);
  for (uint k=0; k<_nbLines; ++k) {
    // addresses
    QWidget *w = new QWidget(grid);
    w->setFixedWidth(10);
    _blocks.append(w);
    QLabel *label = new QLabel(toHex(0, 2*_memory->device().nbBytesAddress()), grid, "address_label");
    _addresses.append(label);
    label = new QLabel(":", grid);
    // memory
    for (uint i = 0; i<_nbCols; ++i) {
      HexWordEditor *h = createHexWordEditor(grid);
      _editors.append(h);
      connect(h, SIGNAL(modified()), SIGNAL(modified()));
      connect(h, SIGNAL(moveNext()), SLOT(moveNext()));
      connect(h, SIGNAL(movePrev()), SLOT(movePrev()));
      connect(h, SIGNAL(moveFirst()), SLOT(moveFirst()));
      connect(h, SIGNAL(moveLast()), SLOT(moveLast()));
      connect(h, SIGNAL(moveUp()), SLOT(moveUp()));
      connect(h, SIGNAL(moveDown()), SLOT(moveDown()));
      connect(h, SIGNAL(moveNextPage()), SLOT(moveNextPage()));
      connect(h, SIGNAL(movePrevPage()), SLOT(movePrevPage()));
    }
  }

  //  scrollbar if there are more lines to display than visible
  _scrollbar = new QScrollBar(0, QMAX(_nbLines, totalNbLines)-_nbLines, 1, _nbLines, 0,
                              QScrollBar::Vertical, frame, "memory_range_editor_scrollbar");
  connect(_scrollbar, SIGNAL(valueChanged(int)), SLOT(setIndex(int))) ;
  if ( totalNbLines<=_nbLines ) _scrollbar->hide();
  fbox->addWidget(_scrollbar);
  fbox->addStretch(1);

  vbox->addStretch(1);
  QVBoxLayout *vboxc = new QVBoxLayout(hbox);
  vboxc->setSpacing(0);
  _comment = new QLabel(this);
  _comment->hide();
  vboxc->addWidget(_comment);
  _spacer = new QLabel(this);
  _spacer->setFixedHeight(10);
  _spacer->hide();
  vboxc->addWidget(_spacer);
  addLegend(vboxc);
  vboxc->addStretch(1);
  hbox->addStretch(1);

  setReadOnly(false);
  setIndex(0);
}

void Device::MemoryRangeEditor::setComment(const QString &text)
{
  _comment->setText(text);
  _comment->show();
  _spacer->show();
}

void Device::MemoryRangeEditor::setReadOnly(bool readOnly)
{
  for (uint i=0; i<_editors.count(); i++)
    _editors[i]->setReadOnly(readOnly || isRangeReadOnly());
}

void Device::MemoryRangeEditor::updateDisplay()
{
  setIndex(_scrollbar->value());
}

void Device::MemoryRangeEditor::setStartWord(int index)
{
  setIndex(index / _nbCols / addressIncrement());
}

void Device::MemoryRangeEditor::setEndWord(int index)
{
  uint i = index / _nbCols / addressIncrement();
  i = (i<_nbLines ? 0 : i - _nbLines + 1);
  setIndex(i);
}

void Device::MemoryRangeEditor::setIndex(int index)
{
  _scrollbar->blockSignals(true);
  _scrollbar->setValue(index);
  _scrollbar->blockSignals(false);

  // memory
  for (uint i=0; i<_editors.count(); i++) {
    int offset = wordOffset() + i;
    _editors[i]->setOffset(offset<int(nbWords()) ? offset : -1);
  }

  // adresses
  uint inc = addressIncrement();
  uint address = startAddress() + inc * wordOffset();
  for (uint i=0; i<_addresses.count(); i++) {
    _addresses[i]->setText(toHex(address, 2*_memory->device().nbBytesAddress()));
    updateAddressColor(i, address);
    address += inc * _nbCols;
  }
}

void Device::MemoryRangeEditor::moveNext()
{
  QValueList<HexWordEditor *>::iterator it = _editors.find((HexWordEditor *)sender());
  ++it;
  if ( it==_editors.end() || (*it)->offset()==-1 ) {
    if ( current()==uint(_scrollbar->maxValue()) ) return; // at end
    setIndex(current()+1);
    _editors[_editors.count()-_nbCols]->setFocus();
  } else (*it)->setFocus();
}

void Device::MemoryRangeEditor::movePrev()
{
  QValueList<HexWordEditor *>::iterator it = _editors.find((HexWordEditor *)sender());
  if ( it==_editors.begin() ) {
    if ( current()==0 ) return; // at beginning
    setIndex(current()-1);
    _editors[_nbCols-1]->setFocus();
  } else {
    --it;
    (*it)->setFocus();
  }
}

void Device::MemoryRangeEditor::moveFirst()
{
  if ( _editors[0]==0 ) return;
  setIndex(0);
  _editors[0]->setFocus();
}

void Device::MemoryRangeEditor::moveLast()
{
  if ( _editors.count()==0 ) return;
  setIndex(_scrollbar->maxValue());
  for (uint i=1; i<=_nbCols; i++) {
    if ( _editors[_editors.count()-i]->offset()==-1 ) continue;
    _editors[_editors.count()-i]->setFocus();
    break;
  }
}

void Device::MemoryRangeEditor::moveUp()
{
  int i = _editors.findIndex((HexWordEditor *)sender());
  uint line = i / _nbCols;
  if ( line==0 ) {
    if ( current()==0 ) return; // on first line
    setIndex(current()-1);
    _editors[i]->setFocus();
  } else _editors[i-_nbCols]->setFocus();
}

void Device::MemoryRangeEditor::moveDown()
{
  int i = _editors.findIndex((HexWordEditor *)sender());
  uint line = i / _nbCols;
  if ( line+1==_nbLines ) {
    if ( current()==uint(_scrollbar->maxValue()) ) return; // on last line
    setIndex(current()+1);
    if ( _editors[i]->offset()==-1 ) _editors[i-_nbCols]->setFocus();
    else _editors[i]->setFocus();
  } else _editors[i+_nbCols]->setFocus();
}

void Device::MemoryRangeEditor::moveNextPage()
{
  int i = _editors.findIndex((HexWordEditor *)sender());
  if ( _nbLines>(uint(_scrollbar->maxValue()) - current()) ) i = (_nbLines-1) * _nbCols + (i % _nbCols);
  else setIndex(current()+_nbLines);
  if ( _editors[i]->offset()==-1 ) _editors[i-_nbCols]->setFocus();
  else _editors[i]->setFocus();
}

void Device::MemoryRangeEditor::movePrevPage()
{
  int i = _editors.findIndex((HexWordEditor *)sender());
  if ( current()<_nbLines ) i = (i % _nbCols);
  else setIndex(current()-_nbLines);
  _editors[i]->setFocus();
}

//-----------------------------------------------------------------------------
Device::MemoryEditorGroup::MemoryEditorGroup(Device::Memory *memory, QWidget *parent, const char *name)
  : MemoryEditor(memory, parent, name)
{}

void Device::MemoryEditorGroup::addEditor(MemoryEditor *editor)
{
  connect(editor, SIGNAL(modified()), SIGNAL(modified()));
  _editors.append(editor);
}

void Device::MemoryEditorGroup::setReadOnly(bool readOnly)
{
  for (uint i=0; i<_editors.count(); i++)
    _editors[i]->setReadOnly(readOnly);
}

void Device::MemoryEditorGroup::updateDisplay()
{
  for (uint i=0; i<_editors.count(); i++)
    _editors[i]->updateDisplay();
}

//-----------------------------------------------------------------------------
const Device::ActionData Device::ACTION_DATA[Nb_Actions] = {
  { I18N_NOOP("&Clear"),       "fileclose",         NeedWrite },
  { I18N_NOOP("&Zero"),        0,                   NeedWrite },
  { I18N_NOOP("Re&load"),      "reload",            SeparatorAfter | NeedModified },
  { I18N_NOOP("&Program"),     "piklab_burnchip",   NeedProgrammer },
  { I18N_NOOP("&Verify"),      "piklab_verifychip", NeedProgrammer },
  { I18N_NOOP("&Read"),        "piklab_readchip",   NeedProgrammer | NeedWrite },
  { I18N_NOOP("&Erase"),       "piklab_erasechip",  NeedProgrammer },
  { I18N_NOOP("&Blank Check"), "piklab_blankcheck", NeedProgrammer }
};

Device::MemoryTypeEditor::MemoryTypeEditor(const HexView *hexview, Device::Memory &memory,
                                            QWidget *parent, const char *name)
  : MemoryEditorGroup(&memory, parent, name),
    _title(0), _comment(0), _hexview(hexview)
{
  for (uint i=0; i<Nb_Actions; i++) _actions[i] = 0;
}

void Device::MemoryTypeEditor::init(bool first)
{
  if ( !first ) _top->addWidget(new SeparatorWidget(this));
  QHBoxLayout *hbox = new QHBoxLayout(_top);

  _title = new PopupButton(this);
  for (uint i=0; i<Nb_Actions; i++) {
    if ( hasAction(Action(i)) ) {
      _actions[i] = new KAction(i18n(ACTION_DATA[i].label), ACTION_DATA[i].icon, 0,
                                this, SLOT(doAction()), Main::toplevel().actionCollection());
      addAction(_actions[i]);
    }
    if ( ACTION_DATA[i].properties & SeparatorAfter ) _title->appendSeparator();
  }
  _title->appendSeparator();
  hbox->addWidget(_title);

  _comment = new QLabel(this);
  hbox->addWidget(_comment);
  hbox->addStretch(1);

  connect(&Main::toplevel(), SIGNAL(stateChanged()), SLOT(stateChanged()));
}

void Device::MemoryTypeEditor::addAction(KAction *action)
{
  _title->appendAction(action);
}

void Device::MemoryTypeEditor::doAction()
{
  for (uint i=0; i<Nb_Actions; i++) {
    if ( sender()==_actions[i] ) {
      doAction(Action(i));
      break;
    }
  }
}

void Device::MemoryTypeEditor::doAction(Action action)
{
  if ( (ACTION_DATA[action].properties & NeedProgrammer)
       && !Programmer::manager->initProgramming(false) ) return;
  bool ok = internalDoAction(action);
  if ( ACTION_DATA[action].properties & NeedProgrammer )
    Programmer::manager->endProgramming();
  if (ok) {
    updateDisplay();
    modified();
  }
}

void Device::MemoryTypeEditor::stateChanged()
{
  bool idle = ( Main::state()==Main::Idle );
  for (uint i=0; i<Nb_Actions; i++) {
    if ( _actions[i]==0 ) continue;
    bool on = true;
    if ( (ACTION_DATA[i].properties & NeedProgrammer) && !idle ) on = false;
    if ( (ACTION_DATA[i].properties & NeedWrite) && _readOnly ) on = false;
    if ( (ACTION_DATA[i].properties & NeedModified) && (_hexview==0 || !_hexview->isModified()) ) on = false;
    _actions[i]->setEnabled(on);
  }
}

void Device::MemoryTypeEditor::setReadOnly(bool readOnly)
{
  _readOnly = readOnly;
  MemoryEditorGroup::setReadOnly(readOnly);
  stateChanged();
}

const Device::Memory *Device::MemoryTypeEditor::originalMemory() const
{
  return (_hexview ? _hexview->originalMemory() : 0);
}
