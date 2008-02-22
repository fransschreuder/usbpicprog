/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ABOUT_H
#define ABOUT_H

#include "global.h"

namespace Piklab
{
//-----------------------------------------------------------------------------
class OptionList : public QValueList<KCmdLineOptions>
{
public:
  OptionList() : _options(0) {}
  OptionList(const KCmdLineOptions *options);
  virtual ~OptionList() { delete[] _options; }
  const KCmdLineOptions *ptr() const;

private:
  mutable KCmdLineOptions *_options;
};

//---------------------------------------------------------------------------
enum UrlType { Homepage = 0, Support, BugReport, Nb_UrlTypes };
extern const char * const URLS[Nb_UrlTypes];
extern void init(KAboutData *about, int argc, char **argv, bool gui, const KCmdLineOptions *options);

//---------------------------------------------------------------------------
class AboutData : public KAboutData
{
public:
  AboutData(const char *executable, const char *name, const char *description);
};

} // namespace

#endif
