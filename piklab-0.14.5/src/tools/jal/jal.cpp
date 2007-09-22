/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "jal.h"

#include "jal_compile.h"
#include "jal_config.h"
#include "devices/pic/pic/pic_memory.h"
#include "devices/list/device_list.h"
#include "common/global/process.h"
#include "jal_generator.h"

//----------------------------------------------------------------------------
bool JAL::Base::checkExecutableResult(bool, QStringList &lines) const
{
  QStringList tmp;
  for (uint i=0; i<lines.count(); i++)
    if ( !lines[i].contains('\r') ) tmp += lines[i]; // ??
  lines = tmp;
  return ( lines.count()>0 && lines[0].startsWith("jal") );
}

//----------------------------------------------------------------------------
QString JAL::Group::downloadText() const
{
  return i18n("JAL can be downloaded from <a href=\"http://jal.sourceforge.net\">this page</a>.");
}

Tool::Base *JAL::Group::baseFactory(Tool::Category category) const
{
  if ( category==Tool::Compiler ) return new ::JAL::Base;
  return 0;
}

const char * const SUPPORTED_DEVICES[] = {
  "12C508", "12C509A", "12CE674", "12F629", "12F675",
  "16C84", "16F84", "16F88", "16F873", "16F876", "16F877", "16F628",
  "18F242", "18F252", "18F442", "18F452",
  0
};

QValueList<const Device::Data *> JAL::Group::getSupportedDevices(const QString &) const
{
  QValueList<const Device::Data *> list;
  for (uint i=0; SUPPORTED_DEVICES[i]; i++) {
    const Device::Data *data = Device::lister().data(SUPPORTED_DEVICES[i]);
    Q_ASSERT(data);
    list.append(data);
  }
  return list;
}

Compile::Process *JAL::Group::processFactory(const Compile::Data &data) const
{
  Q_ASSERT( data.category==Tool::Compiler );
  return new CompileFile;
}

ToolConfigWidget *JAL::Group::configWidgetFactory(Tool::Category category, Project *project) const
{
  if ( category==Tool::Compiler ) return new ConfigWidget(project);
  return 0;
}

Compile::Config *JAL::Group::configFactory(::Project *project) const
{
  return new Config(project);
}

Tool::SourceGenerator *JAL::Group::sourceGeneratorFactory() const
{
  return new SourceGenerator;
}

