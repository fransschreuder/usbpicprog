/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "picc.h"

#include <qregexp.h>

#include "picc_compile.h"
#include "picc_config.h"
#include "devices/pic/pic/pic_memory.h"
#include "devices/list/device_list.h"
#include "common/global/process.h"

//----------------------------------------------------------------------------
bool PICC::BaseLite::checkExecutableResult(bool, QStringList &lines) const
{
  return ( lines.count()>0 && lines[0].startsWith("HI-TECH PICC-Lite COMPILER") );
}

bool PICC::Base::checkExecutableResult(bool, QStringList &lines) const
{
  return ( lines.count()>0 && lines[0].startsWith("HI-TECH PICC COMPILER") );
}

bool PICC::Base18::checkExecutableResult(bool, QStringList &lines) const
{
  return ( lines.count()>0 && lines[0].startsWith("HI-TECH PICC-18 COMPILER") );
}

//----------------------------------------------------------------------------
QValueList<const Device::Data *> PICC::Group::getSupportedDevices(const QString &s) const
{
  QValueList<const Device::Data *> list;
  QStringList lines = QStringList::split('\n', s);
  for (uint i=0; i<lines.count(); i++) {
    QRegExp re("([A-Za-z0-9]+):.*");
    if ( !re.exactMatch(lines[i]) ) continue;
    const Device::Data *data = Device::lister().data(re.cap(1));
    if (data) list.append(data);
  }
  return list;
}

bool PICC::Group::needs(Tool::ActionType type, Tool::Category category) const
{
  return ( type==Tool::Project && category!=Tool::BinToHex );
}

Compile::Process *PICC::Group::processFactory(const Compile::Data &data) const
{
  switch (data.category) {
    case Tool::Compiler:
      if (data.project) return new PICC::CompileProjectFile;
      return new PICC::CompileStandaloneFile;
    case Tool::Assembler:
      if (data.project) return new PICC::AssembleProjectFile;
      return new PICC::AssembleStandaloneFile;
    case Tool::Linker:
      if (data.project) return new PICC::LinkProject;
      break;
    default: break;
  }
  Q_ASSERT(false);
  return 0;
}

ToolConfigWidget *PICC::Group::configWidgetFactory(Tool::Category category, Project *project) const
{
  if ( category==Tool::Compiler || category==Tool::Assembler || (category==Tool::Linker && project))
    return new PICC::ConfigWidget(project);
  return 0;
}

Compile::Config *PICC::Group::configFactory(::Project *project) const
{
  return new Config(project);
}

PURL::FileType PICC::Group::implementationType(PURL::ToolType type) const
{
  if ( type==PURL::Assembler ) return PURL::AsmPICC;
  if ( type==PURL::Compiler ) return PURL::CSource;
  return PURL::Nb_FileTypes;
}

//----------------------------------------------------------------------------
QString PICC::PICCLiteGroup::downloadText() const
{
  return i18n("<a href=\"http://www.htsoft.com\">PICC-Lite</a> is a freeware C compiler distributed by HTSoft.");
}

Tool::Base *PICC::PICCLiteGroup::baseFactory(Tool::Category category) const
{
  if ( category==Tool::Compiler || category==Tool::Assembler
       || category==Tool::Linker || category==Tool::Preprocessor )
    return new BaseLite;
  return 0;
}

//----------------------------------------------------------------------------
QString PICC::PICCGroup::downloadText() const
{
  return i18n("<a href=\"http://www.htsoft.com\">PICC</a> is a C compiler distributed by HTSoft.");
}

Tool::Base *PICC::PICCGroup::baseFactory(Tool::Category category) const
{
  if ( category==Tool::Compiler || category==Tool::Assembler
       || category==Tool::Linker || category==Tool::Preprocessor ) return new ::PICC::Base;
  return 0;
}

//----------------------------------------------------------------------------
QString PICC::PICC18Group::downloadText() const
{
  return i18n("<a href=\"http://www.htsoft.com\">PICC 18</a> is a C compiler distributed by HTSoft.");
}

Tool::Base *PICC::PICC18Group::baseFactory(Tool::Category category) const
{
  if ( category==Tool::Compiler || category==Tool::Assembler
       || category==Tool::Linker || category==Tool::Preprocessor )
    return new Base18;
  return 0;
}
