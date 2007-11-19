/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef C18_COMPILE_H
#define C18_COMPILE_H

#include "tools/list/compile_process.h"

namespace C18
{
//-----------------------------------------------------------------------------
class Process : public Compile::Process
{
Q_OBJECT
protected:
  virtual QString deviceName() const { return _data.device.lower(); }
};

//-----------------------------------------------------------------------------
class CompileFile : public Process
{
Q_OBJECT
protected:
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual QString outputFiles() const;
  virtual void logStderrLine(const QString &line);
};

//-----------------------------------------------------------------------------
class Link : public Process
{
Q_OBJECT
protected:
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual QString outputFiles() const;
  virtual void logStderrLine(const QString &line);
};

} // namespace

#endif
