/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PIC30_CONFIG_H
#define PIC30_CONFIG_H

#include "tools/list/compile_config.h"
#include "tools/base/tool_config_widget.h"
#include "tools/base/toolchain_config_widget.h"
#include "pic30.h"

namespace PIC30
{
//----------------------------------------------------------------------------
class ToolchainConfigWidget : public ::ToolchainConfigWidget
{
Q_OBJECT
public:
  ToolchainConfigWidget(const Tool::Group &group, QWidget *parent) : ::ToolchainConfigWidget(group, parent) {}

public slots:
  virtual void loadConfig();
  virtual void saveConfig();

private:
  QComboBox *_outputType;

  virtual void addCustomExecutableOptions(Container *container);
  virtual QString baseExecutable(Tool::Category category) const;
  virtual ::Process::LineOutput *checkExecutableProcess(Tool::Category category) const;
};

//----------------------------------------------------------------------------
class Config : public Compile::Config
{
public:
  Config(Project *project) : Compile::Config(project) {}
  OutputType outputType() const;
  void setOutputType(OutputType type);
};

//----------------------------------------------------------------------------
class CompilerConfigWidget : public ToolConfigWidget
{
Q_OBJECT
public:
  CompilerConfigWidget(Project *project);
  virtual void initEntries();
};

//----------------------------------------------------------------------------
class AssemblerConfigWidget : public ToolConfigWidget
{
Q_OBJECT
public:
  AssemblerConfigWidget(Project *project);
  virtual void initEntries();
};

//----------------------------------------------------------------------------
class LinkerConfigWidget : public ToolConfigWidget
{
Q_OBJECT
public:
  LinkerConfigWidget(Project *project);
  virtual void initEntries();
};

} // namespace

#endif
