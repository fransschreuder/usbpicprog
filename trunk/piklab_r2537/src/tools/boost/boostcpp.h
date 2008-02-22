/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BOOSTCPP_H
#define BOOSTCPP_H

#include "boost.h"

namespace Boost
{
//----------------------------------------------------------------------------
class CompilerCpp : public Compiler
{
private:
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const;
};

class CompilerCpp16 : public CompilerCpp
{
public:
  virtual QString baseExecutable(bool, Tool::OutputType) const { return "boostc++.pic16.exe"; }
};

class CompilerCpp18 : public CompilerCpp
{
public:
  virtual QString baseExecutable(bool, Tool::OutputType) const { return "boostc++.pic18.exe"; }
};

//----------------------------------------------------------------------------
class GroupCpp : public Group
{
public:
  GroupCpp() : Group(".cpp", "void main(void) {}\n") {}
  virtual QString informationText() const;
  virtual PURL::FileType implementationType(PURL::ToolType type) const { return (type==PURL::ToolType::Compiler ? PURL::CppSource : PURL::Nb_FileTypes); }

private:
  virtual Tool::SourceGenerator *sourceGeneratorFactory() const;
};

class GroupCpp16 : public GroupCpp
{
public:
  virtual QString name() const { return "boostc++16"; }
  virtual QString label() const { return i18n("BoostC++ Compiler for PIC16"); }

private:
  virtual Tool::Base *baseFactory(Tool::Category) const;
  virtual bool supportedArchitecture(Pic::Architecture architecture) const { return ( architecture==Pic::Architecture::P16X ); }
};

class GroupCpp18 : public GroupCpp
{
public:
  virtual QString name() const { return "boostc++18"; }
  virtual QString label() const { return i18n("BoostC++ Compiler for PIC18"); }

private:
  virtual Tool::Base *baseFactory(Tool::Category) const;
  virtual bool supportedArchitecture(Pic::Architecture architecture) const
    { return ( architecture==Pic::Architecture::P18C || architecture==Pic::Architecture::P18F || architecture==Pic::Architecture::P18J ); }
};
} // namespace

#endif
