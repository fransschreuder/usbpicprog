/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BOOSTBASIC_H
#define BOOSTBASIC_H

#include "boost.h"

namespace Boost
{
//----------------------------------------------------------------------------
class CompilerBasic : public Compiler
{
private:
  virtual bool checkExecutableResult(bool withWine, QStringList &lines) const;
};

class CompilerBasic16 : public CompilerBasic
{
public:
  virtual QString baseExecutable(bool) const { return "boostbasic.pic16.exe"; }
};

class CompilerBasic18 : public CompilerBasic
{
public:
  virtual QString baseExecutable(bool) const { return "boostbasic.pic18.exe"; }
};

//----------------------------------------------------------------------------
class GroupBasic : public Group
{
public:
  GroupBasic() : Group(".bas", "Sub main()\nEnd Sub\n") {}
  virtual QString downloadText() const;
  virtual PURL::FileType implementationType(PURL::ToolType type) const { return (type==PURL::Compiler ? PURL::BasicSource : PURL::Nb_FileTypes); }

private:
  virtual Tool::SourceGenerator *sourceGeneratorFactory() const;
};

class GroupBasic16 : public GroupBasic
{
public:
  virtual QString name() const { return "boostbasic16"; }
  virtual QString label() const { return i18n("BoostBasic Compiler for PIC16"); }

private:
  virtual Tool::Base *baseFactory(Tool::Category) const;
  virtual bool supportedArchitecture(Pic::Architecture architecture) const { return ( architecture==Pic::Arch_16X ); }
};

class GroupBasic18 : public GroupBasic
{
public:
  virtual QString name() const { return "boostbasic18"; }
  virtual QString label() const { return i18n("BoostBasic Compiler for PIC18"); }

private:
  virtual Tool::Base *baseFactory(Tool::Category) const;
  virtual bool supportedArchitecture(Pic::Architecture architecture) const
    { return ( architecture==Pic::Arch_18C || architecture==Pic::Arch_18F || architecture==Pic::Arch_18J ); }
};

} // namespace

#endif
