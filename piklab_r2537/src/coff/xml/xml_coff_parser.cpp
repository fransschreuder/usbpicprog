/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "xml_to_data/prog_xml_to_data.h"
#include "coff/base/coff_data.h"

#include "devices/base/device_group.h"
#include "devices/pic/base/pic.h"
#include "devices/list/device_list.h"
#include "gpprocessor.h"

//-----------------------------------------------------------------------------
namespace Coff
{

class XmlToData : public ExtXmlToData<Data>
{
public:
  XmlToData() : ExtXmlToData<Data>("coff", "Coff") {}

private:
  QMap<uint, bool> _ids;
  virtual bool hasFamilies() const { return false; }
  virtual void parseData(QDomElement element, Data &data);
  virtual void outputData(const Data &data, QTextStream &s) const;
  virtual void outputFunctions(QTextStream &s) const;
  virtual void parse();
};

void Coff::XmlToData::parseData(QDomElement element, Data &data)
{
  QStringList list = QStringList::split(' ', element.attribute("id"));
  if ( list.isEmpty() ) qFatal("Missing id");
  if ( list.count()>MAX_NB_IDS ) qFatal("Please raise MAX_NB_IDS");
  for (uint i=0; i<MAX_NB_IDS; i++) {
    if ( i<uint(list.count()) ) {
      bool ok;
      data.ids[i] = fromHexLabel(list[i], 4, &ok);
      if ( !ok ) qFatal("Invalid id");
      //if ( _ids.contains(data.ids[i]) ) qFatal("Duplicated id");
      //_ids[data.ids[i]] = true;
    } else data.ids[i] = 0;
  }
}

void Coff::XmlToData::outputData(const Data &data, QTextStream &s) const
{
  s << "{ ";
  for (uint i=0; i<MAX_NB_IDS; i++) {
    if ( i!=0 ) s << ", ";
    s << toHexLabel(data.ids[i], 4);
  }
  s << "}";
}

void Coff::XmlToData::outputFunctions(QTextStream &s) const
{
  ExtXmlToData<Data>::outputFunctions(s);
  s << "QString findId(uint id)" << endl;
  s << "{" << endl;
  s << "  for (uint i=0; DATA_LIST[i]; i++) {" << endl;
  s << "    for (uint k=0; k<MAX_NB_IDS; k++)" << endl;
  s << "      if ( DATA_LIST[i]->data.ids[k]==id ) return DATA_LIST[i]->name;" << endl;
  s << "  }" << endl;
  s << "  return QString::null;" << endl;
  s << "}" << endl;
}

void Coff::XmlToData::parse()
{
  ExtXmlToData<Data>::parse();

  // check what devices we are missing
  const ::Group::Base *gpic = Device::lister().group("pic");
  ::Group::Base::ConstIterator it;
  for (it=gpic->begin(); it!=gpic->end(); ++it) {
    const Pic::Data *data = static_cast<const Pic::Data*>(it.data().data);
    switch (data->architecture().type()) {
      case Pic::Architecture::P10X:
      case Pic::Architecture::P16X:
      case Pic::Architecture::P17C:
      case Pic::Architecture::P18C:
      case Pic::Architecture::P18F:
      case Pic::Architecture::P18J: break;
      case Pic::Architecture::P24F:
      case Pic::Architecture::P24H:
      case Pic::Architecture::P30F:
      case Pic::Architecture::P33F: continue;
      case Pic::Architecture::Nb_Types: Q_ASSERT(false); continue;
    }
    if ( !hasDevice(data->name()) ) qWarning("No id for device %s", data->name().latin1());
  }

  // extract COFF id from gputils
  for (uint i=0; pics[i].tag!=no_processor; i++) {
    _current = QString(pics[i].names[2]).upper();
    if ( !Device::lister().isSupported(_current) ) continue;
    if ( !hasDevice(_current) ) qDebug(">> add new id %s: %s", _current.latin1(), toHexLabel(pics[i].coff_type, 4).latin1());
    else {
      bool ok = false;
      for (uint k=0; k<MAX_NB_IDS; k++)
        if ( _map[_current].data.ids[k]==pics[i].coff_type ) ok = true;
      if ( !ok ) qFatal(QString("Different ids"));
    }
  }
}

} // namespace

//-----------------------------------------------------------------------------
XML_MAIN(Coff::XmlToData)
