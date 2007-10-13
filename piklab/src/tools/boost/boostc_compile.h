/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BOOSTC_COMPILE_H
#define BOOSTC_COMPILE_H

#include "tools/list/compile.h"

namespace Boost
{
//----------------------------------------------------------------------------
class Process : public Compile::Process
{
Q_OBJECT
protected:
  virtual QString deviceName() const { return "PIC" + _data.device; }
  virtual void logStderrLine(const QString &line);
};

//----------------------------------------------------------------------------
class CompileFile : public Process
{
Q_OBJECT
protected:
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual QString outputFiles() const;
};

//----------------------------------------------------------------------------
class Link : public Process
{
Q_OBJECT
protected:
  virtual QString objectExtension() const { return "obj"; }
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual QString outputFiles() const;
};

} // namespace

#endif
