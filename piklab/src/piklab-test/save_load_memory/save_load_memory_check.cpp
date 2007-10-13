/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "save_load_memory_check.h"

#include "devices/base/generic_memory.h"
#include "devices/base/device_group.h"

//----------------------------------------------------------------------------
SaveLoadMemoryCheck::SaveLoadMemoryCheck()
  : _memory1(0), _memory2(0)
{
  _view = new CLI::View;
  PURL::Url dest(PURL::Directory::current(), "test.hex");
  _fdest = new PURL::File(dest, *_view);
}

SaveLoadMemoryCheck::~SaveLoadMemoryCheck()
{
  _fdest->remove();
  delete _fdest;
  delete _view;
}

bool SaveLoadMemoryCheck::init(const Device::Data &data)
{
  _memory1 = data.group().createMemory(data);
  _memory2 = data.group().createMemory(data);
  return true;
}

void SaveLoadMemoryCheck::cleanup(const Device::Data &)
{
  delete _memory1;
  _memory1 = 0;
  delete _memory2;
  _memory2 = 0;
}

bool SaveLoadMemoryCheck::execute(const Device::Data &data)
{
  // create hex file from blank memory
  if ( !_fdest->openForWrite() ) TEST_FAILED("")
  _memory1->save(_fdest->stream(), HexBuffer::IHX32);

  // read hex file
  if ( !_fdest->openForRead() ) TEST_FAILED("")
  QStringList errors, warnings;
  Device::Memory::WarningTypes wtypes;
  if ( !_memory2->load(_fdest->stream(), errors, wtypes, warnings) ) TEST_FAILED(QString("Error loading hex file into memory %1").arg(data.name()))

  // compare checksums
  if ( _memory1->checksum()!=_memory2->checksum() ) TEST_FAILED("Memory saved and loaded is different")
  TEST_PASSED
}

//----------------------------------------------------------------------------
TEST_MAIN(SaveLoadMemoryCheck)
