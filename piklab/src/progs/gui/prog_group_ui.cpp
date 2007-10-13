/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "prog_group_ui.h"

#include <klocale.h>
#include <kaction.h>
#include <kiconloader.h>

#include "common/common/misc.h"
#include "common/global/purl.h"
#include "common/gui/purl_gui.h"
#include "progs/base/generic_prog.h"
#include "progs/base/prog_group.h"
#include "progs/base/prog_config.h"
#include "libgui/main_global.h"
#include "libgui/toplevel.h"
#include "devices/base/device_group.h"
#include "devices/pic/pic/pic_prog.h"

//----------------------------------------------------------------------------
Programmer::StandaloneMemoryCalibrationEditor::StandaloneMemoryCalibrationEditor(const Pic::Memory &memory, QWidget *parent)
  : Pic::MemoryCalibrationEditor(0, const_cast<Pic::Memory &>(memory), parent)
{}

void Programmer::StandaloneMemoryCalibrationEditor::init(bool first)
{
  Pic::MemoryCalibrationEditor::init(first);
  KAction *action = new KAction(i18n("Read"), "reload", 0, this, SIGNAL(updateCalibration()), Main::toplevel().actionCollection());
  addAction(action);
  action = new KAction(i18n("Regenerate..."), 0, 0, this, SIGNAL(regenerate()), Main::toplevel().actionCollection());
  addAction(action);
}

//----------------------------------------------------------------------------
Programmer::ButtonContainer::ButtonContainer(const QString &title,
                                    QObject *receiver, const char *updateSlot, QWidget *parent)
  : ::ButtonContainer(title, parent)
{
  if (receiver) button().appendAction(i18n("Read"), "reload", receiver, updateSlot);
}

//----------------------------------------------------------------------------
Programmer::AdvancedDialog::AdvancedDialog(Base &base, QWidget *parent, const char *name)
  : Dialog(IconList, i18n("Advanced Dialog"), Close, Close, parent, name, true, false),
    _base(base), _calEditor(0)
{
  // programmer
  KIconLoader loader;
  QPixmap pixmap = loader.loadIcon("piklab_burnchip", KIcon::Toolbar, KIcon::SizeMedium);
  QFrame *page = addPage(i18n("Programmer"), _base.group().label(), pixmap);
  QVBoxLayout *vbox = new QVBoxLayout(page);
  _programmerContainer = new Container(page);
  vbox->addWidget(_programmerContainer);

  Properties properties = _base.group().properties();
  uint row = _programmerContainer->numRows();
  if ( properties & HasFirmware ) {
    _firmwareContainer = new ButtonContainer(i18n("Firmware"), this, SLOT(updateFirmware()), _programmerContainer);
    _programmerContainer->addMultiCellWidget(_firmwareContainer, row,row, 0,1);
    if ( _base.group().properties() & CanUploadFirmware )
      _firmwareContainer->button().appendAction(i18n("Upload..."), "piklab_burnchip", this, SLOT(uploadFirmware()));
    QLabel *label = new QLabel(i18n("Version:"), _firmwareContainer);
    _firmwareContainer->addWidget(label, 1, 0);
    _firmwareLabel = new QLabel(_firmwareContainer);
    _firmwareContainer->addWidget(_firmwareLabel, 1, 1);
    row++;
  } else {
    _firmwareContainer = 0;
    _firmwareLabel = 0;
  }

  if ( _base.group().canReadVoltages() ) {
    _voltagesContainer = new ButtonContainer(i18n("Voltages"), this, SLOT(updateVoltages()), _programmerContainer);
    _programmerContainer->addMultiCellWidget(_voltagesContainer, row,row, 0,1);
    row++;
  } else _voltagesContainer = 0;

  if ( properties & HasSelfTest ) {
    _selfTestContainer = new ButtonContainer(i18n("Self-test"), this, SLOT(updateSelfTest()), _programmerContainer);
    _programmerContainer->addMultiCellWidget(_selfTestContainer, row,row, 0,1);
    row++;
  } else _selfTestContainer = 0;

  // calibration
  pixmap = loader.loadIcon("configure", KIcon::Toolbar, KIcon::SizeMedium);
  page = addPage(i18n("Calibration"), i18n("Calibration"), pixmap);
  vbox = new QVBoxLayout(page);
  _calibrationContainer = new Container(page);
  vbox->addWidget(_calibrationContainer);
  const Device::Data *data = Main::deviceData();
  QString s;
  if ( data==0 ) s = i18n("No device selected");
  else if ( data->group().name()!="pic" || !static_cast<const Pic::Data *>(data)->isReadable(Pic::MemoryRangeType::Cal) )
    s = i18n("This device has no calibration information.");
  else if ( !_base.group().isSupported(data->name()) ) s = i18n("The selected device is not supported by this programmer.");
  else {
    const ::Programmer::PicBase &pbase = static_cast<const ::Programmer::PicBase &>(_base);
    _calEditor = new StandaloneMemoryCalibrationEditor(pbase.deviceMemory(), _calibrationContainer);
    connect(_calEditor, SIGNAL(updateCalibration()), SLOT(updateCalibration()));
    connect(_calEditor, SIGNAL(regenerate()), SLOT(regenerateCalibration()));
    _calEditor->init(true);
    _calEditor->setReadOnly(true);
    _calibrationContainer->addWidget(_calEditor, 0, 0);
  }
  if ( !s.isEmpty() ) {
    QLabel *label = new QLabel(s, _calibrationContainer);
    _calibrationContainer->addWidget(label, 0, 0);
  }
}

