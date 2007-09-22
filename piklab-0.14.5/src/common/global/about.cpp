/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "about.h"

#if defined(Q_WS_WIN)
#  define SVN_REVISION "windows"
#else
#  include "svn_revision/svn_revision.h"
#endif

//---------------------------------------------------------------------------
const char * const Piklab::URLS[Nb_UrlTypes] = {
  "http://piklab.sourceforge.net",
  "http://piklab.sourceforge.net/wiki/index.php/FAQ",
  "http://sourceforge.net/tracker/?func=add&group_id=138852&atid=743140"
};

//---------------------------------------------------------------------------
Piklab::AboutData::AboutData(const char *executable, const char *name,
			     const char *description)
  : KAboutData(executable, name, VERSION, description, KAboutData::License_GPL,
               "(c) 2005-2007 Nicolas Hadacek\n(c) 2002-2005 Alain Gibaud\n(c) 2003-2004 Stephen Landamore\n(c) 2005 Lorenz Möenlechner and Matthias Kranz\n(c) 2001-2005 Craig Franklin",
               0, URLS[Homepage], "hadacek@kde.org")
{
  addAuthor("Nicolas Hadacek", I18N_NOOP("Author and maintainer."), "hadacek@kde.org");
  addAuthor("Alain Gibaud", I18N_NOOP("Original author of PiKdev."), "alain.gibaud@free.fr");
  addAuthor("Stephen Landamore", I18N_NOOP("LPLAB author (original microchip programmer support)."), "webmaster@landamore.com");
  addAuthor("Craig Franklin", I18N_NOOP("Author of gputils"), "craigfranklin@users.sourceforge.net");
  addAuthor("Sébastien Laoût", I18N_NOOP("Author of likeback"), "slaout@linux62.org");

  addCredit("Brian C. Lane", I18N_NOOP("Original code for direct programming."), 0);
  addCredit("Manwlis \"Manos\" Giannos", I18N_NOOP("Direct programming for PIC18F devices."), "mgiannos2000@yahoo.gr");
  addCredit("Sean A. Walberg", I18N_NOOP("Direct programming for 16F676/630."), "sean@ertw.com");
  addCredit("Mirko Panciri", I18N_NOOP("Support for direct programmers with bidirectionnal buffers."), "mirko.panciri@adept.it");
  addCredit("Keith Baker", I18N_NOOP("Direct programming for 16F73/74/76/77."), "susyzygy@pubnix.org" );
  addCredit("Lorenz Möenlechner and Matthias Kranz", I18N_NOOP("USB support for ICD2 programmer."), "icd2linux@hcilab.org");
  addCredit("Xiaofan Chen", I18N_NOOP("Test of PICkit2 and ICD2 programmer."), "xiaofanc@gmail.com");
  addCredit("Homer Reid", I18N_NOOP("Direct programming for dsPICs is inspired from his program \"dspicprg\"."), "homer@homerreid.com");
  addCredit("Frank Damgaard", I18N_NOOP("Direct programming for 24C EEPROM is inspired from his program \"prog84\"."), "frda@post3.tele.dk");
}

//-----------------------------------------------------------------------------
#if defined(NO_KDE)
QCoreApplication *Piklab::init(KAboutData *about, int argc, char **argv, bool,
			       const KCmdLineOptions *options)
{
#  if defined(Q_OS_WIN)
  qDebug("%s \"win32\": version %s", about->appName(), VERSION);
#  else
  qDebug("%s \"qt-only\": version %s (rev. %s)", about->appName(), VERSION, SVN_REVISION);
#  endif
  KCmdLineArgs::init(argc, argv, about);
  KCmdLineArgs::addCmdLineOptions(options);
  return new QCoreApplication(argc, argv);
}
#else
KApplication *Piklab::init(KAboutData *about, int argc, char **argv, bool gui,
			   const KCmdLineOptions *options)
{
  qDebug("%s: version %s (rev. %s)", about->appName(), VERSION, SVN_REVISION);
  if ( !gui ) KApplication::disableAutoDcopRegistration();
  KCmdLineArgs::init(argc, argv, about);
  KCmdLineArgs::addCmdLineOptions(options);
  KApplication::addCmdLineOptions();
  return new KApplication(gui, gui);
}
#endif
