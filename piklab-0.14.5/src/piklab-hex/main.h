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
#include "libcli/cli_main.h"

namespace CLI
{
enum CommandProperty { NoCommandProperty = 0, NeedSource1 = 1, NeedDestination = 2,
                       NeedSource2 = 4, NeedCorrectInput = 8 };
Q_DECLARE_FLAGS(CommandProperties, CommandProperty)
Q_DECLARE_OPERATORS_FOR_FLAGS(CommandProperties)

class Main : public MainBase
{
Q_OBJECT
public:
  Main() {}
  virtual OptionList optionList() const;
  virtual ExitCode run();

private:
  PURL::Url _dest;
  HexBuffer _source1, _source2;
  HexBuffer::Format _format;
  QValueList<HexBuffer::ErrorData> _errors;

  ExitCode prepareCommand(const QString &command);
  ExitCode executeCommand(const QString &command);
};

} // namespace

#endif
