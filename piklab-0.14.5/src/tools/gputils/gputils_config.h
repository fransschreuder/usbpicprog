/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GPUTILS_CONFIG_H
#define GPUTILS_CONFIG_H

#include "tools/list/compile_config.h"
#include "tools/base/tool_config_widget.h"

namespace GPUtils
{
//----------------------------------------------------------------------------
class Config : public Compile::Config
{
public:
  Config(Project *project) : Compile::Config(project) {}
  enum { Nb_WarningLevels = 3 };
  static const char * const WARNING_LEVEL_LABELS[Nb_WarningLevels];
  uint gpasmWarningLevel() const;
  void setGPAsmWarningLevel(uint level) { setWarningLevel(Tool::Assembler, level); }
};

//----------------------------------------------------------------------------
class AssemblerConfigWidget : public ToolConfigWidget
{
Q_OBJECT
public:
  AssemblerConfigWidget(Project *project);
  virtual void initEntries();

protected:
  QComboBox *_gpasmWarning;
  virtual void loadConfig(const Compile::Config &config);
  virtual void saveConfig(Compile::Config &config) const;
};

//----------------------------------------------------------------------------
class ProjectLinkerConfigWidget : public ToolConfigWidget
{
Q_OBJECT
public:
  ProjectLinkerConfigWidget(Project &project);
  virtual void initEntries();
};

} // namespace

#endif
