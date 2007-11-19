/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CCSC_COMPILE_H
#define CCSC_COMPILE_H

#include "tools/list/compile_process.h"

namespace CCSC
{

class CompileFile : public Compile::Process
{
Q_OBJECT
protected:
  virtual QString deviceName() const { return QString::null; }
  virtual QString familyName() const;
  virtual QStringList genericArguments(const Compile::Config &config) const;
  virtual void logStderrLine(const QString &line);
  virtual QString outputFiles() const;
  virtual PURL::Url url(PURL::FileType type = PURL::Nb_FileTypes, uint i = 0) const;

protected slots:
  virtual void done(int code);

private:
  void parseLine(const QString &line);
};

} // namespace

#endif
