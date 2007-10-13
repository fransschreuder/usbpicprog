/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICC_H
#define PICC_H

#include "tools/base/tool_group.h"

namespace PICC
{
//----------------------------------------------------------------------------
class BaseLite : public Tool::Base
{
public:
  virtual QString baseExecutable(bool, Tool::OutputType) const { return "picl"; }

private:
  virtual QStringList checkExecutableOptions(bool) const { return "--ver"; }
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const;
};

class Base : public Tool::Base
{
public:
  virtual QString baseExecutable(bool, Tool::OutputType) const { return "picc"; }

private:
  virtual QStringList checkExecutableOptions(bool) const { return "--ver"; }
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const;
};

class Base18 : public Tool::Base
{
public:
  virtual QString baseExecutable(bool, Tool::OutputType) const { return "picc18"; }

private:
  virtual QStringList checkExecutableOptions(bool) const { return "--ver"; }
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const;
};

//----------------------------------------------------------------------------
class Group : public Tool::Group
{
public:
  virtual Tool::Category checkDevicesCategory() const { return Tool::Category::Compiler; }
  virtual QStringList checkDevicesOptions(uint) const { return "--CHIPINFO"; }
  virtual Tool::ExecutableType preferedExecutableType() const { return Tool::ExecutableType::Unix; }
  virtual Tool::CompileType compileType() const { return Tool::SeparateFiles; }
  virtual bool needs(Tool::ActionType, Tool::Category) const;
  virtual PURL::FileType implementationType(PURL::ToolType type) const;

private:
  virtual QValueList<const Device::Data *> getSupportedDevices(const QString &s) const;
  virtual Compile::Process *processFactory(const Compile::Data &data) const;
  virtual Compile::Config *configFactory(::Project *project) const;
  virtual Tool::SourceGenerator *sourceGeneratorFactory() const { /*TODO*/ return 0; }
};

//----------------------------------------------------------------------------
class PICCLiteGroup : public Group
{
public:
  virtual QString name() const { return "picclite"; }
  virtual QString label() const { return i18n("PICC Lite Compiler"); }
  virtual QString informationText() const;

private:
  virtual Tool::Base *baseFactory(Tool::Category) const;
};

//----------------------------------------------------------------------------
class PICCGroup : public Group
{
public:
  virtual QString name() const { return "picc"; }
  virtual QString label() const { return i18n("PICC Compiler"); }
  virtual QString informationText() const;

private:
  virtual Tool::Base *baseFactory(Tool::Category) const;
};

//----------------------------------------------------------------------------
class PICC18Group : public Group
{
public:
  virtual QString name() const { return "picc18"; }
  virtual QString label() const { return i18n("PICC-18 Compiler"); }
  virtual QString informationText() const;

private:
  virtual Tool::Base *baseFactory(Tool::Category) const;
};

} // namespace

#endif
