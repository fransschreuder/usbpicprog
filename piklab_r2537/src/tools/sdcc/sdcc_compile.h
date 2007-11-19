/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SDCC_COMPILE_H
#define SDCC_COMPILE_H

#include "tools/list/compile_process.h"

namespace SDCC
{
//-----------------------------------------------------------------------------
class Process : public Compile::Process
{
Q_OBJECT
protected:
  virtual QString deviceName() const;
  virtual QString familyName() const;
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual QString outputFiles() const;
  virtual void logStderrLine(const QString &line);
  virtual bool hasLinkerScript() const { return ( _data.linkType==Compile::Icd2Linking || Main::toolGroup().hasCustomLinkerScript(_data.project) ); }
};

//-----------------------------------------------------------------------------
class CompileStandaloneFile : public Process
{
Q_OBJECT
private:
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual QString outputFiles() const;
};

//-----------------------------------------------------------------------------
class CompileProjectFile : public Process
{
Q_OBJECT
private:
  virtual QStringList genericArguments(const Compile::Config &config) const;
};

//-----------------------------------------------------------------------------
class LinkProjectFile : public Process
{
Q_OBJECT
private:
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual QString outputFiles() const;
  virtual QString objectExtension() const { return "o"; }
};

} // namespace

#endif
