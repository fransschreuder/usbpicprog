/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "icd2_group_ui.h"

#include "progs/gui/prog_config_widget.h"
#include "progs/base/prog_group.h"
#include "progs/icd2/base/icd2_debug.h"

//----------------------------------------------------------------------------
Icd2::AdvancedDialog::AdvancedDialog(ProgrammerBase &base, QWidget *parent)
  : ::Programmer::PicAdvancedDialog(base, parent, "icd2_advanced_dialog")
{
  uint row = _firmwareContainer->numRows();
  QLabel *label = new QLabel(i18n("Id:"), _firmwareContainer);
  _firmwareContainer->addWidget(label, row, 0);
  _firmwareIdLabel = new QLabel(_firmwareContainer);
  _firmwareContainer->addWidget(_firmwareIdLabel, row, 1);
  row++;

  row = _programmerContainer->numRows();
  if ( base.group().properties() & ::Programmer::Debugger ) {
    ButtonContainer *container = new ::Programmer::ButtonContainer(i18n("Debug Executive"), this, SLOT(updateDebugExecutive()), _programmerContainer);
    _programmerContainer->addMultiCellWidget(container, row,row, 0,1);
    label = new QLabel(i18n("Version:"), container);
    container->addWidget(label, 1, 0);
    _debugExecLabel = new QLabel(container);
    container->addWidget(_debugExecLabel, 1, 1);
    row++;
  } else _debugExecLabel = 0;

  for (uint i=0; i<TestData::Nb_VoltageTypes; i++) {
    QLabel *label = new QLabel(i18n(TestData::VOLTAGE_LABELS[i]) + ":", _selfTestContainer);
    _selfTestContainer->addWidget(label, 1+i, 0);
    _tests[i] = new QLabel(_selfTestContainer);
    _selfTestContainer->addWidget(_tests[i], 1+i, 1);
  }
}

void Icd2::AdvancedDialog::updateDebugExecutive()
{
  ::BusyCursor bc;
  if ( ensureConnected() ) {
    Pic::TargetMode mode;
    if ( !base().getTargetMode(mode) ) return;
    if ( mode==Pic::TargetInProgramming )
      MessageBox::sorry(i18n("You need to initiate debugging to read the debug executive version."), Log::Show);
    else static_cast<DebugProgrammer &>(base()).readDebugExecutiveVersion();
  }
  updateDisplay();
}

void Icd2::AdvancedDialog::updateDisplay()
{
  ::Programmer::PicAdvancedDialog::updateDisplay();
  uchar id = base().firmwareId();
  _firmwareIdLabel->setText(id==0 ? "---" : toHexLabel(id, 2));
  if (_debugExecLabel) {
    const VersionData &vd = static_cast<DebugProgrammer &>(base()).debugExecutiveVersion();
    _debugExecLabel->setText(vd.isValid() ? vd.pretty() : "---");
  }
  for (uint i=0; i<TestData::Nb_VoltageTypes; i++)
    _tests[i]->setText(base().testData().result(TestData::VoltageType(i)));
}

//----------------------------------------------------------------------------
::Programmer::ConfigWidget *Icd2::GroupUI::createConfigWidget(QWidget *parent) const
{
  return new ::Programmer::ConfigWidget(static_cast<const Group &>(group()), parent, "icd2_config_widget");
}

::Programmer::AdvancedDialog *Icd2::GroupUI::createAdvancedDialog(::Programmer::Base &base, QWidget *parent) const
{
  return new AdvancedDialog(static_cast<ProgrammerBase &>(base), parent);
}
