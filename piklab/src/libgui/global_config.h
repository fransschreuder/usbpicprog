/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include "progs/base/generic_prog.h"
#include "common/global/purl.h"
#include "tools/base/tool_group.h"
#include "log_view.h"

class GlobalConfig : public GenericConfig
{
public:
  GlobalConfig() : GenericConfig(QString::null) {}
  PURL::Url openedProject();
  void writeOpenedProject(const PURL::Url &p);
  PURL::UrlList openedFiles();
  void writeOpenedFiles(const PURL::UrlList &files);
  void writeProgrammerGroup(const Programmer::Group &group);
  const Programmer::Group &programmerGroup();
  void writeDebugLevel(Log::DebugLevel level);
  Log::DebugLevel debugLevel();
  void writeLogOutputType(Log::OutputType type);
  Log::OutputType logOutputType();
  void writeAutoRebuildModified(bool autoRebuild);
  bool autoRebuildModified();
  void writeProgramAfterBuild(bool program);
  bool programAfterBuild();
  void writeUserIdSetToChecksum(bool set);
  bool isUserIdSetToChecksum();
  void writeShowLineNumbers(bool show);
  bool showLineNumbers();
};

#endif
