/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "config_gen.h"

#include <qlayout.h>
#include <qlabel.h>
#include <klocale.h>

#include "device_gui.h"
#include "devices/base/generic_memory.h"
#include "devices/gui/hex_view.h"
#include "devices/gui/memory_editor.h"
#include "devices/base/device_group.h"
#include "devices/gui/device_group_ui.h"
#include "devices/list/device_list.h"
#include "text_editor.h"
#include "tools/list/tool_list.h"

//-----------------------------------------------------------------------------
GeneratorDialog::GeneratorDialog(const QString &title, QWidget *parent, const char *name)
  : Dialog(parent, name, true, title, Close|User1, Close, false, QSize(400, 300))
{
  QVBoxLayout *top = new QVBoxLayout(mainWidget(), 10, 10);

  QHBoxLayout *hbox = new QHBoxLayout(top);
  QLabel *label = new QLabel(i18n("Device:"), mainWidget());
  hbox->addWidget(label);
  _deviceChooser = new DeviceChooser::Button(false, mainWidget());
  connect(_deviceChooser, SIGNAL(changed()), SLOT(reset()));
  hbox->addWidget(_deviceChooser);
  hbox->addSpacing(20);

  label = new QLabel(i18n("Toolchain:"), mainWidget());
  hbox->addWidget(label);
  _configType = new KeyComboBox(mainWidget());
  Tool::Lister::ConstIterator it;
  for (it=Tool::lister().begin(); it!=Tool::lister().end(); ++it)
    _configType->appendItem(it.key(), it.data()->label());
  connect(_configType, SIGNAL(activated()), SLOT(typeChanged()));
  hbox->addWidget(_configType);

  label = new QLabel(i18n("Tool Type:"), mainWidget());
  hbox->addWidget(label);
  _toolType = new KeyComboBox(mainWidget());
  FOR_EACH(PURL::ToolType, type) _toolType->appendItem(type.key(), i18n(type.label()));
  _toolType->fixMinimumWidth();
  connect(_toolType, SIGNAL(activated()), SLOT(compute()));
  hbox->addWidget(_toolType);
  hbox->addStretch(1);

  _hbox = new QHBoxLayout(top);

  _text = new SimpleTextEditor(false, PURL::Nb_FileTypes, mainWidget());
  _text->setReadOnly(true);
  top->addWidget(_text);

  _warning = new QLabel(mainWidget());
  top->addWidget(_warning);

  setButtonText(User1, i18n("Copy to clipboard"));
}

void GeneratorDialog::set(const Device::Data *data, const Tool::Group &group, PURL::ToolType stype)
{
  QString device;
  if ( data==0 ) {
    QValueVector<QString> devices = group.supportedDevices();
    if ( devices.isEmpty() ) return;
    device = devices[0];
  } else device = data->name();
  _deviceChooser->setDevice(device);
  _configType->setCurrentItem(group.name());
  setToolType(stype);
  reset();
}

void GeneratorDialog::typeChanged()
{
  setToolType(PURL::ToolType::Nb_Types);
  compute();
}

void GeneratorDialog::setToolType(PURL::ToolType stype)
{
  const Tool::Group *group = Tool::lister().group(_configType->currentItem());
  _toolType->clear();
  FOR_EACH(PURL::ToolType, type) {
    if ( group->implementationType(type)==PURL::Nb_FileTypes ) continue;
    _toolType->appendItem(type.key(), i18n(type.label()));
  }
  _toolType->setCurrentItem(stype.key());
  _toolType->setEnabled( _toolType->count()>=2 );
}

PURL::ToolType GeneratorDialog::toolType() const
{
  return PURL::ToolType::fromKey(_toolType->currentItem());
}

void GeneratorDialog::compute()
{
  const Tool::Group *group = Tool::lister().group(_configType->currentItem());
  _warning->hide();
  if ( group->isSupported(_deviceChooser->device()) ) {
    const Tool::SourceGenerator *generator = group->sourceGenerator();
    if ( generator==0 ) {
      _text->setFileType(PURL::Nb_FileTypes);
      _text->setText(i18n("Generation is not supported yet for the selected toolchain or device."));
    } else {
      bool ok = true;
      PURL::FileType type = group->implementationType(toolType());
      SourceLine::List lines = generateLines(ok);
      if ( ok && lines.isEmpty() ) {
        _text->setFileType(PURL::Nb_FileTypes);
        _text->setText(i18n("This toolchain does not need explicit config bits."));
      } else {
        _text->setFileType(type);
        _text->setText(SourceLine::text(PURL::DATA[type].sourceFamily, lines, 4));
      }
      if ( !ok ) {
        _warning->show();
        _warning->setText(i18n("Generation is only partially supported for this device."));
      }
    }
  } else {
   _text->setFileType(PURL::Nb_FileTypes);
    if ( group->supportedDevices().isEmpty() ) _text->setText(i18n("Could not detect supported devices for selected toolchain. Please check installation."));
    else _text->setText(i18n("Device not supported by the selected toolchain."));
  }
}

void GeneratorDialog::slotUser1()
{
  _text->selectAll();
  _text->copy();
  _text->deselect();
}

//-----------------------------------------------------------------------------
ConfigGenerator::ConfigGenerator(QWidget *parent)
  : GeneratorDialog(i18n("Config Generator"), parent, "config_generator"), _memory(0), _configEditor(0)
{}

ConfigGenerator::~ConfigGenerator()
{
  delete _memory;
}

void ConfigGenerator::reset()
{
  delete _memory;
  const Device::Data *data = Device::lister().data(_deviceChooser->device());
  _memory = data->group().createMemory(*data);
  delete _configEditor;
  _configEditor = Device::groupui(*data).createConfigEditor(*_memory, mainWidget());
  if (_configEditor) {
    _configEditor->show();
    connect(_configEditor, SIGNAL(modified()), SLOT(compute()));
    _configEditor->updateDisplay();
    _hbox->addWidget(_configEditor);
  }
  compute();
}

SourceLine::List ConfigGenerator::generateLines(bool &ok) const
{
  const Tool::Group *group = Tool::lister().group(_configType->currentItem());
  const Tool::SourceGenerator *generator = group->sourceGenerator();
  Q_ASSERT(generator);
  return generator->configLines(toolType(), *_memory, ok);
}

//-----------------------------------------------------------------------------
TemplateGenerator::TemplateGenerator(QWidget *parent)
  : GeneratorDialog(i18n("Template Generator"), parent, "template_generator")
{}

SourceLine::List TemplateGenerator::generateLines(bool &ok) const
{
  const Tool::Group *group = Tool::lister().group(_configType->currentItem());
  const Tool::SourceGenerator *generator = group->sourceGenerator();
  Q_ASSERT(generator);
  const Device::Data *data = Device::lister().data(_deviceChooser->device());
  return generator->templateSourceFile(toolType(), *data, ok);
}
