/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef TOOL_LIST_H
#define TOOL_LIST_H

#include "common/common/lister.h"
#include "tools/base/tool_group.h"

namespace Tool
{

class Lister : public ::Group::Lister<Group>
{
public:
  Lister();
};

extern const Lister &lister();

} // namespace

#endif
