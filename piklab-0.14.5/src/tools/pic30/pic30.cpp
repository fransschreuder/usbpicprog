/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic30.h"

#include "devices/list/device_list.h"
#include "devices/base/device_group.h"
#include "pic30_compile.h"
#include "pic30_config.h"
#include "devices/pic/pic/pic_memory.h"
#include "common/global/process.h"
#include "pic30_generator.h"

//----------------------------------------------------------------------------
const PIC30::OutputTypeData PIC30::OUTPUT_TYPE_DATA[Nb_OutputTypes] = {
  { I18N_NOOP("Coff"), "coff", PURL::Coff },
  { I18N_NOOP("Elf"),  "elf",  PURL::Elf  }
};

//----------------------------------------------------------------------------
QString PIC30::Base::baseExecutable(bool withWine, OutputType type) const
{
  QString s = "pic30-";
  if ( type==CoffOutput ) s += "coff";
  else s += "elf";
  s += "-";
  switch (_category) {
    case Tool::Compiler:     return s + (withWine ? "cc1" : "gcc");
    case Tool::Assembler:    return s + "as";
    case Tool::Linker:       return s + "ld";
    case Tool::BinToHex:     return s + "bin2hex";
    case Tool::Preprocessor: return s + "gcc";
    default: break;
  }
  Q_ASSERT(false);
  return QString::null;
}

::Process::LineOutput *PIC30::Base::checkExecutableProcess(const PURL::Directory &dir, bool withWine, OutputType type) const
{
  ::Process::LineOutput *process = new ::Process::LineOutput;
  process->setup(dir.path(false) + baseExecutable(withWine, type), checkExecutableOptions(withWine), withWine);
  PURL::Directory wdir = checkExecutableWorkingDirectory();
  if ( !wdir.isEmpty() ) process->setWorkingDirectory(wdir.path(false));
  return process;
}

QString PIC30::Base::baseExecutable(bool withWine) const
{
  const PIC30::Config *config = static_cast<const PIC30::Config *>(_group->createConfig(0));
  QString s = baseExecutable(withWine, config->outputType());
  delete config;
  return s;
}

QStringList PIC30::Base::checkExecutableOptions(bool) const
{
  if ( _category!=Tool::BinToHex ) return "--version";
  return QStringList();
}

bool PIC30::Base::checkExecutableResult(bool withWine, QStringList &lines) const
{
  if ( lines.count()==0 ) return false;
  lines[0] = lines[0].stripWhiteSpace();
  switch (_category) {
    case Tool::Compiler:
      if (withWine) return lines[0].startsWith("GNU C");
      return lines[0].startsWith("pic30");
    case Tool::Assembler: return lines[0].startsWith("GNU assembler");
    case Tool::Linker: return lines[0].startsWith("GNU ld");
    case Tool::BinToHex: return lines.join(" ").contains("Microchip ");
    case Tool::Preprocessor: return lines[0].startsWith("pic30");
    case Tool::Nb_Categories: break;
  }
  Q_ASSERT(false);
  return false;
}

//----------------------------------------------------------------------------
QString PIC30::Group::downloadText() const
{
  return i18n("The <a href=\"http://microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&nodeId=1406&dDocName=en010065&part=SW006012\">PIC30 ToolChain</a> is distributed by Microchip under the GNU Public License.<br>"
              "Sources and packages can be downloaded from <a href=\"http://sourceforge.net/project/showfiles.php?group_id=138852&package_id=181225\">this page</a>.");
}

Tool::Base *PIC30::Group::baseFactory(Tool::Category category) const
{
  if ( category==Tool::Compiler || category==Tool::Assembler
       || category==Tool::Linker || category==Tool::BinToHex
       || category==Tool::Preprocessor ) return new ::PIC30::Base;
  return 0;
}

QValueList<const Device::Data *> PIC30::Group::getSupportedDevices(const QString &) const
{
  QValueList<const Device::Data *> list;
  QValueVector<QString> devices = Device::lister().group("pic")->supportedDevices();
  for (uint i=0; i<devices.count(); i++) {
    const Device::Data *data = Device::lister().data(devices[i]);
    if ( static_cast<const Pic::Data *>(data)->architecture()==Pic::Arch_30X ) list.append(data);
  }
  return list;
}

bool PIC30::Group::needs(Tool::ActionType type, Tool::Category category) const
{
  if ( type==Tool::Project || category==Tool::Linker || category==Tool::BinToHex ) return true;
  return false;
}

Compile::Process *PIC30::Group::processFactory(const Compile::Data &data) const
{
  switch (data.category) {
    case Tool::Compiler:
      if (data.project) return new PIC30::CompileProjectFile;
      return new PIC30::CompileStandaloneFile;
    case Tool::Assembler:
      if (data.project) return new PIC30::AssembleProjectFile;
      return new PIC30::AssembleStandaloneFile;
    case Tool::Linker:
      if (data.project) return new PIC30::LinkProject;
      return new PIC30::LinkStandalone;
    case Tool::BinToHex: return new PIC30::BinToHex;
    default: break;
  }
  Q_ASSERT(false);
  return 0;
}

ToolConfigWidget *PIC30::Group::configWidgetFactory(Tool::Category category, Project *project) const
{
  if ( category==Tool::Compiler ) return new PIC30::CompilerConfigWidget(project);
  if ( category==Tool::Assembler ) return new PIC30::AssemblerConfigWidget(project);
  if ( category==Tool::Linker ) return new PIC30::LinkerConfigWidget(project);
  return 0;
}

ToolchainConfigWidget *PIC30::Group::toolchainConfigWidgetFactory(QWidget *parent) const
{
  return new PIC30::ToolchainConfigWidget(*this, parent);
}

Compile::Config *PIC30::Group::configFactory(::Project *project) const
{
  return new Config(project);
}

PURL::FileType PIC30::Group::implementationType(PURL::ToolType type) const
{
  if ( type==PURL::Assembler ) return PURL::AsmPIC30;
  if ( type==PURL::Compiler ) return PURL::CSource;
  return PURL::Nb_FileTypes;
}

Tool::SourceGenerator *PIC30::Group::sourceGeneratorFactory() const
{
  return new SourceGenerator;
}
