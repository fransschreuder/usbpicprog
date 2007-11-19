/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PFILE_EXT_H
#define PFILE_EXT_H

#include "common/global/pfile.h"

namespace PURL
{

class TempFile : public FileBase
{
public:
  TempFile(Log::Generic &log, const QString &extension = QString::null);
  ~TempFile() { close(); }
  Url url() const;
  bool close();
  bool openForWrite();
};

} // namespace

#endif
