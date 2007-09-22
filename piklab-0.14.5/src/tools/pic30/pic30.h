/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC30_H
#define PIC30_H

#include "tools/base/tool_group.h"

namespace PIC30
{
enum OutputType { CoffOutput = 0, ElfOutput, Nb_OutputTypes };
struct OutputTypeData {
  const char     *label, *key;
  PURL::FileType  type;
};
extern const OutputTypeData OUTPUT_TYPE_DATA[Nb_OutputTypes];

//----------------------------------------------------------------------------
class Base : public Tool::Base
{
public:
  QString baseExecutable(bool withWine, OutputType type) const;
  ::Process::LineOutput *checkExecutableProcess(const PURL::Directory &dir, bool withWine, OutputType type) const;
  virtual QString baseExecutable(bool withWine) const;

private:
  virtual QStringList checkExecutableOptions(bool withWine) const;
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const;
};

//----------------------------------------------------------------------------
class Group : public Tool::Group
{
public:
  virtual QString name() const { return "pic30"; }
  virtual QString label() const { return i18n("PIC30 Toolchain"); }
  virtual QString downloadText() const;
  virtual Tool::Category checkDevicesCategory() const { return Tool::Nb_Categories; }
  virtual Tool::ExecutableType preferedExecutableType() const { return Tool::Unix; }
  virtual Tool::CompileType compileType() const { return Tool::SeparateFiles; }
  virtual PURL::FileType linkerScriptType() const { return PURL::Gld; }
  virtual bool needs(Tool::ActionType type, Tool::Category category) const;
  virtual PURL::FileType implementationType(PURL::ToolType type) const;

private:
  virtual Tool::Base *baseFactory(Tool::Category category) const;
  virtual QValueList<const Device::Data *> getSupportedDevices(const QString &s) const;
  virtual Compile::Process *processFactory(const Compile::Data &data) const;
  virtual ToolConfigWidget *configWidgetFactory(Tool::Category category, ::Project *project) const;
  virtual Compile::Config *configFactory(::Project *project) const;
  virtual ToolchainConfigWidget *toolchainConfigWidgetFactory(QWidget *parent) const;
  virtual Tool::SourceGenerator *sourceGeneratorFactory() const;
};

} // namespace

#endif
