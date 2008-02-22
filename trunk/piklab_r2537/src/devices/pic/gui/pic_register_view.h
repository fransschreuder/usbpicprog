/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC_REGISTER_VIEW_H
#define PIC_REGISTER_VIEW_H

#include <qvaluevector.h>
class QPushButton;
class QCheckBox;
class QLabel;
class QComboBox;

#include "devices/gui/register_view.h"
#include "devices/pic/base/pic.h"
#include "devices/pic/base/pic_register.h"
class PopupButton;
namespace Device { class RegisterHexWordEditor; }

namespace Pic
{
//-----------------------------------------------------------------------------
class BankWidget : public QFrame
{
Q_OBJECT
public:
  BankWidget(uint bank, QWidget *parent);
  void updateView();

private slots:
  void buttonActivated(int id);
  void write();
  void bankChanged();

private:
  enum Id { ReadId, EditId, WatchId };
  class Data {
  public:
    Data() : label(0), button(0), edit(0) {}
    uint address;
    QLabel *alabel, *label;
    PopupButton *button;
    Register::LineEdit *edit;
  };
  uint _bindex;
  QComboBox *_bankCombo;
  QValueVector<Data> _registers;

  uint bank() const;
  uint nbRegisters() const;
  uint indexOffset() const;
  void updateRegisterAddresses();
};

//-----------------------------------------------------------------------------
class RegisterView : public Register::View
{
Q_OBJECT
public:
  RegisterView(QWidget *parent);
  virtual void updateView();

private slots:
  void stopWatchAllRegisters();

private:
  QPushButton *_readAllButton, *_clearAllButton;
  QValueVector<BankWidget *> _banks;
};

//-----------------------------------------------------------------------------
class RegisterListViewItem : public Register::ListViewItem
{
public:
  RegisterListViewItem(const Register::TypeData &data, KListViewItem *parent);

private:
  virtual uint nbCharsAddress() const;
  virtual QString label() const;
};

} // namespace

#endif
