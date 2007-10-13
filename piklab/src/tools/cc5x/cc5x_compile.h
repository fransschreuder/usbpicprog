/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CC5X_COMPILE_H
#define CC5X_COMPILE_H

#include "tools/list/compile.h"

namespace CC5X
{

class CompileFile : public Compile::Process
{
Q_OBJECT
protected:
  virtual QString deviceName() const { return QString::null; }
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual void logStderrLine(const QString &line);
  virtual QString outputFiles() const;
};

} // namespace

#endif
