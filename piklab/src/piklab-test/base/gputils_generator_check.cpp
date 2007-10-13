/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "gputils_generator_check.h"

#include "devices/base/device_group.h"
#include "devices/pic/pic/pic_memory.h"
#include "tools/gputils/gputils_generator.h"
#include "devices/list/device_list.h"

GPUtilsGeneratorCheck::GPUtilsGeneratorCheck()
  : _fdest(0), _fhex(0), _cprocess(0), _lprocess(0), _memory1(0)
{
  _view = new CLI::View;
  _generator = new GPUtils::SourceGenerator;
}

void GPUtilsGeneratorCheck::runTest()
{
  Process::StringOutput p;
  QStringList options;
  options += "-l";
  p.setup("gpasm", options, false);
  Process::runSynchronously(p, Process::Start, 2000); // 2s timeout
  _supported = getSupportedDevices(p.sout());
  DeviceTest::runTest();
}

QStringList GPUtilsGeneratorCheck::getSupportedDevices(const QString &s) const
{
  QStringList devices = QStringList::split(' ', s.simplifyWhiteSpace().upper());
  QStringList list;
  for (uint i=0; i<devices.count(); i++) {
    QString name = devices[i];
    if ( devices[i].startsWith("P1") ) name = name.mid(1);
    const Device::Data *data = Device::lister().data(name);
    if (data) list.append(name);
  }
  return list;
}

GPUtilsGeneratorCheck::~GPUtilsGeneratorCheck()
{
  delete _view;
  delete _generator;
}

bool GPUtilsGeneratorCheck::skip(const Device::Data &data) const
{
  return !_supported.contains(data.name());
}

bool GPUtilsGeneratorCheck::init(const Device::Data &data)
{
  PURL::Url dest(PURL::Directory::current(), "test.asm");
  _fdest = new PURL::File(dest, *_view);
  _cprocess = new Process::StringOutput;
  QStringList options;
  options = "-c";
  options += "-p" + GPUtils::toDeviceName(data.name());
  options += "test.asm";
  _cprocess->setup("gpasm", options, false);
  _lprocess = new Process::StringOutput;
  options = "-o";
  options += "test.hex";
  options += "test.o";
  _lprocess->setup("gplink", options, false);
  PURL::Url hex(PURL::Directory::current(), "test.hex");
  _fhex = new PURL::File(hex, *_view);
  _memory1 = static_cast<Pic::Memory *>(data.group().createMemory(data));
  return true;
}

bool GPUtilsGeneratorCheck::execute(const Device::Data &data)
{
  // create asm file from template source code
  if ( !_fdest->openForWrite() ) TEST_FAILED("");
  _fdest->appendText(_source);
  _fdest->close();

  // run compiler
  Process::State state = Process::runSynchronously(*_cprocess, Process::Start, 2000); // 2s timeout
  if ( state!=Process::Exited ) TEST_FAILED("Error while running compilation")
  if ( _cprocess->exitCode()!=0 ) TEST_FAILED(QString("Error in compilation for %1:\n%2%3").arg(data.name()).arg(_cprocess->sout()+_cprocess->serr()).arg(QString::null))

  // run linker
  state = Process::runSynchronously(*_lprocess, Process::Start, 2000); // 2s timeout
  if ( state!=Process::Exited ) TEST_FAILED("Error while running linking")
  if ( _lprocess->exitCode()!=0 ) TEST_FAILED(QString("Error in linking for %1:\n%2%3").arg(data.name()).arg(_lprocess->sout()+_lprocess->serr()).arg(QString::null))

  // load hex file
  if ( !_fhex->openForRead() ) TEST_FAILED("")
  QStringList errors, warnings;
  Device::Memory::WarningTypes warningTypes;
  if ( !_memory1->load(_fhex->stream(), errors, warningTypes, warnings) ) TEST_FAILED(QString("Error loading hex into memory: %1").arg(errors.join(" ")))
  //if ( warningTypes!=Device::Memory::NoWarning ) TEST_FAILED(QString("Warning loading hex into memory: %1").arg(warnings.join(" ")))

  TEST_PASSED
}

void GPUtilsGeneratorCheck::cleanup(const Device::Data &)
{
  delete _memory1;
  _memory1 = 0;
  //_fhex->remove();
  delete _fhex;
  _fhex = 0;
  delete _lprocess;
  _lprocess = 0;
  delete _cprocess;
  _cprocess = 0;
  _fdest->remove();
  delete _fdest;
  _fdest = 0;
}
