/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CMDLINE_H
#define CMDLINE_H

#include "libcli/cli_main.h"
#include "cli_interactive.h"
#include "common/global/purl.h"
#include "devices/base/hex_buffer.h"
namespace Device { class Data; class Memory; class MemoryRange; }
namespace Programmer { class Base; class Group; }

namespace CLI
{
class Main : public MainBase
{
Q_OBJECT
public:
  Main();
  virtual ~Main();
  virtual OptionList optionList() const;
  virtual ExitCode run();
  ExitCode prepareCommand(const QString &command);
  ExitCode executeCommand(const QString &command);
  ExitCode executeSetCommand(const QString &property, const QString &value);
  QString executeGetCommand(const QString &property);
  ExitCode list(const QString &command);
  ExitCode extractRange(const QString &range);

private:
  Device::MemoryRange *_range;

  ExitCode formatList();
  ExitCode programmerList();
  ExitCode deviceList();
  ExitCode portList();
  ExitCode propertyList();
  ExitCode rangeList();
  ExitCode checkProgrammer();
};

extern const Programmer::Group *_progGroup;
extern const Device::Data *_device;
extern HexBuffer::Format _format;
extern QString _port, _targetSelfPowered;
extern PURL::Directory _firmwareDir;
extern PURL::Url _hexUrl, _coffUrl;
extern Device::Memory *_memory;
extern Interactive *_interactive;

} // namespace

#endif
