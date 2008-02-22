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

//----------------------------------------------------------------------------
class GeneratorCheckHelper
{
public:
  GeneratorCheckHelper();
  virtual ~GeneratorCheckHelper();
  virtual void initSupported() = 0;
  virtual bool init(const Device::Data &data) = 0;
  void cleanup();
  bool isSupported(const Device::Data &data) const { return _supported.contains(&data); }
  virtual PURL::FileType sourceFileType() const = 0;
  const Tool::SourceGenerator *generator() const { return _generator; }
  virtual SourceLine::List configEndLines() const = 0;

protected:
  QValueList<const Device::Data *> _supported;
  Process::StringOutput *_cprocess, *_lprocess;
  Tool::SourceGenerator *_generator;

  friend class GeneratorCheck;
};

class GeneratorCheck : public DeviceTest
{
public:
  GeneratorCheck(GeneratorCheckHelper *helper);
  virtual ~GeneratorCheck();
  virtual bool skip(const Device::Data &data) const;
  virtual void runTest();
  virtual bool init(const Device::Data &data);
  virtual bool execute(const Device::Data &data);
  virtual void cleanup(const Device::Data &data);

protected:
  GeneratorCheckHelper *_helper;
  CLI::View  *_view;
  PURL::File *_fdest, *_fhex;
  Pic::Memory *_memory1;
  QString _source;
};

//----------------------------------------------------------------------------
class ConfigGeneratorCheck : public GeneratorCheck
{
public:
  ConfigGeneratorCheck(GeneratorCheckHelper *helper) : GeneratorCheck(helper), _memory2(0) {}
  virtual bool init(const Device::Data &data);
  virtual bool execute(const Device::Data &data);
  virtual void cleanup(const Device::Data &data);

private:
  Pic::Memory *_memory2;
};

//----------------------------------------------------------------------------
class TemplateGeneratorCheck : public GeneratorCheck
{
public:
  TemplateGeneratorCheck(GeneratorCheckHelper *helper) : GeneratorCheck(helper) {}
  virtual bool init(const Device::Data &data);
};

//----------------------------------------------------------------------------
class GPUtilsGeneratorCheckHelper : public GeneratorCheckHelper
{
public:
  GPUtilsGeneratorCheckHelper();
  virtual void initSupported();
  virtual bool init(const Device::Data &data);
  virtual PURL::FileType sourceFileType() const { return PURL::AsmGPAsm; }
  virtual SourceLine::List configEndLines() const;
};

//----------------------------------------------------------------------------
class SDCCGeneratorCheckHelper : public GeneratorCheckHelper
{
public:
  SDCCGeneratorCheckHelper();
  virtual void initSupported();
  virtual bool init(const Device::Data &data);
  virtual PURL::FileType sourceFileType() const { return PURL::CSource; }
  virtual SourceLine::List configEndLines() const;
};
#endif
