/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "libgui/toplevel.h"
#include "common/global/about.h"
#include "libgui/likeback.h"

const KCmdLineOptions OPTIONS[] = {
  { "+[file]", I18N_NOOP("Optional filenames to be opened upon startup."), 0 },
  { 0, 0, 0}
};

int main(int argc, char **argv)
{
  Piklab::AboutData *about = new Piklab::AboutData("piklab", I18N_NOOP("Piklab"), I18N_NOOP( "Graphical development environment for applications based on PIC and dsPIC microcontrollers."));
  Piklab::init(about, argc, argv, true, OPTIONS);
  LikeBack::init(true);
  LikeBack::setServer("piklab.sourceforge.net", "/likeback/send.php");
  LikeBack::setAllowFeatureWishes(true);
  //  LikeBack::setWindowNamesListing(LikeBack:: /*NoListing*/ /*WarnUnnamedWindows*/ AllWindows);
  if ( kapp->isRestored() ) RESTORE(MainWindow)
  else {
    MainWindow *wid = new MainWindow;
    wid->show();
  }
  return kapp->exec();
}
