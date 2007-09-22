/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "tool_list.h"

#include "tools/gputils/gputils.h"
#include "tools/sdcc/sdcc.h"
#include "tools/pic30/pic30.h"
#include "tools/picc/picc.h"
#include "tools/jal/jal.h"
#include "tools/c18/c18.h"
#include "tools/ccsc/ccsc.h"
#include "tools/jalv2/jalv2.h"
#include "tools/boost/boostc.h"
#include "tools/boost/boostcpp.h"
#include "tools/boost/boostbasic.h"
#include "tools/mpc/mpc.h"
#include "tools/cc5x/cc5x.h"

Tool::Lister::Lister()
{
  addGroup(new GPUtils::Group, 0);
  addGroup(new SDCC::Group, 0);
  addGroup(new PIC30::Group, 0);
  addGroup(new PICC::PICCLiteGroup, 0);
  addGroup(new PICC::PICCGroup, 0);
  addGroup(new PICC::PICC18Group, 0);
  addGroup(new JAL::Group, 0);
  addGroup(new C18::Group, 0);
  addGroup(new CCSC::Group, 0);
  addGroup(new JALV2::Group, 0);
  addGroup(new Boost::GroupC16, 0);
  addGroup(new Boost::GroupC18, 0);
  addGroup(new Boost::GroupCpp16, 0);
  addGroup(new Boost::GroupCpp18, 0);
  addGroup(new Boost::GroupBasic16, 0);
  addGroup(new Boost::GroupBasic18, 0);
  addGroup(new MPC::Group, 0);
  addGroup(new CC5X::Group, 0);
}

namespace Tool
{
  Lister *_lister = 0;
}

const Tool::Lister &Tool::lister()
{
  if ( _lister==0 ) _lister = new Lister;
  return *_lister;
}
