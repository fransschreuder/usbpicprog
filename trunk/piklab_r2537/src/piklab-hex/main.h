/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MAIN_H
#define MAIN_H

#include "common/global/pfile.h"
#include "devices/base/hex_buffer.h"
#include "common/cli/cli_main.h"

namespace Device
{
  class Data;
  class Memory;
}

namespace CLI
{
//----------------------------------------------------------------------------
enum CommandProperty { NoCommandProperty = 0, NeedSource1 = 1, NeedDestination = 2,
                       NeedSource2 = 4, NeedCorrectInput = 8, NeedDevice = 16 };
Q_DECLARE_FLAGS(CommandProperties, CommandProperty)
Q_DECLARE_OPERATORS_FOR_FLAGS(CommandProperties)

//----------------------------------------------------------------------------
struct FillOptions {
  const char *name, *description;
};
extern const FillOptions FILL_OPTIONS[];

//----------------------------------------------------------------------------
class Main : public MainBase
{
Q_OBJECT
public:
  Main();
  virtual ~Main();

private:
  PURL::Url _dest;
  HexBuffer _source1, _source2;
  HexBuffer::Format _format;
  QValueList<HexBuffer::ErrorData> _errors;
  const Device::Data *_device;
  Device::Memory *_memory;
  QString _fill;

  virtual ExitCode prepareCommand(const QString &command);
  virtual ExitCode executeCommand(const QString &command);
  virtual ExitCode prepareRun(bool &interactive);
  virtual ExitCode executeSetCommand(const QString &property, const QString &value);
  virtual QString executeGetCommand(const QString &property);
  virtual ExitCode list(const QString &listName);
  ExitCode deviceList();
  ExitCode fillOptionList();
};

} // namespace

#endif
