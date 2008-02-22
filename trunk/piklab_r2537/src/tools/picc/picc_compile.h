/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICC_COMPILE_H
#define PICC_COMPILE_H

#include "tools/list/compile_process.h"

namespace PICC
{
//-----------------------------------------------------------------------------
class Process : public Compile::Process
{
Q_OBJECT
protected:
  virtual QString deviceName() const { return _data.device; }
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual void logStderrLine(const QString &line);
};

//-----------------------------------------------------------------------------
class CompileStandaloneFile : public Process
{
Q_OBJECT
protected:
  virtual QString outputFiles() const;
  virtual QStringList genericArguments(const Compile::Config &config) const;
};

//-----------------------------------------------------------------------------
class CompileProjectFile : public Process
{
Q_OBJECT
protected:
  virtual QString outputFiles() const;
  virtual QStringList genericArguments(const Compile::Config &config) const;
};

//-----------------------------------------------------------------------------
class AssembleStandaloneFile : public Process
{
Q_OBJECT
protected:
  virtual QString outputFiles() const;
  virtual QStringList genericArguments(const Compile::Config &config) const;
};

//-----------------------------------------------------------------------------
class AssembleProjectFile : public Process
{
Q_OBJECT
protected:
  virtual QString outputFiles() const;
  virtual QStringList genericArguments(const Compile::Config &config) const;
};

//-----------------------------------------------------------------------------
class LinkProject : public Process
{
Q_OBJECT
protected:
  virtual QString outputFiles() const;
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual QString objectExtension() const { return "obj"; }
};

} // namespace

#endif
