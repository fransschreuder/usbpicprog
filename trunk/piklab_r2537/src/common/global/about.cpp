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

//-----------------------------------------------------------------------------
Piklab::OptionList::OptionList(const KCmdLineOptions *options)
  : _options(0)
{
  for (uint i=0; options[i].name; i++) append(options[i]);
}

const KCmdLineOptions *Piklab::OptionList::ptr() const
{
  delete[] _options;
  _options = new KCmdLineOptions[count()+1];
  for (uint i=0; i<uint(count()); i++) {
    _options[i] = *at(i);
    Q_ASSERT( _options[i].name );
  }
  _options[count()].name = 0;
  return _options;
}

//-----------------------------------------------------------------------------
void Piklab::init(KAboutData *about, int argc, char **argv, bool gui, const KCmdLineOptions *options)
{
  Q_ASSERT(about);
#if defined(NO_KDE)
#  if defined(Q_OS_WIN)
  printf("%s \"win32\": version %s\n", about->appName(), VERSION);
#  else
  printf("%s \"qt-only\": version %s (rev. %s)\n", about->appName(), VERSION, SVN_REVISION);
#  endif
  Q_UNUSED(gui);
  Q_ASSERT( !gui );
#else
  printf("%s: version %s (rev. %s)\n", about->appName(), VERSION, SVN_REVISION);
  if ( !gui ) KApplication::disableAutoDcopRegistration();
#endif
  KCmdLineArgs::init(argc, argv, about);
  KCmdLineArgs::addCmdLineOptions(options);
#if defined(NO_KDE)
#  if QT_VERSION<0x040000
  (void)new QApplication(argc, argv, QApplication::Tty);
#  else
  (void)new QCoreApplication(argc, argv);
#  endif
#else
  (void)new KApplication(gui, gui);
#endif
}

//---------------------------------------------------------------------------
Piklab::AboutData::AboutData(const char *executable, const char *name,
			     const char *description)
  : KAboutData(executable, name, VERSION, description, KAboutData::License_GPL,
               "(c) 2005-2007 Nicolas Hadacek\n(c) 2002-2005 Alain Gibaud\n(c) 2003-2004 Stephen Landamore\n(c) 2005 Lorenz Möenlechner and Matthias Kranz\n(c) 2001-2005 Craig Franklin",
               0, URLS[Homepage], URLS[BugReport])
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
