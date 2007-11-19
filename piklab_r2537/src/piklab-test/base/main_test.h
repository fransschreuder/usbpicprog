/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef MAIN_TEST_H
#define MAIN_TEST_H

#include "common/global/about.h"

//----------------------------------------------------------------------------
#define TEST_SKIPPED_RETURN { skipped(); return false; }
#define TEST_FAILED_RETURN(message) { failed(message, __FILE__, __LINE__); return false; }
#define TEST_FAILED(message) { failed(message, __FILE__, __LINE__); }
#define TEST_PASSED { printf("."); fflush(stdout); passed(); }
#define TEST_MAIN(Type)  \
  int main(int argc, char **argv) \
  { \
  Type *check = new Type; \
  Piklab::OptionList opt = check->optionList(); \
  Piklab::init(new Piklab::AboutData("test", 0, 0), argc, argv, false, opt.ptr()); \
  check->runTest(); \
  return 0; \
  }

//----------------------------------------------------------------------------
class Test
{
public:
  Test();
  virtual Piklab::OptionList optionList() const { return Piklab::OptionList(); }
  virtual void runTest();

protected:
  KCmdLineArgs *_args;
  QString       _message;

  void passed() { _nbPassed++; }
  void failed(const QString &message, const char *file, int line);
  void skipped() { _nbSkipped++; }
  virtual bool execute() = 0; // returns false if failed or skipped
  virtual void checkArguments() {}

private:
  uint _nbPassed, _nbFailed, _nbSkipped;
};

#endif
