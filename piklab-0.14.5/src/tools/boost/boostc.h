/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BOOSTC_H
#define BOOSTC_H

#include "boost.h"

namespace Boost
{
//----------------------------------------------------------------------------
class CompilerC : public Compiler
{
private:
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const;
};

class CompilerC16 : public CompilerC
{
public:
  virtual QString baseExecutable(bool) const { return "boostc.pic16.exe"; }
};

class CompilerC18 : public CompilerC
{
public:
  virtual QString baseExecutable(bool) const { return "boostc.pic18.exe"; }
};

//----------------------------------------------------------------------------
class GroupC : public Group
{
public:
  GroupC() : Group(".c", "void main(void) {}\n") {}
  virtual QString downloadText() const;
  virtual PURL::FileType implementationType(PURL::ToolType type) const { return (type==PURL::Compiler ? PURL::CSource : PURL::Nb_FileTypes); }

private:
  virtual Tool::SourceGenerator *sourceGeneratorFactory() const;
};

class GroupC16 : public GroupC
{
public:
  virtual QString name() const { return "boostc16"; }
  virtual QString label() const { return i18n("BoostC Compiler for PIC16"); }

private:
  virtual Tool::Base *baseFactory(Tool::Category) const;
  virtual bool supportedArchitecture(Pic::Architecture architecture) const { return ( architecture==Pic::Arch_16X ); }
};

class GroupC18 : public GroupC
{
public:
  virtual QString name() const { return "boostc18"; }
  virtual QString label() const { return i18n("BoostC Compiler for PIC18"); }

private:
  virtual Tool::Base *baseFactory(Tool::Category) const;
  virtual bool supportedArchitecture(Pic::Architecture architecture) const
    { return ( architecture==Pic::Arch_18C || architecture==Pic::Arch_18F || architecture==Pic::Arch_18J ); }
};

} // namespace

#endif
