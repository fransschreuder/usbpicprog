/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "jalv2.h"

#include "jalv2_compile.h"
#include "jalv2_config.h"
#include "devices/pic/pic/pic_memory.h"
#include "devices/list/device_list.h"
#include "common/global/process.h"
#include "tools/jal/jal_generator.h"

//----------------------------------------------------------------------------
bool JALV2::Base::checkExecutableResult(bool, QStringList &lines) const
{
  QStringList tmp;
  for (uint i=0; i<lines.count(); i++)
    if ( !lines[i].contains('\r') ) tmp += lines[i]; // ??
  lines = tmp;
  return ( lines.count()>0 && lines[0].startsWith("jal") );
}

//----------------------------------------------------------------------------
QString JALV2::Group::downloadText() const
{
  return i18n("JAL V2 can be downloaded from <a href=\"http://www.casadeyork.com/jalv2\">this page</a>.");
}

Tool::Base *JALV2::Group::baseFactory(Tool::Category category) const
{
  if ( category==Tool::Compiler ) return new ::JALV2::Base;
  return 0;
}

const char * const SUPPORTED_DEVICES[] = {
  "12C509A", "12F675",
  "16F628",
  "16F818", "16F819",
  "16C84", "16F84",
  "16F87", "16F88",
  "16F873", "16F874", "16F876", "16F877",
  "16F873A", "16F874A", "16F876A", "16F877A",
  "18F242", "18F252", "18F452",
  0
};

QValueList<const Device::Data *> JALV2::Group::getSupportedDevices(const QString &) const
{
  QValueList<const Device::Data *> list;
  for (uint i=0; SUPPORTED_DEVICES[i]; i++) {
    const Device::Data *data = Device::lister().data(SUPPORTED_DEVICES[i]);
    Q_ASSERT(data);
    list.append(data);
  }
  return list;
}

Compile::Process *JALV2::Group::processFactory(const Compile::Data &data) const
{
  Q_ASSERT( data.category==Tool::Compiler );
  return new CompileFile;
}

ToolConfigWidget *JALV2::Group::configWidgetFactory(Tool::Category category, Project *project) const
{
  if ( category==Tool::Compiler ) return new ConfigWidget(project);
  return 0;
}

Compile::Config *JALV2::Group::configFactory(::Project *project) const
{
  return new Config(project);
}

Tool::SourceGenerator *JALV2::Group::sourceGeneratorFactory() const
{
  return new JAL::SourceGenerator;
}

