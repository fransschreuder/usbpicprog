/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef JAL_GENERATOR_H
#define JAL_GENERATOR_H

#include "tools/base/tool_group.h"

namespace JAL
{

class SourceGenerator : public Tool::SourceGenerator
{
public:
  virtual SourceLine::List configLines(PURL::ToolType type, const Device::Memory &memory, bool &ok) const;
  virtual SourceLine::List sourceFileContent(PURL::ToolType type, const Device::Data &data, bool &ok) const;
  virtual SourceLine::List includeLines(PURL::ToolType type, const Device::Data &data) const;
};

} // namespace

#endif
