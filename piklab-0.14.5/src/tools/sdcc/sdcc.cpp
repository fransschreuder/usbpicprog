/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "sdcc.h"

#include "devices/list/device_list.h"
#include "sdcc_compile.h"
#include "sdcc_config.h"
#include "devices/pic/pic/pic_memory.h"
#include "common/global/pfile.h"
#include "common/global/process.h"
#include "sdcc_generator.h"

//----------------------------------------------------------------------------
const SDCC::FamilyData SDCC::FAMILY_DATA[Nb_Families] = {
  { "pic14" },
  { "pic16" }
};

SDCC::Family SDCC::family(const QString &device)
{
  const Device::Data *data = Device::lister().data(device);
  switch (static_cast<const Pic::Data *>(data)->architecture()) {
    case Pic::Arch_10X:
    case Pic::Arch_16X: return P14;
    case Pic::Arch_17X:
    case Pic::Arch_18C:
    case Pic::Arch_18F:
    case Pic::Arch_18J: return P16;
    case Pic::Arch_24F:
    case Pic::Arch_24H:
    case Pic::Arch_30X:
    case Pic::Nb_Architectures: break;
  }
  Q_ASSERT(false);
  return Nb_Families;
}

//----------------------------------------------------------------------------
bool SDCC::Base::checkExecutableResult(bool, QStringList &lines) const
{
  return ( lines.count()>0 && lines[0].startsWith("SDCC") );
}

//----------------------------------------------------------------------------
QString SDCC::Group::downloadText() const
{
  return i18n("The Small Devices C Compiler can be downloaded from <a href=\"http://sdcc.sourceforge.net\">this page</a>.<br>"
              "A binary package for Mandriva is available <a href=\"http://sourceforge.net/project/showfiles.php?group_id=138852&package_id=181223\">here</a>.");
}

const ::Tool::Base *SDCC::Group::base(Tool::Category category) const
{
  if ( category==Tool::Assembler ) return Tool::lister().group("gputils")->base(category);
  return Tool::Group::base(category);
}

Tool::Base *SDCC::Group::baseFactory(Tool::Category category) const
{
  if ( category==Tool::Compiler || category==Tool::Preprocessor || category==Tool::Linker) return new ::SDCC::Base;
  return 0;
}

SDCC::Group::Group()
  : _checkDevicesTmp(0, ".c")
{
  // used to obtain device list
  if ( _checkDevicesTmp.openForWrite() ) _checkDevicesTmp.appendText("void main(void) {}\n");
  _checkDevicesTmp.close();
}

QStringList SDCC::Group::checkDevicesOptions(uint i) const
{
  QStringList options;
  options += QString("-m") + SDCC::FAMILY_DATA[i].name;
  options += "-phelp";
  options += _checkDevicesTmp.url().filepath(false);
  return options;
}

QValueList<const Device::Data *> SDCC::Group::getSupportedDevices(const QString &s) const
{
  QValueList<const Device::Data *> list;
  QStringList devices = QStringList::split(' ', s.simplifyWhiteSpace().upper());
  for (uint i=0; i<devices.count(); i++) {
    QString name = devices[i];
    if ( devices[i].startsWith("P1") ) name = name.mid(1);
    const Device::Data *data = Device::lister().data(name);
    if (data) list.append(data);
  }
  return list;
}

PURL::Directory SDCC::Group::autodetectDirectory(Compile::DirectoryType type, const PURL::Directory &dir, bool withWine) const
{
  return Tool::lister().group("gputils")->autodetectDirectory(type, dir, withWine);
}

bool SDCC::Group::needs(Tool::ActionType type, Tool::Category category) const
{
  if ( type==Tool::Project && category==Tool::Linker ) return true;
  return false;
}

Compile::Process *SDCC::Group::processFactory(const Compile::Data &data) const
{
  switch (data.category) {
    case Tool::Assembler:
      return Tool::lister().group("gputils")->processFactory(data);
    case Tool::Compiler:
      if (data.project) return new SDCC::CompileProjectFile;
      return new SDCC::CompileStandaloneFile;
    case Tool::Linker:
      return new SDCC::LinkProjectFile;
    default: break;
  }
  return 0;
}

ToolConfigWidget *SDCC::Group::configWidgetFactory(Tool::Category category, Project *project) const
{
  switch (category) {
    case Tool::Compiler: return new SDCC::CompilerConfigWidget(project);
    case Tool::Linker: return new SDCC::LinkerConfigWidget(project);
    default: break;
  }
  return 0;
}

Compile::Config *SDCC::Group::configFactory(::Project *project) const
{
  return new Config(project);
}

PURL::FileType SDCC::Group::implementationType(PURL::ToolType type) const
{
  if ( type==PURL::Assembler ) return PURL::AsmGPAsm;
  if ( type==PURL::Compiler ) return PURL::CSource;
  return PURL::Nb_FileTypes;
}

Tool::SourceGenerator *SDCC::Group::sourceGeneratorFactory() const
{
  return new SourceGenerator;
}
