/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2003-2004 Alain Gibaud <alain.gibaud@free.fr>           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "gputils.h"

#include <qregexp.h>

#include "gputils_compile.h"
#include "gputils_config.h"
#include "devices/pic/pic/pic_memory.h"
#include "common/global/process.h"
#include "gputils_generator.h"

//----------------------------------------------------------------------------
QString GPUtils::Base::baseExecutable(bool, Tool::OutputType) const
{
  if ( _category==Tool::Category::Assembler ) return "gpasm";
  return "gplink";
}

bool GPUtils::Base::checkExecutableResult(bool withWine, QStringList &lines) const
{
  return ( lines.count()>0 && lines[0].startsWith(baseExecutable(withWine, Tool::OutputType::Coff)) );
}

//----------------------------------------------------------------------------
QString GPUtils::Group::informationText() const
{
  return i18n("<a href=\"%1\">GPUtils</a> is an open-source assembler and linker suite.<br>").arg("http://gputils.sourceforge.net");
}

Tool::Base *GPUtils::Group::baseFactory(Tool::Category category) const
{
  if ( category==Tool::Category::Assembler || category==Tool::Category::Linker ) return new ::GPUtils::Base;
  return 0;
}

PURL::Directory GPUtils::Group::autodetectDirectory(Compile::DirectoryType type, const PURL::Directory &execDir, bool withWine) const
{
  switch (type.type()) {
    case Compile::DirectoryType::LinkerScript: {
      QString exec = execDir.path() + base(Tool::Category::Linker)->baseExecutable(withWine, Tool::OutputType::Coff);
      ::Process::StringOutput process;
      process.setup(exec, "-h", withWine);
      if ( ::Process::runSynchronously(process, ::Process::Start, 1000)!=::Process::Exited ) return PURL::Directory();
      QString s = process.sout() + process.serr();
      QRegExp re(".*Default linker script path ([^\\n]*)\\n.*");
      if ( !re.exactMatch(s) ) return PURL::Directory();
      return PURL::Directory(re.cap(1));
    }
    case Compile::DirectoryType::Header: {
      QString exec = execDir.path() + base(Tool::Category::Assembler)->baseExecutable(withWine, Tool::OutputType::Coff);
      ::Process::StringOutput process;
      process.setup(exec, "-h", withWine);
      if ( ::Process::runSynchronously(process, ::Process::Start, 1000)!=::Process::Exited ) return PURL::Directory();
      QString s = process.sout() + process.serr();
      QRegExp re(".*Default header file path ([^\\n]*)\\n.*");
      if ( !re.exactMatch(s) ) return PURL::Directory();
      return PURL::Directory(re.cap(1));
    }
    case Compile::DirectoryType::Executable:
    case Compile::DirectoryType::Library:
    case Compile::DirectoryType::Source:
    case Compile::DirectoryType::Nb_Types: break;
  }
  return PURL::Directory();
}

bool GPUtils::Group::needs(Tool::ActionType type, Tool::Category category) const
{
  if ( type==Tool::Project && category==Tool::Category::Linker ) return true;
  return false;
}

Compile::Process *GPUtils::Group::processFactory(const Compile::Data &data) const
{
  if (data.project) {
    if ( data.category==Tool::Category::Assembler ) return new GPUtils::AssembleProjectFile;
    Q_ASSERT( data.category==Tool::Category::Linker );
    return new GPUtils::LinkProject;
  }
  Q_ASSERT( data.category==Tool::Category::Assembler );
  return new GPUtils::AssembleStandaloneFile;
}

Compile::Config *GPUtils::Group::configFactory(::Project *project) const
{
  return new Config(project);
}

Tool::SourceGenerator *GPUtils::Group::sourceGeneratorFactory() const
{
  return new SourceGenerator;
}
