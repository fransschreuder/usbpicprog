/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef JAL_COMPILE_H
#define JAL_COMPILE_H

#include "tools/list/compile.h"

namespace JAL
{
//-----------------------------------------------------------------------------
class CompileFile : public Compile::Process
{
Q_OBJECT
protected:
  virtual QString deviceName() const { return _data.device; }
  virtual void logStderrLine(const QString &line);
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual QString outputFiles() const;
};

} // namespace

#endif
