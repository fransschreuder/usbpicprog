/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "register_view.h"

#include "libgui/main_global.h"
#include "libgui/gui_debug_manager.h"

//----------------------------------------------------------------------------
Register::PortBitListViewItem::PortBitListViewItem(uint index, uint bit, KListViewItem *parent)
  : KListViewItem(parent), _index(index), _bit(bit)
{
  const Device::RegistersData *rdata = Main::deviceData()->registersData();
  setText(1, rdata->portBitName(_index, _bit));
  setSelectable(false);
}

void Register::PortBitListViewItem::updateView()
{
  const QMap<uint, Device::PortBitData> &pdata = Register::list().portData(_index);
  QString text;
  if ( pdata.isEmpty() ) text = "--";
  else {
/*
    switch (pdata[_bit].state) {
      case Device::High: text = " 1"; break;
      case Device::Low: text = " 0"; break;
      case Device::WeakPullUp: text = "w1"; break;
      case Device::WeakPullDown: text = "w0"; break;
      case Device::HighImpedance: text = "HZ"; break;
      case Device::Unknown: text = "  "; break;
    }
*/
    if ( pdata[_bit].drivingState==Device::IoUnknown ) text = " ";
    else text += (pdata[_bit].drivingState==Device::IoHigh ? "1" : "0");
    text += (pdata[_bit].driving ? " > " : " < ");
    if ( pdata[_bit].drivenState==Device::IoUnknown ) text += " ";
    else text += (pdata[_bit].drivenState==Device::IoHigh ? "1" : "0");
  }
  setText(2, text);
  repaint();
}

void Register::PortBitListViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align)
{
  QColorGroup ncg = cg;
  const QMap<uint, Device::PortBitData> &data = Register::list().portData(_index);
  const QMap<uint, Device::PortBitData> &odata = Register::list().oldPortData(_index);
  bool changed = ( !data.isEmpty() && data[_bit]!=odata[_bit] );
  if ( column==2 && changed ) ncg.setColor(QColorGroup::Text, red);
  KListViewItem::paintCell(p, ncg, column, width, align);
}

QString Register::PortBitListViewItem::tooltip(int col) const
{
  if ( col!=2 ) return QString::null;
  const QMap<uint, Device::PortBitData> &pdata = Register::list().portData(_index);
  if ( pdata.isEmpty() ) return QString::null;
  QString s = text(1) + ": ";
  if (pdata[_bit].driving) {
    if ( pdata[_bit].drivingState!=Device::IoUnknown ) s += i18n("unknown state");
    else s += (pdata[_bit].drivingState==Device::IoHigh ? i18n("driving high") : i18n("driving low"));
    s += i18n(" (output)");
  } else {
    if ( pdata[_bit].drivenState==Device::IoUnknown ) s += i18n("unknown state");
    else s += (pdata[_bit].drivenState==Device::IoHigh ? i18n("driven high") : i18n("driven low"));
    s += i18n(" (input)");
  }
  return s;
}

//-----------------------------------------------------------------------------
Register::ListViewItem::ListViewItem(const TypeData &data, KListViewItem *parent)
  : EditListViewItem(parent), _data(data), _base(NumberBase::Hex)
{
  setSelectable(false);
  const Device::RegistersData *rdata = Main::deviceData()->registersData();
  if ( data.type()==Regular && rdata ) {
    int i = rdata->portIndex(data.address());
    if ( i!=-1 ) {
      for (int k=Device::MAX_NB_PORT_BITS-1; k>=0; k--) {
        if ( !rdata->hasPortBit(i, k) ) continue;
        PortBitListViewItem *item = new PortBitListViewItem(i, k, this);
        _items.append(item);
      }
    }
  }
}

QString Register::ListViewItem::text() const
{
  BitValue value = Register::list().value(_data);
  uint nbChars = convertNbChars(_data.nbChars(), NumberBase::Hex, _base);
  return (value.isInitialized() ? toLabel(_base, value, nbChars) : "--");
}

int Register::ListViewItem::compare(QListViewItem *item, int, bool) const
{
  const TypeData &data = static_cast<ListViewItem *>(item)->data();
  int i1 = list().watched().findIndex(data);
  Q_ASSERT( i1!=-1 );
  int i2 = list().watched().findIndex(_data);
  Q_ASSERT( i2!=-1 );
  return ( i1-i2 );
}

void Register::ListViewItem::updateView()
{
  if ( _data.type()!=Special ) setText(0, toHexLabel(_data.address(), nbCharsAddress()) + ":");
  setText(1, label());
  setText(2, text());
  repaint();
  for (uint i=0; i<_items.count(); i++) _items[i]->updateView();
}

void Register::ListViewItem::setBase(NumberBase base)
{
  _base = base;
  updateView();
}

void Register::ListViewItem::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align)
{
  QColorGroup ncg = cg;
  BitValue value = Register::list().value(_data);
  BitValue oldValue = Register::list().oldValue(_data);
  if ( column==2 && value!=oldValue ) ncg.setColor(QColorGroup::Text, red);
  EditListViewItem::paintCell(p, ncg, column, width, align);
}

QString Register::ListViewItem::tooltip(int col) const
{
  if ( col!=2 ) return QString::null;
  BitValue value = Register::list().value(_data);
  if ( !value.isInitialized() ) return QString::null;
  BitValue v = value;
  QString s;
  for (uint i=0; i<_data.nbChars(); i++) {
    char c = v.nybble(i);
    if ( isprint(c) ) s = c + s;
    else s = "." + s;
  }
  return QString("%1 - %2 - \"%3\"").arg(toHexLabel(value, _data.nbChars()))
         .arg(toLabel(NumberBase::Bin, value, 4*_data.nbChars())).arg(s);
}

QWidget *Register::ListViewItem::editWidgetFactory(int col) const
{
  if ( col!=2 || Main::programmerState()!=Programmer::Halted ) return 0;
  return new NumberLineEdit(text(), 0);
}

void Register::ListViewItem::editDone(int col, const QWidget *edit)
{
  if ( col!=2 ) return;
  bool ok;
  ulong value = static_cast<const NumberLineEdit *>(edit)->value(&ok);
  if (ok) Debugger::manager->writeRegister(_data, value);
}

//----------------------------------------------------------------------------
Register::LineEdit::LineEdit(QWidget *parent, const char *name)
  : NumberLineEdit(parent, name), _base(NumberBase::Nb_Types)
{
  connect(this, SIGNAL(lostFocus()), SLOT(updateText()));
  connect(this, SIGNAL(returnPressed()), SLOT(returnPressedSlot()));
}

void Register::LineEdit::updateText()
{
  setText(_value.isInitialized() ? toLabel(_base, _value, _nbChars) : "--");
  uint w = 2*frameWidth() + maxLabelWidth(_base, _nbChars, font());
  setFixedWidth(w+5);
  setFixedHeight(minimumSizeHint().height());
}

void Register::LineEdit::setValue(NumberBase base, BitValue value, uint nbChars)
{
  _base = base;
  _value = value;
  _nbChars = nbChars;
  updateText();
}

void Register::LineEdit::returnPressedSlot()
{
  bool ok;
  uint v = fromAnyLabel(text(), &ok);
  if (ok) _value = v;
  updateText();
  emit modified();
}

void Register::LineEdit::keyPressEvent(QKeyEvent *e)
{
  if ( e->key()==Key_Escape ) clearFocus();
  else NumberLineEdit::keyPressEvent(e);
}

//----------------------------------------------------------------------------
Register::View::View(QWidget *parent, const char *name)
 : QFrame(parent, name), GenericView(list())
{}
