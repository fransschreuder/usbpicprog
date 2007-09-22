/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "device_gui.h"

#include <qlayout.h>
#include <qpainter.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qsplitter.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <klistview.h>
#include <klocale.h>

#include "devices/list/device_list.h"
#include "devices/base/device_group.h"
#include "devices/gui/device_group_ui.h"
#include "progs/list/prog_list.h"
#include "tools/list/device_info.h"
#include "tools/list/tool_list.h"
#include "global_config.h"

namespace DeviceChooser
{
//-----------------------------------------------------------------------------
class ListItem : public KListViewItem
{
public:
  ListItem(KListView *list, const QString &name, bool selectable, bool isDevice)
    : KListViewItem(list, name), _device(isDevice) {
    setSelectable(selectable);
  }
  ListItem(KListViewItem *item, const QString &name)
    : KListViewItem(item, name), _device(true) {}

  bool isDevice() const { return _device; }
  virtual void paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align) {
    QColorGroup ncg = cg;
    if (_device) {
      const Device::Data *device = Device::lister().data(text(0));
      Q_ASSERT(device);
      ncg.setColor(QColorGroup::Text, Device::statusColor(device->status()));
    }
    KListViewItem::paintCell(p, ncg, column, width, align);
  }

private:
  bool _device;
};

} // namespace

//-----------------------------------------------------------------------------
const char * const DeviceChooser::Dialog::LIST_TYPE_LABELS[Nb_ListTypes] = {
  I18N_NOOP("Family Tree"), I18N_NOOP("Flat List")
};

QString DeviceChooser::htmlInfo(const Device::Data &data, const QString &deviceHref,
                                const QString &imagePrefix, QMimeSourceFactory *msf)
{
  return Device::htmlInfo(data, deviceHref, Device::supportedHtmlInfo(data), imagePrefix, msf);
}

DeviceChooser::Dialog::Dialog(const QString &device, Type type, QWidget *parent)
  : ::Dialog(parent, "device_chooser_dialog", true, i18n("Select a device"),
             Ok|Close, Close, false, QSize(400, 300)), _withAuto(type==ChooseWithAuto)
{
  setButtonOK(KGuiItem(i18n( "&Select"), "button_ok"));
  QVBoxLayout *top = new QVBoxLayout(mainWidget(), 0, 10);

  // view
  QHBoxLayout *hbox = new QHBoxLayout(top, 10);
  _listTypeCombo = new QComboBox(mainWidget());
  for (uint i=0; i<Nb_ListTypes; i++)
    _listTypeCombo->insertItem(i18n(LIST_TYPE_LABELS[i]));
  Config config;
  _listTypeCombo->setCurrentItem(config.listType());
  connect(_listTypeCombo, SIGNAL(activated(int)), SLOT(updateList()));
  hbox->addWidget(_listTypeCombo);

  // filters
  QFrame *frame = new QFrame(mainWidget());
  frame->setFrameStyle(QFrame::Panel | QFrame::Raised);
  frame->setMargin(5);
  hbox->addWidget(frame);
  hbox = new QHBoxLayout(frame, 10, 10);
  QLabel *label = new QLabel(i18n("Filters:"), frame);
  hbox->addWidget(label);

  // programmer filter
  _programmerCombo = new QComboBox(frame);
  _programmerCombo->insertItem(i18n("<Programmer>"));
  for (uint i=0; i<Programmer::lister().nbGroups(); i++) {
    const Programmer::Group &group = Programmer::lister().group(i);
    _programmerCombo->insertItem(group.label());
  }
  const Programmer::Group *pgroup = config.programmerGroup();
  if (pgroup) _programmerCombo->setCurrentItem(pgroup->index()+1);
  connect(_programmerCombo, SIGNAL(activated(int)), SLOT(updateList()));
  hbox->addWidget(_programmerCombo);

  // tool filter
  _toolCombo = new QComboBox(frame);
  _toolCombo->insertItem(i18n("<Toolchain>"));
  for (uint i=0; i<Tool::lister().nbGroups(); i++)
    _toolCombo->insertItem(Tool::lister().group(i).label());
  const Tool::Group *tgroup = config.toolGroup();
  if (tgroup) _toolCombo->setCurrentItem(tgroup->index()+1);
  connect(_toolCombo, SIGNAL(activated(int)), SLOT(updateList()));
  hbox->addWidget(_toolCombo);

  // memory filter
  _memoryCombo = new QComboBox(frame);
  _memoryCombo->insertItem(i18n("<Memory Type>"));
  for (uint i=0; i<Device::Nb_MemoryTechnologies; i++)
    _memoryCombo->insertItem(i18n(Device::MEMORY_TECHNOLOGY_DATA[i].label));
  Device::MemoryTechnology tech = config.memoryTechnology();
  if ( tech!=Device::Nb_MemoryTechnologies) _memoryCombo->setCurrentItem(tech+1);
  connect(_memoryCombo, SIGNAL(activated(int)), SLOT(updateList()));
  hbox->addWidget(_memoryCombo);

  // reset filters button
  hbox->addStretch(1);
  QPushButton *button = new KPushButton(i18n("Reset Filters"), frame);
  connect(button, SIGNAL(clicked()), SLOT(resetFilters()));
  hbox->addWidget(button);

  // list view
  QValueList<int> widths;
  widths += 80;
  widths += 500;
  Splitter *splitter = new Splitter(widths, Horizontal, mainWidget(), "device_shooser_splitter");
  top->addWidget(splitter);
  _listView = new KListView(splitter);
  connect(_listView, SIGNAL(currentChanged(QListViewItem *)),
          SLOT(currentChanged(QListViewItem *)));
  connect(_listView, SIGNAL(doubleClicked(QListViewItem *, const QPoint &, int)),
          SLOT(listDoubleClicked(QListViewItem *)));
  _listView->setAllColumnsShowFocus(true);
  _listView->setRootIsDecorated(true);
  _listView->setSorting(-1);
  _listView->addColumn(i18n("Device"));
  _listView->setResizeMode(QListView::LastColumn);

  // device view
  _deviceView = new View(splitter);
  connect(_deviceView, SIGNAL(deviceChanged(const QString &)),
          SLOT(deviceChange(const QString &)));

  updateList(device);
}

