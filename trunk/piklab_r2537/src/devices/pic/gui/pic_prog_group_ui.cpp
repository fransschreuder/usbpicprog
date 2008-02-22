/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_prog_group_ui.h"

#include "progs/gui/prog_config_widget.h"
#include "progs/base/prog_group.h"

Programmer::PicAdvancedDialog::PicAdvancedDialog(PicBase &base, QWidget *parent, const char *name)
  : AdvancedDialog(base, parent, name)
{
  if (_voltagesContainer) {
    uint k = _voltagesContainer->numRows();
    for (uint i=0; i<Pic::Nb_VoltageTypes; i++) {
      if ( !base.group().canReadVoltage(Pic::VoltageType(i)) ) _voltages[i] = 0;
      else {
        QLabel *label = new QLabel(i18n(Pic::VOLTAGE_TYPE_LABELS[i]) + ":", _voltagesContainer);
        _voltagesContainer->addWidget(label, k, 0);
        _voltages[i] = new QLabel(_voltagesContainer);
        _voltagesContainer->addWidget(_voltages[i], k, 1);
        k++;
      }
    }
  }
}

void Programmer::PicAdvancedDialog::updateDisplay()
{
  ::Programmer::AdvancedDialog::updateDisplay();
  for (uint i=0; i<Pic::Nb_VoltageTypes; i++) {
    if ( !base().group().canReadVoltage(Pic::VoltageType(i)) ) continue;
    double v = base().voltage(Pic::VoltageType(i));
    if ( v==::Programmer::UNKNOWN_VOLTAGE ) _voltages[i]->setText("---");
    else _voltages[i]->setText(QString("%1 V").arg(v));
  }
}
