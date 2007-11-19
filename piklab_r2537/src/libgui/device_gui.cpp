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

namespace DeviceChooser
{
//-----------------------------------------------------------------------------
void DeviceChooser::Config::writeProgrammerGroup(const Programmer::Group *group)
{
  writeEntry("programmer", group ? group->name() : QString::null);
}
const Programmer::Group *DeviceChooser::Config::programmerGroup()
{
  QString name = readEntry("programmer", QString::null);
  return Programmer::lister().group(name);
}

void DeviceChooser::Config::writeToolGroup(const Tool::Group *group)
{
  writeEntry("tool", group ? group->name() : QString::null);
}
const Tool::Group *DeviceChooser::Config::toolGroup()
{
  QString name = readEntry("tool", QString::null);
  return Tool::lister().group(name);
}

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
const DeviceChooser::ListType::Data DeviceChooser::ListType::DATA[Nb_Types] = {
  { "family_tree", I18N_NOOP("Family Tree") },
  { "flat",        I18N_NOOP("Flat List")   }
};

DeviceChooser::Dialog::Dialog(const QString &device, Type type, QWidget *parent)
  : ::Dialog(parent, "device_chooser_dialog", true, i18n("Select a device"),
             Ok|Close, Close, false, QSize(400, 300)), _withAuto(type==ChooseWithAuto)
{
  setButtonOK(KGuiItem(i18n( "&Select"), "button_ok"));
  QVBoxLayout *top = new QVBoxLayout(mainWidget(), 0, 10);

  // view
  QHBoxLayout *hbox = new QHBoxLayout(top, 10);
  QVBoxLayout *vbox = new QVBoxLayout(hbox);
  _listTypeCombo = new EnumComboBox<ListType>("list_type", mainWidget());
  connect(_listTypeCombo->combo(), SIGNAL(activated(int)), SLOT(updateList()));
  vbox->addWidget(_listTypeCombo->combo());
  QPushButton *button = new KPushButton(KGuiItem(i18n("Reset Filters"), "reload"), mainWidget());
  connect(button, SIGNAL(clicked()), SLOT(resetFilters()));
  vbox->addWidget(button);
  vbox->addStretch(1);

  // filters
  QFrame *frame = new QFrame(mainWidget());
  frame->setFrameStyle(QFrame::Panel | QFrame::Raised);
  frame->setMargin(5);
  hbox->addWidget(frame);
  hbox = new QHBoxLayout(frame, 10, 10);
  QLabel *label = new QLabel(i18n("Filters:"), frame);
  hbox->addWidget(label);
  vbox = new QVBoxLayout(hbox);

  QHBoxLayout *shbox = new QHBoxLayout(vbox);

  // programmer filter
  _programmerCombo = new KeyComboBox(frame);
  _programmerCombo->appendItem("<all>", i18n("<Programmer>"));
  Programmer::Lister::ConstIterator pit;
  for (pit=Programmer::lister().begin(); pit!=Programmer::lister().end(); ++pit)
    _programmerCombo->appendItem(pit.key(), pit.data()->label());
  Config config;
  const Programmer::Group *pgroup = config.programmerGroup();
  if (pgroup) _programmerCombo->setCurrentItem(pgroup->name());
  connect(_programmerCombo, SIGNAL(activated()), SLOT(updateList()));
  shbox->addWidget(_programmerCombo);

  // tool filter
  _toolCombo = new KeyComboBox(frame);
  _toolCombo->appendItem("<all>", i18n("<Toolchain>"));
  Tool::Lister::ConstIterator tit;
  for (tit=Tool::lister().begin(); tit!=Tool::lister().end(); ++tit) {
    if ( tit.data()->isCustom() ) continue;
    _toolCombo->appendItem(tit.key(), tit.data()->label());
  }
  const Tool::Group *tgroup = config.toolGroup();
  if (tgroup) _toolCombo->setCurrentItem(tgroup->name());
  connect(_toolCombo, SIGNAL(activated()), SLOT(updateList()));
  shbox->addWidget(_toolCombo);

  // memory filter
  _memoryCombo = new EnumComboBox<Device::MemoryTechnology>(i18n("<Memory Type>"), "memory_technology", frame);
  connect(_memoryCombo->combo(), SIGNAL(activated(int)), SLOT(updateList()));
  shbox->addWidget(_memoryCombo->combo());

  shbox->addStretch(1);
  shbox = new QHBoxLayout(vbox);

  // status filter
  _statusCombo = new EnumComboBox<Device::Status>(i18n("<Status>"), "status", frame);
  connect(_statusCombo->combo(), SIGNAL(activated(int)), SLOT(updateList()));
  shbox->addWidget(_statusCombo->combo());

  // features filter
  _featureCombo = new EnumComboBox<Pic::Feature>(i18n("<Feature>"), "feature", frame);
  connect(_featureCombo->combo(), SIGNAL(activated(int)), SLOT(updateList()));
  shbox->addWidget(_featureCombo->combo());

  shbox->addStretch(1);

  // list view
  QValueList<int> widths;
  widths += 80;
  widths += 500;
  Splitter *splitter = new Splitter(widths, Horizontal, mainWidget(), "device_shooser_splitter");
  top->addWidget(splitter, 1);
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
  config.writeProgrammerGroup(programmerGroup());
  config.writeToolGroup(toolGroup());
  _listTypeCombo->writeConfig();
  _memoryCombo->writeConfig();
  _statusCombo->writeConfig();
  _featureCombo->writeConfig();
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
  else _deviceView->setDevice(item->text(0), false);
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
  _programmerCombo->setCurrentItem("<all>");
  _toolCombo->setCurrentItem("<all>");
  _memoryCombo->reset();
  _statusCombo->reset();
  _featureCombo->reset();
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
  return Programmer::lister().group(_programmerCombo->currentItem());
}

const Tool::Group *DeviceChooser::Dialog::toolGroup() const
{
  return Tool::lister().group(_toolCombo->currentItem());
}

void DeviceChooser::Dialog::updateList(const QString &device)
{
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
    if ( _memoryCombo->value()!=Device::MemoryTechnology::Nb_Types && data->memoryTechnology()!=_memoryCombo->value() ) continue;
    if ( _statusCombo->value()!=Device::Status::Nb_Types && data->status()!=_statusCombo->value() ) continue;
    if ( _featureCombo->value()!=Pic::Feature::Nb_Types ) {
      if ( data->group().name()!="pic" ) continue;
      if ( !static_cast<const Pic::Data *>(data)->hasFeature(_featureCombo->value()) ) continue;
    }
    KListViewItem *item = 0;
    switch (_listTypeCombo->value().type()) {
      case ListType::FamilyTree: {
        QString gname = data->listViewGroup();
        if ( !groups.contains(gname) )
          groups[gname] = new ListItem(_listView, gname, false, false);
        item = new ListItem(groups[gname], list[i]);
        break;
      }
      case ListType::Flat:
        item = new ListItem(_listView, list[i], true, true);
        break;
      case ListType::Nb_Types: Q_ASSERT(false); break;
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
  Device::Lister::ConstIterator it;
  for (it=Device::lister().begin(); it!=Device::lister().end(); ++it) {
    QValueVector<QString> devices = it.data()->supportedDevices();
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

PURL::Url findDocumentUrl(const QString &prefix, const QString &baseName)
{
  PURL::Url previous = KURL::fromPathOrURL(prefix + baseName + ".pdf");
  bool previousExists = previous.exists();
  for (uint i=0; i<26; i++) {
    PURL::Url url = KURL::fromPathOrURL(prefix + baseName + QChar('a' + i) + ".pdf");
    bool exists = url.exists();
    if ( !exists && previousExists ) return previous;
    previous = url;
    previousExists = exists;
  }
  return previous;
}

void DeviceChooser::Browser::setSource(const QString &name)
{
  ::BusyCursor bc;
  if ( name.startsWith("device://") ) emit deviceChanged(name.mid(9));
  else if ( name.startsWith("document://") ) {
    QString prefix = "http://ww1.microchip.com/downloads/en/DeviceDoc/";
    PURL::Url url = findDocumentUrl(prefix, name.mid(11, name.length()-11-1));
    KTextBrowser::setSource(url.kurl().htmlURL());
  }
  else KTextBrowser::setSource(name);
}

//-----------------------------------------------------------------------------
DeviceChooser::View::View(QWidget *parent)
  : TabWidget(parent, "device_view")
{
  // Information
  _info = new Browser(this);
  _info->setMimeSourceFactory(&_msf);
  insertTab(_info, i18n("Information"));
  connect(_info, SIGNAL(deviceChanged(const QString &)),
          SIGNAL(deviceChanged(const QString &)));

  // Voltage-Frequency Graphs
  _vfg = new Browser(this);
  _vfg->setMimeSourceFactory(&_msf);
  insertTab(_vfg, i18n("Voltage-Frequency Graphs"));

  // Pin Diagrams
  _pins = new Browser(this);
  _pins->setMimeSourceFactory(&_msf);
  insertTab(_pins, i18n("Pin Diagrams"));
}

void DeviceChooser::View::setDevice(const QString &name, bool cannotChangeDevice)
{
  const Device::Data *data = Device::lister().data(name);
  if ( data==0 ) return;
  QString doc = htmlInfo(*data, (cannotChangeDevice ? QString::null : "device:%1"), Device::documentHtml(*data));
  doc += Device::supportedHtmlInfo(*data);
  _info->setText("<html><body>" + doc + "</body></html>");
  doc = htmlVoltageFrequencyGraphs(*data, QString::null, &_msf);
  _vfg->setText("<html><body>" + doc + "</body></html>");
  doc = htmlPinDiagrams(*data, QString::null, &_msf);
  _pins->setText("<html><body>" + doc + "</body></html>");
}

void DeviceChooser::View::setText(const QString &text)
{
  _info->setText(text);
  _vfg->setText(text);
  _pins->setText(text);
}

void DeviceChooser::View::clear()
{
  _info->clear();
  _vfg->clear();
  _pins->clear();
}

//-----------------------------------------------------------------------------
DeviceChooser::Editor::Editor(const QString &title, const QString &tag, QWidget *widget)
  : DeviceEditor(title, tag, widget, "device_view_editor")
{}

QWidget *DeviceChooser::Editor::createView(const Device::Data *, QWidget *parent)
{
  DeviceChooser::View *view = new DeviceChooser::View(parent);
  connect(view, SIGNAL(deviceChanged(const QString &)), SIGNAL(deviceChanged(const QString &)));
  view->setDevice(_device, true);
  return view;
}
