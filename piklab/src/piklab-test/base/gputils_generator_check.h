/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GPUTILS_GENERATOR_CHECK_H
#define GPUTILS_GENERATOR_CHECK_H

#include "device_test.h"
#include "common/cli/cli_log.h"
#include "common/global/pfile.h"
#include "coff/base/disassembler.h"
#include "common/global/process.h"
namespace Pic { class Memory; }

class GPUtilsGeneratorCheck : public DeviceTest
{
Q_OBJECT
public:
  GPUtilsGeneratorCheck();
  virtual ~GPUtilsGeneratorCheck();
  virtual void runTest();
  virtual bool skip(const Device::Data &data) const;
  virtual bool init(const Device::Data &data);
  virtual bool execute(const Device::Data &data);
  virtual void cleanup(const Device::Data &data);

protected:
  CLI::View  *_view;
  PURL::File *_fdest, *_fhex;
  QStringList _supported;
  Tool::SourceGenerator *_generator;
  Process::StringOutput *_cprocess, *_lprocess;
  Pic::Memory *_memory1;
  QString _source;

  QStringList getSupportedDevices(const QString &s) const;
};

#endif
