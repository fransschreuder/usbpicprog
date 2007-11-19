/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MPC_COMPILE_H
#define MPC_COMPILE_H

#include "tools/list/compile_process.h"

namespace MPC
{

class CompileFile : public Compile::Process
{
Q_OBJECT
protected:
  virtual QString deviceName() const { return QString::null; }
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual void logStderrLine(const QString &line);
  virtual QString outputFiles() const;

protected slots:
  virtual void done(int code);

private:
  void parseLine(const QString &line);
};

} // namespace

#endif
