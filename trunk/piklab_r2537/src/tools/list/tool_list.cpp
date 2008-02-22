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
#include "tools/gputils/gui/gputils_ui.h"
#include "tools/sdcc/sdcc.h"
#include "tools/sdcc/gui/sdcc_ui.h"
#include "tools/pic30/pic30.h"
#include "tools/pic30/gui/pic30_ui.h"
#include "tools/picc/picc.h"
#include "tools/picc/gui/picc_ui.h"
#include "tools/jal/jal.h"
#include "tools/jal/gui/jal_ui.h"
#include "tools/c18/c18.h"
#include "tools/c18/gui/c18_ui.h"
#include "tools/ccsc/ccsc.h"
#include "tools/ccsc/gui/ccsc_ui.h"
#include "tools/jalv2/jalv2.h"
#include "tools/jalv2/gui/jalv2_ui.h"
#include "tools/boost/boostc.h"
#include "tools/boost/boostcpp.h"
#include "tools/boost/boostbasic.h"
#include "tools/boost/gui/boost_ui.h"
#include "tools/mpc/mpc.h"
#include "tools/mpc/gui/mpc_ui.h"
#include "tools/cc5x/cc5x.h"
#include "tools/cc5x/gui/cc5x_ui.h"
#include "tools/custom/custom.h"

Tool::Lister::Lister()
{
  addGroup(new GPUtils::Group, new GPUtils::GroupUI);
  addGroup(new SDCC::Group, new SDCC::GroupUI);
  addGroup(new PIC30::Group, new PIC30::GroupUI);
  addGroup(new PICC::PICCLiteGroup, new PICC::GroupUI);
  addGroup(new PICC::PICCGroup, new PICC::GroupUI);
  addGroup(new PICC::PICC18Group, new PICC::GroupUI);
  addGroup(new JAL::Group, new JAL::GroupUI);
  addGroup(new C18::Group, new C18::GroupUI);
  addGroup(new CCSC::Group, new CCSC::GroupUI);
  addGroup(new JALV2::Group, new JALV2::GroupUI);
  addGroup(new Boost::GroupC16, new Boost::GroupUI);
  addGroup(new Boost::GroupC18, new Boost::GroupUI);
  addGroup(new Boost::GroupCpp16, new Boost::GroupUI);
  addGroup(new Boost::GroupCpp18, new Boost::GroupUI);
  addGroup(new Boost::GroupBasic16, new Boost::GroupUI);
  addGroup(new Boost::GroupBasic18, new Boost::GroupUI);
  addGroup(new MPC::Group, new MPC::GroupUI);
  addGroup(new CC5X::Group, new CC5X::GroupUI);
  addGroup(new CustomTool::Group, 0);
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
