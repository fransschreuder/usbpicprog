/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DEVICE_TEST_H
#define DEVICE_TEST_H

#include "piklab-test/base/main_test.h"
namespace Device { class Data; }

class DeviceTest : public Test
{
public:
  virtual Piklab::OptionList optionList() const;

protected:
  virtual bool execute();
  virtual bool skip(const Device::Data &) const { return false; }
  virtual bool init(const Device::Data &) { return true; } // returns false if skipped or failed
  virtual bool execute(const Device::Data &data) = 0; // returns false if skipped or failed
  virtual void cleanup(const Device::Data &) {}
  virtual void checkArguments();

private:
  QString _device;
};

#endif