DeviceChooser::Dialog::~Dialog()
{
  Config config;
  config.writeListType(ListType(_listTypeCombo->currentItem()));
  config.writeProgrammerGroup(programmerGroup());
  config.writeToolGroup(toolGroup());
  config.writeMemoryTechnology(memoryTechnology());
}

QString DeviceChooser::Dialog::device() const
{
  QListViewItem *item = _listView->selectedItem();
  if ( item==0 || !static_cast<ListItem *>(item)->isDevice() ) return Device::AUTO_DATA.name;
  return item->text(0);
}

void DeviceChooser::Dialog::listDoubleClicked(QListViewItem *item)
{
  if ( item==0 ) return;
  if ( !static_cast<ListItem *>(item)->isDevice() ) item->setOpen(!item->isOpen());
  else accept();
}

void DeviceChooser::Dialog::currentChanged(QListViewItem *item)
{
  if ( item==0 || !static_cast<ListItem *>(item)->isDevice() ) _deviceView->clear();
  else _deviceView->setDevice(item->text(0));
}

void DeviceChooser::Dialog::deviceChange(const QString &name)
{
  QListViewItemIterator it(_listView);
  for (; it.current(); ++it)
    if ( it.current()->text(0)==name ) {
      _listView->setSelected(it.current(), true);
      _listView->ensureItemVisible(it.current());
      break;
    }
}

void DeviceChooser::Dialog::resetFilters()
{
  _programmerCombo->setCurrentItem(0);
  _toolCombo->setCurrentItem(0);
  _memoryCombo->setCurrentItem(0);
  updateList();
}

void DeviceChooser::Dialog::updateList()
{
  QListViewItem *item = _listView->selectedItem();
  QString device = (item ? item->text(0) : QString::null);
  _listView->clear();
  updateList(device);
}

const Programmer::Group *DeviceChooser::Dialog::programmerGroup() const
{
  for (uint i=0; i<Programmer::lister().nbGroups(); i++)
    if ( _programmerCombo->currentItem()==int(i+1) ) return &Programmer::lister().group(i);
  return 0;
}

const Tool::Group *DeviceChooser::Dialog::toolGroup() const
{
  for (uint i=0; i<Tool::lister().nbGroups(); i++)
    if ( _toolCombo->currentItem()==int(i+1) ) return &Tool::lister().group(i);
  return 0;
}

Device::MemoryTechnology DeviceChooser::Dialog::memoryTechnology() const
{
  if ( _memoryCombo->currentItem()==0 ) return Device::Nb_MemoryTechnologies;
  return Device::MemoryTechnology(_memoryCombo->currentItem()-1);
}

void DeviceChooser::Dialog::updateList(const QString &device)
{
  ListType type = ListType(_listTypeCombo->currentItem());
  QValueVector<QString> list = Device::lister().supportedDevices();
  QMap<QString, KListViewItem *> groups;
  QListViewItem *selected = 0;
  const Programmer::Group *pgroup = programmerGroup();
  if ( pgroup && pgroup->supportedDevices().isEmpty() && pgroup->isSoftware() ) {
    _deviceView->setText(i18n("Could not detect supported devices for \"%1\". Please check installation.").arg(pgroup->label()));
    return;
  }
  const Tool::Group *tgroup = toolGroup();
  if ( tgroup && tgroup->supportedDevices().isEmpty() ) {
    _deviceView->setText(i18n("Could not detect supported devices for toolchain \"%1\". Please check installation.").arg(tgroup->label()));
    return;
  }
  for (int i=list.count()-1; i>=0; i--) {
    if ( pgroup && !pgroup->isSupported(list[i]) ) continue;
    if ( tgroup && !tgroup->isSupported(list[i]) ) continue;
    const Device::Data *data = Device::lister().data(list[i]);
    Q_ASSERT(data);
    if ( memoryTechnology()!=Device::Nb_MemoryTechnologies
         && data->memoryTechnology()!=memoryTechnology() ) continue;
    KListViewItem *item = 0;
    switch (type) {
      case FamilyTree: {
        QString gname = Device::groupui(*data).listviewGroup(list[i]);
        if ( !groups.contains(gname) )
          groups[gname] = new ListItem(_listView, gname, false, false);
        item = new ListItem(groups[gname], list[i]);
        break;
      }
      case Flat:
        item = new ListItem(_listView, list[i], true, true);
        break;
      case Nb_ListTypes: Q_ASSERT(false); break;
    }
    if ( device==list[i] ) selected = item;
  }
  if (_withAuto) (void)new ListItem(_listView, i18n(Device::AUTO_DATA.label), true, false);
  if ( selected==0 ) selected = _listView->firstChild();
  if (selected) {
    _listView->setSelected(selected, true);
    _listView->ensureItemVisible(selected);
    currentChanged(selected);
  }
}

