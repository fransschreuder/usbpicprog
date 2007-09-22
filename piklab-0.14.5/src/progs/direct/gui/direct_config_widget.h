/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DIRECT_CONFIG_WIDGET
#define DIRECT_CONFIG_WIDGET

#include <knuminput.h>

#include "progs/gui/prog_group_ui.h"
#include "progs/gui/hardware_config_widget.h"
#include "progs/direct/base/direct.h"

namespace Direct
{
//----------------------------------------------------------------------------
class GroupUI : public ::Programmer::GroupUI
{
public:
  virtual ::Programmer::ConfigWidget *createConfigWidget(QWidget *parent) const;
  virtual bool hasAdvancedDialog() const { return false; }
  virtual ::Programmer::AdvancedDialog *createAdvancedDialog(::Programmer::Base &, QWidget *) const { return 0; }
};

//-----------------------------------------------------------------------------
class HardwareConfigWidget : public ::HProgrammer::HardwareConfigWidget
{
Q_OBJECT
public:
  HardwareConfigWidget(::Programmer::Base &base, QWidget *parent, bool edit);
  virtual bool set(const Port::Description &pd, const ::HProgrammer::HardwareData &data);
  virtual ::HProgrammer::HardwareData *data() const;

private slots:
  void slotTestPin();
  void slotPinChanged();
  void updateDataIn();
  void sendBits();
  void slotSendBits();

private:
  QComboBox    *_combos[Nb_PinTypes];
  QCheckBox    *_invcbs[Nb_PinTypes];
  QCheckBox    *_testcbs[Nb_PinTypes];
  QLabel       *_testLabels[Nb_PinTypes];
  KIntNumInput *_delay;
  QPushButton  *_sendBitsButton;
  QTimer       *_timerSendBits, *_timerPollDataOut;

  void sendBits(uint d, int nbb);
  void updateTestPin(PinType ptype);
  void updateTestStatus(PinType ptype, bool on);
  uint pin(PinType ptype) const;
  void updatePin(PinType ptype);
  Hardware *hardware() { return static_cast<Hardware *>(_hardware); }
};

//-----------------------------------------------------------------------------
class ConfigWidget : public ::HProgrammer::ConfigWidget
{
Q_OBJECT
public:
  ConfigWidget(const ::Programmer::Group &group, QWidget *parent);
  virtual ::HProgrammer::HardwareConfigWidget *createHardwareConfigWidget(QWidget *parent, bool edit) const;
};

} // namespace

#endif
