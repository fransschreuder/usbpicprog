/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "device_editor.h"

#include <qscrollview.h>
#include <qregexp.h>

#include "devices/list/device_list.h"
#include "toplevel.h"
#include "gui_debug_manager.h"
#include "project_manager.h"
#include "common/global/pfile.h"
#include "main_global.h"

DeviceEditor::DeviceEditor(const QString &title, const QString &tag, QWidget *parent, const char *name)
  : Editor(title, tag, parent, name), _view(0)
{
  init();
}

DeviceEditor::DeviceEditor(QWidget *parent, const char *name)
  : Editor(parent, name), _view(0)
{
  init();
}

void DeviceEditor::init()
{
  QHBoxLayout *hbox = new QHBoxLayout(this, 0);
  QScrollView *sview = new QScrollView(this, "scroll_view");
  sview->setResizePolicy(QScrollView::AutoOneFit);
  hbox->addWidget(sview);
  _widget = new QWidget(sview->viewport(), "main_scroll_widget");
  sview->addChild(_widget);
  _top = new QVBoxLayout(_widget, 0, 0);
  _labelDevice = new QLabel(_widget);
  _labelDevice->setMargin(10);
  _labelDevice->setTextFormat(RichText);
  _top->addWidget(_labelDevice);
  _labelWarning = new QLabel(_widget);
  _labelWarning->setMargin(10);
  _labelWarning->setTextFormat(RichText);
  _top->addWidget(_labelWarning);
  _vbox = new QVBoxLayout(_top);

  connect(&Main::toplevel(), SIGNAL(stateChanged()), SLOT(updateDevice()));
}

void DeviceEditor::setDevice(bool force)
{
  if ( Main::device()==Device::AUTO_DATA.name ) {
    PURL::Url url = Main::projectManager().projectUrl();
    QString name = guessDeviceFromFile(url);
    if ( !force && name==_device ) return;
    _device = name;
    if ( name==Device::AUTO_DATA.name )
      _labelDevice->setText(i18n("The target device is not configured and cannot be guessed from source file. "
                                 "The source file either cannot be found or does not contain any processor directive."));
    else _labelDevice->setText(i18n("Device guessed from file: %1").arg(name));
    _labelDevice->show();
  } else {
    if ( !force && Main::device()==_device ) return;
    _device = Main::device();
    _labelDevice->hide();
  }
  if ( _view && isModified() ) {
    if ( MessageBox::questionYesNo(i18n("File %1 not saved.").arg(filename()), KStdGuiItem::save(), KStdGuiItem::discard()) )
      Editor::save();
  }
  _labelWarning->hide();
  const Device::Data *data = Device::lister().data(_device);
  delete _view;
  _view = createView(data, _widget);
  if (_view) {
    _view->show();
    _vbox->addWidget(_view);
    updateGeometry();
  }
  setModified(false);
  emit guiChanged();
}

PURL::Url DeviceEditor::findAsmFile(const PURL::Url &url)
{
  PURL::SourceFamily family = PURL::DATA[url.fileType()].sourceFamily;
  if ( family.data()->toolType==PURL::ToolType::Assembler ) return url;
  for (uint i=0; i<PURL::Nb_FileTypes; i++) {
    const PURL::Data &data = PURL::DATA[PURL::FileType(i)];
    if ( data.sourceFamily.data()->toolType!=PURL::ToolType::Assembler ) continue;
    for (uint k=0; data.extensions[k]; k++) {
      PURL::Url src = url.toExtension(data.extensions[k]);
      if ( PURL::findExistingUrl(src) ) return src;
    }
  }
  return PURL::Url();
}

QString DeviceEditor::guessDeviceFromFile(const PURL::Url &url)
{
  PURL::Url src = findAsmFile(url);
  Log::StringView sview;
  PURL::File file(src, sview);
  if ( src.isEmpty() || !file.openForRead() ) return Device::AUTO_DATA.name;

  QString device;
  //   QRegExp re1("^[ \\t]+(?:PROCESSOR|processor)[ \\t]+((?:p|sx|P|SX)[a-z0-9A-Z]+)" ) ;
  QRegExp re1("^[ \\t]+(?:PROCESSOR|processor)[ \\t]+([a-z0-9A-Z]+)" ) ;
  QRegExp re2("^[ \\t]+(?:LIST|list)[ \\t]+" ) ;
  for (;;) {
    QString line = file.readLine();
    if ( line.isNull() ) break;
    // search PROCESSOR directive
    if ( re1.search(line, 0)!=-1 ) {
      device = re1.cap(1);
      break;
    }
    // search LIST p=... directive
    int k = re2.search(line,0);
    if ( k!=-1 ) {
      //QRegExp re3("(?:p|P)[ \\t]*=[ \\t]*((?:p|sx|P|SX)[a-z0-9A-Z]+)") ;
      QRegExp re3("(?:p|P)[ \\t]*=[ \\t]*([a-z0-9A-Z]+)") ;
      if ( re3.search(line, k+5)!=-1 ) {
        device = re3.cap(1);
        break;
      }
    }
  }
  device = device.upper();
  if( device[0]=='P') return device.mid(1);
  if ( Device::lister().data(device)==0 ) return Device::AUTO_DATA.name;
  return device;
}