//-----------------------------------------------------------------------------
DeviceChooser::ComboBox::ComboBox(bool withAuto, QWidget *parent)
  : QComboBox(parent, "device_chooser_combo"), _withAuto(withAuto)
{
  if (withAuto) insertItem(i18n(Device::AUTO_DATA.label));
  for (uint i=0; i<Device::lister().nbGroups(); i++) {
    QValueVector<QString> devices = Device::lister().group(i).supportedDevices();
    qHeapSort(devices);
    for (uint k=0; k<devices.count(); k++) insertItem(devices[k]);
  }
}

void DeviceChooser::ComboBox::setDevice(const QString &device, const Device::Data *data)
{
  QString text = device;
  if ( device.isEmpty() || device==Device::AUTO_DATA.name ) {
    if (_withAuto) text = QString::null;
    else text = Device::lister().supportedDevices()[0];
  }
  if ( text.isEmpty() ) {
    if (data) changeItem(data->name() + " " + i18n(Device::AUTO_DATA.label), 0);
    else changeItem(i18n(Device::AUTO_DATA.label), 0);
    setCurrentItem(0);
  } else setCurrentText(text);
}

QString DeviceChooser::ComboBox::device() const
{
  if ( _withAuto && currentItem()==0 ) return Device::AUTO_DATA.name;
  return currentText();
}

//-----------------------------------------------------------------------------
DeviceChooser::Button::Button(bool withAuto, QWidget *parent)
  : QWidget(parent, "device_chooser_button")
{
  QHBoxLayout *hbox = new QHBoxLayout(this, 0, 10);
  _combo = new ComboBox(withAuto, this);
  connect(_combo, SIGNAL(activated(int)), SIGNAL(changed()));
  hbox->addWidget(_combo);
  KIconLoader loader;
  QIconSet iconset = loader.loadIcon("fileopen", KIcon::Toolbar);
  KPushButton *button = new KPushButton(iconset, QString::null, this);
  connect(button, SIGNAL(clicked()), SLOT(chooseDevice()));
  hbox->addWidget(button);
}

void DeviceChooser::Button::chooseDevice()
{
  Dialog dialog(_combo->device(), (_combo->withAuto() ? ChooseWithAuto : Choose), this);
  if ( !dialog.exec() || dialog.device().isEmpty() ) return;
  _combo->setDevice(dialog.device());
  emit changed();
}

//-----------------------------------------------------------------------------
DeviceChooser::Browser::Browser(QWidget *parent)
 : KTextBrowser(parent, "device_browser")
{}

void DeviceChooser::Browser::setSource(const QString &name)
{
  if ( name.startsWith("device://") ) {
    emit deviceChanged(name.mid(9));
    return;
  }
  KTextBrowser::setSource(name);
}

//-----------------------------------------------------------------------------
DeviceChooser::View::View(QWidget *parent)
  : QWidget(parent, "device_view")
{
  QVBoxLayout *top = new QVBoxLayout(this);
  _browser = new Browser(this);
  connect(_browser, SIGNAL(deviceChanged(const QString &)),
          SIGNAL(deviceChanged(const QString &)));
  _msf = new QMimeSourceFactory;
  _browser->setMimeSourceFactory(_msf);
  top->addWidget(_browser);
}

DeviceChooser::View::~View()
{
  delete _msf;
}

void DeviceChooser::View::setDevice(const QString &name)
{
  const Device::Data *data = Device::lister().data(name);
  if ( data==0 ) return;
  QString html = "<html><body>";
  html += htmlInfo(*data, "device:%1", QString::null, _msf);
  html += "</body></html>";
  setText(html);
}

void DeviceChooser::View::setText(const QString &text)
{
  _browser->setText(text);
}

void DeviceChooser::View::clear()
{
  _browser->clear();
}

//-----------------------------------------------------------------------------
DeviceChooser::Editor::Editor(const QString &title, const QString &tag, QWidget *widget)
  : DeviceEditor(title, tag, widget, "device_view_editor")
{}

QWidget *DeviceChooser::Editor::createView(const Device::Data *, QWidget *parent)
{
  DeviceChooser::View *view = new DeviceChooser::View(parent);
  connect(view, SIGNAL(deviceChanged(const QString &)), SIGNAL(deviceChanged(const QString &)));
  view->setDevice(_device);
  return view;
}
