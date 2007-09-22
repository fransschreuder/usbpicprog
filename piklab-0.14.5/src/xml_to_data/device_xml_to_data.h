/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DEVICE_XML_TO_DATA_H
#define DEVICE_XML_TO_DATA_H

#include <qmap.h>
#include <qfile.h>
#include <qtextstream.h>

#include "common/common/misc.h"
#include "common/common/streamer.h"
#include "devices/base/generic_device.h"
#include "xml_to_data.h"

namespace Device
{
class XmlToDataBase : public ::XmlToData
{
public:
  XmlToDataBase() : _data(0) {}

protected:
  mutable Data *_data;
  QMap<QString, Data *> _map;          // device -> data

  virtual void parse();
  virtual QString currentDevice() const { return (_data ? _data->name() : QString::null); }
  virtual QString namespaceName() const = 0;
  virtual Data *createData() const = 0;
  virtual void processDevice(QDomElement device);
  virtual void checkPins(const QMap<QString, uint> &pinLabels) const = 0;

private:
  QMap<QString, QString>     _documents;    // document -> device
  QMap<QString, QStringList> _alternatives; // device -> alternatives

  bool getFrequencyRange(FrequencyRange::Type type, FrequencyRange::Special special, QDomElement element);
  bool getMemoryTechnology(QDomElement element);
  Device::Package processPackage(QDomElement element);
};

template <class DataType>
class XmlToData : public XmlToDataBase, public DataStreamer<DataType>
{
public:
  virtual Device::Data *createData() const { return new DataType; }
  DataType *data() { return static_cast<DataType *>(_data); }
  const DataType *data() const { return static_cast<DataType *>(_data); }
  virtual void output() {
    QFile dfile("deps.mak");
    if ( !dfile.open(IO_WriteOnly) ) return;
    QTextStream dts(&dfile);
    dts << "noinst_DATA = ";
    uint i = 0;
    QMap<QString, Data *>::const_iterator it;
    for (it=_map.begin(); it!=_map.end(); ++it) {
      if ( (i%10)==0 ) dts << "\\" << endl << "    ";
      dts << " " << it.key() << ".xml";
      i++;
    }
    dts << endl;

    QFile file(namespaceName().lower() + "_data.cpp");
    if ( !file.open(IO_WriteOnly) ) return;
    QTextStream ts(&file);
    ts << "#include \"devices/" << namespaceName().lower() << "/"
       << namespaceName().lower() << "/" << namespaceName().lower() << "_group.h\"" << endl << endl;
    ts << "const char *" << namespaceName() << "::DATA_STREAM =" << endl;
    QValueList<DataType *> list;
    for (it=_map.begin(); it!=_map.end(); ++it)
      list.append(static_cast<const DataType *>(it.data()));
    uint size = toCppString(list, ts);
    ts << ";" << endl;
    ts << "const uint " << namespaceName() << "::DATA_SIZE = " << size << ";" << endl;
  }
};

} // namespace

#endif
