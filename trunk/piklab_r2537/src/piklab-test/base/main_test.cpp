/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "main_test.h"

//----------------------------------------------------------------------------
Test::Test()
  : _nbPassed(0), _nbFailed(0), _nbSkipped(0)
{}

void Test::runTest()
{
  _args = KCmdLineArgs::parsedArgs();
  checkArguments();
  fflush(stdout);
  execute();
  printf("\n");
  printf("RESULTS: %i PASSED / %i FAILED / %i SKIPPED \n", _nbPassed, _nbFailed, _nbSkipped);
}

void Test::failed(const QString &message, const char *file, int line)
{
  _nbFailed++;
  printf("\n");
  if ( !_message.isEmpty() ) printf("[%s]", _message.latin1());
  printf("FAILED in \"%s\" line #%i: %s\n", file, line, message.latin1());
}
