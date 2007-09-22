/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef C18_CONFIG_H
#define C18_CONFIG_H

#include "tools/list/compile_config.h"
#include "tools/base/tool_config_widget.h"

namespace C18
{
//----------------------------------------------------------------------------
class Config : public Compile::Config
{
public:
  Config(Project *project) : Compile::Config(project) {}
  enum { Nb_WarningLevels = 3 };
  static const char * const WARNING_LEVEL_LABELS[Nb_WarningLevels];
  uint warningLevel() const { return QMIN(Compile::Config::warningLevel(Tool::Compiler), uint(Nb_WarningLevels)); }
  void setWarningLevel(uint level) { Compile::Config::setWarningLevel(Tool::Compiler, level); }
};

//----------------------------------------------------------------------------
class CompilerConfigWidget : public ToolConfigWidget
{
Q_OBJECT
public:
  CompilerConfigWidget(Project *project);

private:
  QComboBox *_warningLevel;
  virtual void initEntries();
  virtual void loadConfig(const Compile::Config &config);
  virtual void saveConfig(Compile::Config &config) const;
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
