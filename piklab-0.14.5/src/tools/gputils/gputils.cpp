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
#include "devices/list/device_list.h"
#include "common/global/process.h"
#include "gputils_generator.h"

//----------------------------------------------------------------------------
QString GPUtils::Base::baseExecutable(bool) const
{
  if ( _category==Tool::Assembler ) return "gpasm";
  return "gplink";
}

bool GPUtils::Base::checkExecutableResult(bool withWine, QStringList &lines) const
{
  return ( lines.count()>0 && lines[0].startsWith(baseExecutable(withWine)) );
}

//----------------------------------------------------------------------------
QString GPUtils::Group::downloadText() const
{
  return i18n("GPUtils can be downloaded from <a href=\"http://gputils.sourceforge.net\">this page</a>.<br>"
              "A binary package for Mandriva is available <a href=\"href=http://sourceforge.net/project/showfiles.php?group_id=138852&package_id=181224\">here</a>.");
}

Tool::Base *GPUtils::Group::baseFactory(Tool::Category category) const
{
  if ( category==Tool::Assembler || category==Tool::Linker ) return new ::GPUtils::Base;
  return 0;
}

QValueList<const Device::Data *> GPUtils::Group::getSupportedDevices(const QString &s) const
{
  QStringList devices = QStringList::split(' ', s.simplifyWhiteSpace().upper());
  QValueList<const Device::Data *> list;
  for (uint i=0; i<devices.count(); i++) {
    QString name = devices[i];
    if ( devices[i].startsWith("P1") ) name = name.mid(1);
    const Device::Data *data = Device::lister().data(name);
    if (data) list.append(data);
  }
  return list;
}

PURL::Directory GPUtils::Group::autodetectDirectory(Compile::DirectoryType type, const PURL::Directory &execDir, bool withWine) const
{
  switch (type) {
    case Compile::LinkerScriptDirectory: {
      QString exec = execDir.path(false) + base(Tool::Linker)->baseExecutable(withWine);
      ::Process::StringOutput process;
      process.setup(exec, "-h", withWine);
      if ( ::Process::runSynchronously(process, ::Process::Start, 1000)!=::Process::Exited ) return PURL::Directory();
      QString s = process.sout() + process.serr();
      QRegExp re(".*Default linker script path ([^\\n]*)\\n.*");
      if ( !re.exactMatch(s) ) return PURL::Directory();
      return PURL::Directory(re.cap(1));
    }
    case Compile::HeaderDirectory: {
      QString exec = execDir.path(false) + base(Tool::Assembler)->baseExecutable(withWine);
      ::Process::StringOutput process;
      process.setup(exec, "-h", withWine);
      if ( ::Process::runSynchronously(process, ::Process::Start, 1000)!=::Process::Exited ) return PURL::Directory();
      QString s = process.sout() + process.serr();
      QRegExp re(".*Default header file path ([^\\n]*)\\n.*");
      if ( !re.exactMatch(s) ) return PURL::Directory();
      return PURL::Directory(re.cap(1));
    }
    case Compile::ExecutableDirectory:
    case Compile::LibraryDirectory:
    case Compile::SourceDirectory:
    case Compile::Nb_DirectoryTypes: Q_ASSERT(false); break;
  }
  return PURL::Directory();
}

bool GPUtils::Group::needs(Tool::ActionType type, Tool::Category category) const
{
  if ( type==Tool::Project && category==Tool::Linker ) return true;
  return false;
}

Compile::Process *GPUtils::Group::processFactory(const Compile::Data &data) const
{
  if (data.project) {
    if ( data.category==Tool::Assembler ) return new GPUtils::AssembleProjectFile;
    Q_ASSERT( data.category==Tool::Linker );
    return new GPUtils::LinkProject;
  }
  Q_ASSERT( data.category==Tool::Assembler );
  return new GPUtils::AssembleStandaloneFile;
}

ToolConfigWidget *GPUtils::Group::configWidgetFactory(Tool::Category category, Project *project) const
{
  if ( category==Tool::Assembler ) return new GPUtils::AssemblerConfigWidget(project);
  if ( category==Tool::Linker && project ) return new GPUtils::ProjectLinkerConfigWidget(*project);
  return 0;
}

Compile::Config *GPUtils::Group::configFactory(::Project *project) const
{
  return new Config(project);
}

Tool::SourceGenerator *GPUtils::Group::sourceGeneratorFactory() const
{
  return new SourceGenerator;
}
