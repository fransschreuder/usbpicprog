/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SAVE_LOAD_MEMORY_CHECK_H
#define SAVE_LOAD_MEMORY_CHECK_H

#include "piklab-test/base/device_test.h"
#include "common/cli/cli_log.h"
#include "common/global/pfile.h"
namespace Device { class Memory; }

class SaveLoadMemoryCheck : public DeviceTest
{
Q_OBJECT
public:
  SaveLoadMemoryCheck();
  virtual ~SaveLoadMemoryCheck();
  virtual bool init(const Device::Data &data);
  virtual bool execute(const Device::Data &data);
  virtual void cleanup(const Device::Data &data);

private:
  CLI::View  *_view;
  PURL::File *_fdest;
  Device::Memory *_memory1, *_memory2;
};

#endif