bool Programmer::AdvancedDialog::connectProgrammer()
{
  _base.disconnectHardware();
  if ( !_base.connectHardware() ) {
    MessageBox::sorry(i18n("Could not connect programmer."), Log::Show);
    return false;
  }
  return true;
}

bool Programmer::AdvancedDialog::ensureConnected()
{
  if ( (_base.state()==NotConnected || _base.state()==Running) && !connectProgrammer() ) return false;
  Config config;
  return _base.setTargetPowerOn(!config.isTargetSelfPowered());
}

void Programmer::AdvancedDialog::updateFirmware()
{
  ::BusyCursor bc;
  if ( ensureConnected() ) _base.readFirmwareVersion();
  updateDisplay();
}

void Programmer::AdvancedDialog::uploadFirmware()
{
  PURL::Url url = PURL::getOpenUrl(":open_firmware", PURL::filter(PURL::Hex), this ,i18n("Open Firmware"));
  if ( url.isEmpty() ) return;
  ::BusyCursor bc;
  if ( !connectProgrammer() ) return;
  if ( _base.uploadFirmware(url) )
    MessageBox::information(i18n("Firmware uploaded successfully."), Log::Show);
  else MessageBox::sorry(i18n("Error uploading firmware."), Log::Show);
  updateFirmware();
}

void Programmer::AdvancedDialog::updateVoltages()
{
  ::BusyCursor bc;
  if ( ensureConnected() ) _base.readVoltages();
  updateDisplay();
}

void Programmer::AdvancedDialog::updateSelfTest()
{
  ::BusyCursor bc;
  if ( ensureConnected() ) _base.selfTest(false);
  updateDisplay();
}

void Programmer::AdvancedDialog::updateCalibration()
{
  ::BusyCursor bc;
  if ( ensureConnected() ) static_cast< ::Programmer::PicBase &>(_base).readCalibration();
  updateDisplay();
}

void Programmer::AdvancedDialog::regenerateCalibration()
{
  MessageBox::sorry(i18n("Osccal regeneration is not available with the selected programmer."), Log::Show);
}

void Programmer::AdvancedDialog::updateDisplay()
{
  if (_firmwareLabel) _firmwareLabel->setText(_base.firmwareVersion().pretty());
  if (_calEditor) _calEditor->updateDisplay();
}
