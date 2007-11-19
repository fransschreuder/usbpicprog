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
namespace Device { class Data; class Memory; }
namespace Coff { class Symbol; class AuxSymbol; }

namespace CLI
{
//----------------------------------------------------------------------------
enum CommandProperty { NoCommandProperty = 0, NeedSource = 1, NeedDestination = 2, NeedDevice = 4 };
Q_DECLARE_FLAGS(CommandProperties, CommandProperty)
Q_DECLARE_OPERATORS_FOR_FLAGS(CommandProperties)

//----------------------------------------------------------------------------
class Main : public MainBase
{
Q_OBJECT
public:
  Main();
  virtual ~Main();

private:
  const Device::Data *_device;
  PURL::Url           _source, _dest;

  virtual ExitCode prepareCommand(const QString &command);
  virtual ExitCode executeCommand(const QString &command);
  virtual ExitCode prepareRun(bool &interactive);
  virtual ExitCode executeSetCommand(const QString &property, const QString &value);
  ExitCode executeCommandArchive(const QString &command, Log::KeyList &keys);
  ExitCode executeCommandObject(const QString &command, Log::KeyList &keys);
  virtual QString executeGetCommand(const QString &property);
  virtual ExitCode list(const QString &listName);
  ExitCode deviceList();
  static QString prettyAuxSymbol(const Coff::AuxSymbol &aux);
  static QString prettySymbol(const Coff::Symbol &sym);
};

} // namespace

#endif
