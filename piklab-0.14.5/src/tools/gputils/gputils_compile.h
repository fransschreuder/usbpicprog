/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GPUTILS_COMPILE_H
#define GPUTILS_COMPILE_H

#include "tools/list/compile.h"

namespace GPUtils
{
//-----------------------------------------------------------------------------
class Process : public Compile::Process
{
Q_OBJECT
private:
  virtual QString deviceName() const;
  virtual bool hasLinkerScript() const { return ( _data.linkType==Compile::Icd2Linking || Compile::Process::hasLinkerScript() ); }
};

//-----------------------------------------------------------------------------
class AssembleFile : public Process
{
Q_OBJECT
private:
  virtual void logStderrLine(const QString &line);
};

//-----------------------------------------------------------------------------
class AssembleStandaloneFile : public AssembleFile
{
Q_OBJECT
private:
  virtual QString outputFiles() const;
  virtual QStringList genericArguments(const Compile::Config &config) const;
};

//-----------------------------------------------------------------------------
class AssembleProjectFile : public AssembleFile
{
Q_OBJECT
private:
  virtual QString outputFiles() const;
  virtual QStringList genericArguments(const Compile::Config &config) const;
};

//-----------------------------------------------------------------------------
class LinkProject : public Process
{
Q_OBJECT
private:
  virtual QString outputFiles() const;
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual void logStderrLine(const QString &line) { _manager->log(filterType(line), line, QString::null, 0); }
};

} // namespace

#endif
