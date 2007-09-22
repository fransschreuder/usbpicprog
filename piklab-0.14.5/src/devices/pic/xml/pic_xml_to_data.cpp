/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include <qfile.h>
#include <qregexp.h>

#include "xml_to_data/device_xml_to_data.h"
#include "common/common/misc.h"
#include "devices/pic/base/pic_config.h"
#include "devices/pic/base/pic_register.h"

namespace Pic
{
class XmlToData : public Device::XmlToData<Data>
{
private:
  virtual QString namespaceName() const { return "Pic"; }

bool getVoltages(ProgVoltageType type, QDomElement element)
{
  QDomElement voltages = findUniqueElement(element, "voltages", "name", PROG_VOLTAGE_TYPE_DATA[type].name);
  if ( voltages.isNull() ) return false;
  bool ok1, ok2, ok3;
  data()->_voltages[type].min = voltages.attribute("min").toDouble(&ok1);
  data()->_voltages[type].max = voltages.attribute("max").toDouble(&ok2);
  data()->_voltages[type].nominal = voltages.attribute("nominal").toDouble(&ok3);
  if ( !ok1 || !ok2 || !ok3 ) qFatal(QString("Cannot extract voltage value for \"%1\"").arg(PROG_VOLTAGE_TYPE_DATA[type].name));
  if ( data()->_voltages[type].min>data()->_voltages[type].max
       || data()->_voltages[type].nominal<data()->_voltages[type].min
       || data()->_voltages[type].nominal>data()->_voltages[type].max )
    qFatal("Inconsistent voltages order");
  return true;
}

bool getMemoryRange(MemoryRangeType type, QDomElement element)
{
  QDomElement range = findUniqueElement(element, "memory", "name", MEMORY_RANGE_TYPE_DATA[type].name);
  if ( range.isNull() ) return false;
  data()->_ranges[type].properties = Present;
  bool ok;
  uint nbCharsAddress = data()->nbCharsAddress();
  data()->_ranges[type].start = fromHexLabel(range.attribute("start"), nbCharsAddress, &ok);
  if ( !ok ) qFatal("Cannot extract start address");
  data()->_ranges[type].end = fromHexLabel(range.attribute("end"), nbCharsAddress, &ok);
  if ( !ok ) qFatal("Cannot extract end address");
  if ( data()->_ranges[type].end<data()->_ranges[type].start ) qFatal("Memory range end is before its start");
  uint nbCharsWord = data()->nbCharsWord(type);
  if ( data()->nbBitsWord(type)==0 ) qFatal(QString("Architecture doesn't contain memory range %1").arg(MEMORY_RANGE_TYPE_DATA[type].name));
  if ( type==MemoryUserId ) {
    data()->_ranges[type].rmask = fromHexLabel(range.attribute("rmask"), nbCharsWord, &ok);
    if ( !ok ) qFatal("Cannot extract rmask value for user id");
    if ( data()->_ranges[type].rmask>data()->mask(type) ) qFatal(QString("Read mask bigger than mask %1 (%2)").arg(toHexLabel(data()->_ranges[type].rmask, 8)).arg(toHexLabel(data()->mask(type), 8)));
  } else data()->_ranges[type].rmask = data()->mask(type);
  if ( range.attribute("hexfile_offset")!="?" ) {
    data()->_ranges[type].properties |= Programmable;
    if ( !range.attribute("hexfile_offset").isEmpty() ) {
      data()->_ranges[type].hexFileOffset = fromHexLabel(range.attribute("hexfile_offset"), nbCharsAddress, &ok);
      if ( !ok ) qFatal("Cannot extract hexfile_offset");
    }
  }
  if ( type==MemoryCal && !data()->is18Family() ) {
    data()->_calibration.opcodeMask = fromHexLabel(range.attribute("cal_opmask"), nbCharsWord, &ok);
    if ( !ok ) qFatal("Cannot extract calibration opcode mask");
    data()->_calibration.opcode = fromHexLabel(range.attribute("cal_opcode"), nbCharsWord, &ok);
    if ( !ok ) qFatal("Cannot extract calibration opcode");
    if ( (data()->_calibration.opcodeMask & data()->_calibration.opcode)!=data()->_calibration.opcode )
      qFatal("Calibration opcode mask should overlap opcode mask");
    if ( (data()->_calibration.opcodeMask & data()->mask(type))!=data()->_calibration.opcodeMask )
      qFatal("Calibration mask should overlap opcode mask");
  }
  QString wwa = range.attribute("word_write_align");
  QString wea = range.attribute("word_erase_align");
  if ( type==MemoryCode ) {
    if ( data()->_architecture==Arch_18F || data()->_architecture==Arch_18J ) {
      data()->_nbWordsCodeWrite = wwa.toUInt(&ok);
      if ( !ok || data()->_nbWordsCodeWrite==0 || (data()->_nbWordsCodeWrite%4)!=0 ) qFatal("Missing or malformed word write align");
      data()->_nbWordsCodeRowErase = wea.toUInt(&ok);
      if ( !ok || (data()->_nbWordsCodeRowErase%4)!=0 ) qFatal("Missing or malformed word erase align");
    } else {
      if ( !wwa.isEmpty() || !wea.isEmpty() ) qFatal("word align should not be defined for this device family/subfamily");
      data()->_nbWordsCodeWrite = 0; // #### TODO
      data()->_nbWordsCodeRowErase = 0; // #### TODO
    }
  } else if ( !wwa.isEmpty() || !wea.isEmpty() ) qFatal("word align should not be defined for this memory range");
  return true;
}

bool hasValue(const Pic::Config::Mask &mask, uint value)
{
  for (uint i=0; i<uint(mask.values.count()); i++)
    if ( mask.values[i].value==value ) return true;
  return false;
}

void processName(const Pic::Config::Mask &cmask, uint pmask, const QString &ecnames, Pic::Config::Value &cvalue)
{
  if ( cvalue.name=="invalid" ) {
    cvalue.name = QString::null;
    if ( !cvalue.cname.isEmpty() ) qFatal(QString("No cname should be defined for invalid value in mask %1").arg(cmask.name));
    return;
  }
  if ( cvalue.name.isEmpty() ) qFatal(QString("Empty value name in mask %1").arg(cmask.name));
  if ( (cmask.value & pmask)==cmask.value ) { // protected bits
    if ( !cvalue.cname.isEmpty() ) qFatal(QString("Config name should be null for protected config mask \"%1\"").arg(cmask.name));
  } else {
    if ( cvalue.cname.isEmpty() && cmask.name!="BSSEC" && cmask.name!="BSSIZ" && cmask.name!="SSSEC" && cmask.name!="SSSIZ" ) {
      // #### FIXME: 18F 18J 24H
      if ( data()->architecture()!=Pic::Arch_18F && data()->architecture()!=Pic::Arch_18J && data()->architecture()!=Pic::Arch_24H )
        qFatal(QString("cname not defined for \"%1\" (%2)").arg(cvalue.name).arg(cmask.name));
    }
    QStringList list = QStringList::split(' ', cvalue.cname);
    for (uint i=0; i<uint(list.count()); i++) {
      if ( list[i]=="_" && list.count()==1 ) continue;
      else if ( list[i].startsWith("0x") ) {
        bool ok;
        uint v = fromHexLabel(list[i], &ok);
        if ( list.count()==1 && ok && cmask.value!=cvalue.value && (v & cmask.value)==cvalue.value ) continue;
      } else if ( XOR(list[i].startsWith("_"), data()->architecture()==Pic::Arch_30X) ) continue;
      qFatal(QString("Invalid config name for \"%1\"/\"%2\"").arg(cmask.name).arg(cvalue.name));
    }
    cvalue.cname = list.join(" & ");
    cvalue.extraCNames = QStringList::split(' ', ecnames);
    for (uint i=0; i<uint(cvalue.extraCNames.count()); i++)
      if ( cvalue.extraCNames[i][0]!='_'  ) qFatal(QString("Invalid extra config name for %1").arg(cvalue.name));
  }
}

Pic::Config::Mask toConfigMask(QDomElement mask, uint pmask)
{
  uint nbChars = data()->nbCharsWord(MemoryConfig);
  bool ok;
  QString defName, defCName, defECNames;
  Config::Mask cmask;
  cmask.name = mask.attribute("name");
  if ( !Config::hasMaskName(cmask.name) ) qFatal(QString("Unknown mask name %1").arg(cmask.name));
  cmask.value = fromHexLabel(mask.attribute("value"), nbChars, &ok);
  if ( !ok || cmask.value==0 || cmask.value>data()->mask(MemoryConfig) )
    qFatal(QString("Malformed mask value in mask %1").arg(mask.attribute("name")));
  //QStringList names;
  QDomNode child = mask.firstChild();
  while ( !child.isNull() ) {
    QDomElement value = child.toElement();
    child = child.nextSibling();
    if ( value.isNull() ) continue;
    if ( value.nodeName()!="value" ) qFatal(QString("Non value child in mask %1").arg(cmask.name));
    if ( value.attribute("value")=="default" ) {
      if ( !defName.isEmpty() ) qFatal(QString("Default value already defined for mask %1").arg(cmask.name));
      defName = value.attribute("name");
      //if ( names.contains(defName) ) qFatal(QString("Value name duplicated in mask %1").arg(cmask.name));
      //names.append(defName);
      defCName = value.attribute("cname");
      defECNames = value.attribute("ecnames");
      continue;
    }
    Config::Value cvalue;
    cvalue.value = fromHexLabel(value.attribute("value"), nbChars, &ok);
    if ( !ok || (cmask.value & cvalue.value)!=cvalue.value ) qFatal(QString("Malformed value in mask %1").arg(cmask.name));
    cvalue.name = value.attribute("name");
    //if ( names.contains(cvalue.name) ) qFatal(QString("Value name duplicated in mask %1").arg(cmask.name));
    //names.append(cvalue.name);
    cvalue.cname = value.attribute("cname");
    processName(cmask, pmask, value.attribute("ecnames"), cvalue);
    cmask.values.append(cvalue);
  }
  // add default values
  if ( !defName.isEmpty() ) {
    uint nb = 0;
    for (uint k=0; k<=cmask.value; k++) {
      if ( (k & cmask.value)!=k ) continue; // outside mask
      if ( hasValue(cmask, k) ) continue; // already set
      nb++;
      Config::Value cvalue;
      cvalue.value = k;
      cvalue.name = defName;
      cvalue.cname = defCName;
      processName(cmask, pmask, defECNames, cvalue);
      cmask.values.append(cvalue);
    }
    if ( nb<=1 ) qFatal(QString("Default value used less than twice in mask %1").arg(cmask.name));
  }
  qHeapSort(cmask.values);
  return cmask;
}

Pic::Config::Word toConfigWord(QDomElement config)
{
  uint nbChars = data()->nbCharsWord(MemoryConfig);
  Config::Word cword;
  cword.name = config.attribute("name");
  if ( cword.name.isNull() ) qFatal("Config word name not specified.");
  bool ok;
  cword.wmask = fromHexLabel(config.attribute("wmask"), nbChars, &ok);
  uint gmask = data()->mask(MemoryConfig);
  if ( !ok || cword.wmask>gmask ) qFatal(QString("Missing or malformed config wmask \"%1\"").arg(config.attribute("wmask")));
  cword.bvalue = fromHexLabel(config.attribute("bvalue"), nbChars, &ok);
  if ( !ok ) qFatal(QString("Missing or malformed config bvalue \"%1\"").arg(config.attribute("bvalue")));
  if ( (cword.bvalue & cword.wmask)!=cword.bvalue ) qFatal("bvalue should be inside wmask");
  if ( config.attribute("pmask").isEmpty() ) cword.pmask = 0;
  else {
    bool ok;
    cword.pmask = fromHexLabel(config.attribute("pmask"), nbChars, &ok);
    if ( !ok || cword.pmask>gmask ) qFatal("Missing or malformed config pmask");
  }
  cword.ignoredCNames = QStringList::split(' ', config.attribute("icnames"));
  for (uint i=0; i<uint(cword.ignoredCNames.count()); i++)
    if ( cword.ignoredCNames[i][0]!='_'  ) qFatal(QString("Invalid ignored config name for %1").arg(cword.name));
  QDomNode child = config.firstChild();
  while ( !child.isNull() ) {
    QDomElement mask = child.toElement();
    child = child.nextSibling();
    if ( mask.isNull() ) continue;
    if ( mask.nodeName()!="mask" ) qFatal(QString("Non mask child in config %1").arg(cword.name));
    if ( mask.attribute("name").isEmpty() ) qFatal(QString("Empty mask name in config %1").arg(cword.name));
    Config::Mask cmask = toConfigMask(mask, cword.pmask);
    if ( (cmask.value & gmask)!=cmask.value ) qFatal(QString("Mask value not inside mask in config %1").arg(cword.name));
    for (uint i=0; i<uint(cword.masks.count()); i++) {
      if ( cword.masks[i].name==cmask.name ) qFatal(QString("Duplicated mask name %1 in config %2").arg(cmask.name).arg(cword.name));
      if ( cword.masks[i].value & cmask.value ) qFatal(QString("Overlapping masks in config %1").arg(cword.name));
    }
    cword.masks.append(cmask);
  }
  qHeapSort(cword.masks);
  if ( config.attribute("cmask").isEmpty() ) {
    if ( data()->_architecture==Pic::Arch_30X ) cword.cmask = cword.wmask;
    else cword.cmask = cword.usedMask();
  } else {
    bool ok;
    cword.cmask = fromHexLabel(config.attribute("cmask"), nbChars, &ok);
    if ( !ok || cword.cmask>gmask ) qFatal("Missing or malformed config cmask");
  }
  uint rmask = cword.usedMask();
  if ( (cword.pmask & rmask)!=cword.pmask ) qFatal("pmask should be inside or'ed mask values.");
  return cword;
}

QValueVector<Pic::Config::Word> getConfigWords(QDomElement element)
{
  uint nbWords = data()->nbWords(MemoryConfig);
  QValueVector<Config::Word> configWords(nbWords);
  QDomNode child = element.firstChild();
  while ( !child.isNull() ) {
    QDomElement config = child.toElement();
    child = child.nextSibling();
    if ( config.isNull() || config.nodeName()!="config" ) continue;
    bool ok;
    uint offset = fromHexLabel(config.attribute("offset"), 1, &ok);
    if ( !ok ) qFatal("Missing or malformed config offset");
    if ( (offset % data()->addressIncrement(MemoryConfig))!=0 ) qFatal("Config offset not aligned");
    offset /= data()->addressIncrement(MemoryConfig);
    if ( offset>=nbWords ) qFatal(QString("Offset too big %1/%2").arg(offset).arg(nbWords));
    if ( !configWords[offset].name.isNull() ) qFatal(QString("Config offset %1 is duplicated").arg(offset));
    for (uint i=0; i<nbWords; i++) {
      if ( !configWords[i].name.isNull() && configWords[i].name==config.attribute("name") )
        qFatal(QString("Duplicated config name %1").arg(configWords[i].name));
    }
    configWords[offset] = toConfigWord(config);
  }
  return configWords;
}

virtual void processDevice(QDomElement device)
{
  Device::XmlToDataBase::processDevice(device);

  QString arch = device.attribute("architecture");
  data()->_architecture = Nb_Architectures;
  for (uint i=0; i<Nb_Architectures; i++)
    if ( arch==ARCHITECTURE_DATA[i].name ) data()->_architecture = Architecture(i);
  if ( data()->_architecture==Nb_Architectures ) qFatal(QString("Unrecognized architecture \"%1\"").arg(arch));
  if ( (data()->_architecture==Arch_18F && data()->_name.contains("C"))
       || (data()->_architecture==Arch_18F && data()->_name.contains("J")) ) qFatal("Not matching family");

  if ( data()->_document.isEmpty() ) warning("No document specified");
  else {
    QRegExp rexp("\\d{6}");
    if ( !rexp.exactMatch(data()->_document) ) qFatal(QString("Document should be 6 digits \"%1\"").arg(data()->_document));
  }
  bool ok;
  QString pc = device.attribute("pc");
  if ( data()->_architecture==Arch_10X ) {
    data()->_nbBitsPC = pc.toUInt(&ok);
    if ( !ok || data()->_nbBitsPC==0 ) qFatal("Malformed or missing PC");
  } else {
    data()->_nbBitsPC = ARCHITECTURE_DATA[data()->_architecture].nbBitsPC;
    if ( !pc.isEmpty() ) qFatal("No PC should be provided for this device architecture");
  }

  // device id
  QString id = device.attribute("id");
  if ( id.isEmpty() ) data()->_id = 0;
  else {
    data()->_id = fromHexLabel(id, 4, &ok);
    if ( !ok ) qFatal("Malformed id");
  }

  // checksum
  QString cst = device.attribute("checksum_cst");
  if ( cst.isEmpty() ) data()->_checksum.constant = 0;
  else {
    data()->_checksum.constant = fromHexLabel(cst, 4, &ok);
    if ( !ok ) qFatal("Malformed checksum constant");
  }
  cst = device.attribute("checksum_pcst");
  if ( cst.isEmpty() ) data()->_checksum.protectedConstant = 0;
  else {
    data()->_checksum.protectedConstant = fromHexLabel(cst, 4, &ok);
    if ( !ok ) qFatal("Malformed checksum constant");
  }
  data()->_checksum.protectedType = NormalChecksum; // #### FIXME

  // voltages
  QStringList names;
  for (uint i=0; i<Nb_ProgVoltageTypes; i++) {
    ProgVoltageType vtype = ProgVoltageType(i);
    names += PROG_VOLTAGE_TYPE_DATA[i].name;
    if ( !getVoltages(vtype, device) ) {
      switch (vtype) {
        case Vpp:
        case VddBulkErase: qFatal(QString("Voltage \"%1\" not defined").arg(PROG_VOLTAGE_TYPE_DATA[i].name));
        case VddWrite: data()->_voltages[VddWrite] = data()->_voltages[VddBulkErase]; break;
        case Nb_ProgVoltageTypes: Q_ASSERT(false); break;
      }
    }
  }
  if ( data()->vddMin()>data()->_voltages[VddWrite].min ) qFatal("Vdd min higher than VddWrite min");
  if ( data()->vddMax()<data()->_voltages[VddWrite].max ) qFatal("Vdd max lower than VddWrite max");
  if ( data()->_voltages[VddWrite].min>data()->_voltages[VddBulkErase].min ) qFatal("VddWrite min higher than VddBulkErase min");
  if ( data()->_voltages[VddWrite].max<data()->_voltages[VddBulkErase].max ) qFatal("VddWrite max lower than VddBulkErase max");
  checkTagNames(device, "voltages", names);

  // memory ranges
  names.clear();
  for (uint i=0; i<Nb_MemoryRangeTypes; i++) {
    names += MEMORY_RANGE_TYPE_DATA[i].name;
    if ( !getMemoryRange(MemoryRangeType(i), device) ) continue;
     if ( !(data()->_ranges[i].properties & Programmable) ) continue;
    for (uint k=0; k<i; k++) {
      if ( !(data()->_ranges[k].properties & Present)
           || !(data()->_ranges[k].properties & Programmable) ) continue;
      if ( MemoryRangeType(i)==MemoryDebugVector
           && MemoryRangeType(k)==MemoryProgramExecutive ) continue;
      if ( MemoryRangeType(k)==MemoryDebugVector
           && MemoryRangeType(i)==MemoryProgramExecutive ) continue;
      uint start1 = data()->_ranges[k].start + data()->_ranges[k].hexFileOffset;
      uint end1 = data()->_ranges[k].end + data()->_ranges[k].hexFileOffset;
      uint start2 = data()->_ranges[i].start + data()->_ranges[i].hexFileOffset;
      uint end2 = data()->_ranges[i].end + data()->_ranges[i].hexFileOffset;
      if ( end1>=start2 && start1<=end2 )
        qFatal(QString("Overlapping memory ranges (%1 and %2)").arg(MEMORY_RANGE_TYPE_DATA[k].name).arg(MEMORY_RANGE_TYPE_DATA[i].name));
    }
  }
  checkTagNames(device, "memory", names);
  if ( XOR(data()->_id, (data()->_ranges[MemoryDeviceId].properties & Present)) )
    qFatal("Id present and device id memory range absent or opposite");

  // config words
  QValueVector<Config::Word> cwords = getConfigWords(device);
  uint nbWords = data()->nbWords(MemoryConfig);
  data()->_config->_words.resize(nbWords);
  QMap<QString, QString> cnames; // cname -> mask name
  for (uint i=0; i<nbWords; i++) {
    if ( cwords[i].name.isNull() ) qFatal(QString("Config word #%1 not defined").arg(i));
    data()->_config->_words[i] = cwords[i];
    const Config::Word &word = data()->_config->_words[i];
    for (uint j=0; j<uint(word.masks.count()); j++) {
      const Config::Mask &mask = word.masks[j];
      for (uint k=0; k<uint(mask.values.count()); k++) {
        QString cname = mask.values[k].cname;
        if ( cname.isEmpty() || cname=="_" || cname.startsWith("0x") ) continue;
        if ( cnames.contains(cname) && cnames[cname]!=mask.name )
          qFatal(QString("Duplicated config name for %1/%2").arg(mask.name).arg(mask.values[k].name));
        cnames[cname] = word.masks[j].name;
      }
    }
  }
  // check validity of value names
  for (uint i=0; i<nbWords; i++) {
    const Config::Word &word = data()->_config->_words[i];
    for (uint j=0; j<uint(word.masks.count()); j++) {
      const Config::Mask &mask = word.masks[j];
      for (uint k=0; k<uint(mask.values.count()); k++) {
        const Config::Value &value = mask.values[k];
        if ( !value.isValid() ) continue;
        if ( !data()->_config->checkValueName(mask.name, value.name) )
          qFatal(QString("Malformed value name \"%1\" in mask %2").arg(value.name).arg(mask.name));
      }
    }
  }
  // check if all values are explicit
  for (uint i=0; i<nbWords; i++) {
    const Config::Word &word = data()->_config->_words[i];
    for (uint j=0; j<uint(word.masks.count()); j++) {
      const Config::Mask &mask = word.masks[j];
      for (uint k=0; k<=mask.value; k++) {
        if ( (k & mask.value)!=k ) continue; // outside mask
        if ( !hasValue(mask, k) ) qFatal(QString("Value %1 not defined in mask %2").arg(toHexLabel(k, data()->nbCharsWord(MemoryConfig))).arg(mask.name));
      }
    }
  }
}

void processMirrored(QDomElement element)
{
  QValueVector<RangeData> mirrored;
  QDomNode child = element.firstChild();
  while ( !child.isNull() ) {
    if ( !child.isElement() ) qFatal("\"mirror\" child should be an element");
    QDomElement e = child.toElement();
    if ( e.nodeName()!="range" ) qFatal("\"mirror\" child should be \"range\"");
    RangeData rd;
    bool ok;
    rd.start = fromHexLabel(e.attribute("start"), &ok);
    rd.length = fromHexLabel(e.attribute("end"), &ok)-rd.start+1;
    if ( !mirrored.isEmpty() && rd.length!=mirrored[0].length )
      qFatal("Mirrored are not of the same length");
    mirrored.append(rd);
    child = child.nextSibling();
  }
  if ( !mirrored.isEmpty() ) static_cast<RegistersData *>(data()->_registersData)->mirrored.append(mirrored);
}

void processUnused(QDomElement e)
{
  RangeData rd;
  bool ok;
  rd.start = fromHexLabel(e.attribute("start"), &ok);
  if (!ok) qFatal("Malformed start for unused register");
  rd.length = fromHexLabel(e.attribute("end"), &ok)-rd.start+1;
  if (!ok) qFatal("Malformed end for unused register");
  static_cast<RegistersData *>(data()->_registersData)->unused.append(rd);
}

void processSfr(QDomElement e)
{
  QString name = e.attribute("name");
  if ( name.isEmpty() ) qFatal("SFR cannot have empty name");
  if ( data()->registersData().sfrs.contains(name) || data()->registersData().combined.contains(name) )
    qFatal("SFR name is duplicated");
  bool ok;
  uint address = fromHexLabel(e.attribute("address"), &ok);
  if ( !ok ) qFatal(QString("SFR %1 address %2 is malformed").arg(name).arg(e.attribute("address")));
  uint rlength = data()->registersData().nbBanks * ARCHITECTURE_DATA[data()->architecture()].registerBankLength;
  if ( address>=rlength ) qFatal(QString("Address %1 outside register range").arg(toHexLabel(address, 3)));
  RegisterData rdata;
  rdata.address = address;
  uint nb = data()->registersData().nbBits();
  if ( nb>Device::MAX_NB_PORT_BITS ) qFatal(QString("Need higher MAX_NB_PORT_BITS: %1").arg(nb));
  QString access = e.attribute("access");
  if ( uint(access.length())!=nb ) qFatal("access is missing or malformed");
  QString mclr = e.attribute("mclr");
  if ( uint(mclr.length())!=nb ) qFatal("mclr is missing or malformed");
  QString por = e.attribute("por");
  if ( uint(por.length())!=nb ) qFatal("por is missing or malformed");
  for (uint i=0; i<nb; i++) {
    uint k = nb - i - 1;
    bool ok;
    rdata.bits[k].properties = RegisterBitProperties(fromHex(access[i].latin1(), &ok));
    if ( !ok || rdata.bits[k].properties>MaxRegisterBitProperty ) qFatal(QString("Malformed access bit %1").arg(k));
    rdata.bits[k].mclr = RegisterBitState(fromHex(mclr[i].latin1(), &ok));
    if ( !ok || rdata.bits[k].mclr>Nb_RegisterBitStates ) qFatal(QString("Malformed mclr bit %1").arg(k));
    rdata.bits[k].por = RegisterBitState(fromHex(por[i].latin1(), &ok));
    if ( !ok || rdata.bits[k].por>Nb_RegisterBitStates ) qFatal(QString("Malformed por bit %1").arg(k));
  }
  static_cast<RegistersData *>(data()->_registersData)->sfrs[name] = rdata;
}

void processCombined(QDomElement e)
{
  QString name = e.attribute("name");
  if ( name.isEmpty() ) qFatal("Combined register cannot have empty name");
  if ( data()->registersData().sfrs.contains(name) || data()->registersData().combined.contains(name) )
    qFatal("Combined register name is duplicated");
  bool ok;
  CombinedData rdata;
  rdata.address = fromHexLabel(e.attribute("address"), &ok);
  if ( !ok ) qFatal(QString("Combined %1 address %2 is malformed").arg(name).arg(e.attribute("address")));
  uint rlength = data()->registersData().nbBanks * ARCHITECTURE_DATA[data()->architecture()].registerBankLength;
  if ( rdata.address>=rlength ) qFatal(QString("Address %1 outside register range").arg(toHexLabel(rdata.address, 3)));
  rdata.nbChars = 2*e.attribute("size").toUInt(&ok);
  if ( !ok || rdata.nbChars<2 ) qFatal(QString("Combined %1 size %2 is malformed").arg(name).arg(e.attribute("size")));
  uint end = rdata.address + rdata.nbChars/2 - 1;
  if ( end>=rlength ) qFatal(QString("Address %1 outside register range").arg(toHexLabel(end, 3)));
  static_cast<RegistersData *>(data()->_registersData)->combined[name] = rdata;
}

void processDeviceRegisters(QDomElement element)
{
  QString s = element.attribute("same_as");
  if ( !s.isEmpty() ) {
    if ( !_map.contains(s) ) qFatal(QString("Registers same as unknown device %1").arg(s));
    const Pic::Data *d = static_cast<const Pic::Data *>(_map[s]);
    data()->_registersData = d->_registersData;
    return;
  }

  RegistersData &rdata = *static_cast<RegistersData *>(data()->_registersData);
  bool ok;
  rdata.nbBanks = element.attribute("nb_banks").toUInt(&ok);
  if ( !ok || data()->registersData().nbBanks==0 ) qFatal("Malformed number of banks");
  if ( data()->is18Family() ) {
    rdata.accessBankSplit = fromHexLabel(element.attribute("access_bank_split_offset"), &ok);
    if ( !ok || rdata.accessBankSplit==0 || rdata.accessBankSplit>=0xFF ) qFatal("Malformed access bank split offset");
    rdata.unusedBankMask = fromHexLabel(element.attribute("unused_bank_mask"), &ok);
    if ( !ok || rdata.unusedBankMask>=maxValue(Number::Hex, rdata.nbBanks) ) qFatal("Malformed access unused bank mask");
  } else {
    rdata.accessBankSplit = 0;
    rdata.unusedBankMask = 0;
  }

  QDomNode child = element.firstChild();
  while ( !child.isNull() ) {
    if ( !child.isElement() ) qFatal("\"device\" child should be an element");
    QDomElement e = child.toElement();
    if ( e.nodeName()=="mirror" ) processMirrored(e);
    else if ( e.nodeName()=="unused" ) processUnused(e);
    else if ( e.nodeName()=="combined" ) processCombined(e);
    else if ( e.nodeName()=="sfr" ) processSfr(e);
    else qFatal(QString("Node name \"%1\" is not recognized").arg(e.nodeName()));
    child = child.nextSibling();
  }

  for (uint i=0; i<Device::MAX_NB_PORTS; i++) {
    QString portname = rdata.portName(i);
    if ( portname.isEmpty() ) break;
    bool hasPort = rdata.sfrs.contains(portname);
    QString trisname = rdata.trisName(i);
    if ( trisname.isEmpty() ) continue;
    bool hasTris = rdata.sfrs.contains(trisname);
    if ( !hasPort && hasTris ) qFatal(QString("%1 needs %2 to be present").arg(trisname).arg(portname));
    QString latchname = rdata.latchName(i);
    if ( latchname.isEmpty() ) continue;
    bool hasLatch = rdata.sfrs.contains(latchname);
    if ( !hasPort && hasLatch ) qFatal(QString("%1 needs %2 to be present").arg(latchname).arg(portname));
  }
}

void processRegistersFile(const QString &filename, QStringList &devices)
{
  QDomDocument doc = parseFile(filename);
  QDomElement root = doc.documentElement();
  if ( root.nodeName()!="registers" ) qFatal("root node should be \"registers\"");
  for (QDomNode child=root.firstChild(); !child.isNull(); child = child.nextSibling()) {
    if ( child.isComment() ) qDebug("comment: %s", child.toComment().data().latin1());
    else {
      if ( !child.isElement() ) qFatal("\"registers\" child should be an element");
      if ( child.nodeName()!="device" ) qFatal("Device node should be named \"device\"");
      QDomElement device = child.toElement();
      QString name = device.attribute("name");
      if ( devices.contains(name) ) qFatal(QString("Registers already defined for %1").arg(name));
      if ( _map.contains(name) ) {
        _data = _map[name];
        processDeviceRegisters(device);
        devices.append(name);
      }
    }
  }
}

void processRegisters()
{
  QStringList devices;
  processRegistersFile("registers/registers.xml", devices);
  processRegistersFile("registers/registers_missing.xml", devices);

  // check if we miss any register description
  QMap<QString, Device::Data *>::const_iterator it = _map.begin();
  for (; it!=_map.end(); ++it) {
    _data = it.data();
    if ( !devices.contains(it.key()) ) qWarning("Register description not found for %s", it.key().latin1());
  }
}

virtual void checkPins(const QMap<QString, uint> &pinLabels) const
{
  if ( !pinLabels.contains("VDD") ) qFatal("No VDD pin specified");
  if ( !pinLabels.contains("VSS") ) qFatal("No VSS pin specified");
  QMap<QString, uint>::const_iterator it;
  for (it=pinLabels.begin(); it!=pinLabels.end(); ++it) {
    if ( it.key()=="VDD" || it.key()=="VSS" || it.key().startsWith("CCP") ) continue;
    if ( it.data()!=1 ) qFatal(QString("Duplicated pin \"%1\"").arg(it.key()));
  }
  const Pic::RegistersData &rdata = static_cast<const Pic::RegistersData &>(*_data->registersData());
  for (uint i=0; i<Device::MAX_NB_PORTS; i++) {
    if ( !rdata.hasPort(i) ) continue;
    for (uint k=0; k<Device::MAX_NB_PORT_BITS; k++) {
      if ( !rdata.hasPortBit(i, k) ) continue;
      QString name = rdata.portBitName(i, k);
      if ( !pinLabels.contains(name) ) qFatal(QString("Pin \"%1\" not present").arg(name));
    }
  }
}

virtual void parse()
{
  Device::XmlToDataBase::parse();
  processRegisters();
}

}; // class Pic::XmlToData

} // namespace

//-----------------------------------------------------------------------------
XML_MAIN(Pic::XmlToData)
