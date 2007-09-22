/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "generic_tool.h"

#include <qdatetime.h>
#include <klocale.h>
#include <kprogress.h>

#include "devices/list/device_list.h"
#include "common/global/process.h"
#include "common/gui/misc_gui.h"
#include "tools/list/compile.h"
#include "tools/list/compile_config.h"

//----------------------------------------------------------------------------
const Tool::CategoryData Tool::CATEGORY_DATA[Nb_Categories] = {
  { "compiler",     I18N_NOOP("Compiler:"),     I18N_NOOP("Compiler")     },
  { "assembler",    I18N_NOOP("Assembler:"),    I18N_NOOP("Assembler")    },
  { "linker",       I18N_NOOP("Linker:"),       I18N_NOOP("Linker")       },
  { "bin_to_hex",   I18N_NOOP("Bin to Hex:"),   I18N_NOOP("Bin to Hex")   },
  { "preprocessor", I18N_NOOP("Preprocessor:"), I18N_NOOP("Preprocessor") }
};

const Tool::ExecutableTypeData Tool::EXECUTABLE_TYPE_DATA[Nb_ExecutableTypes] = {
  { I18N_NOOP("Unix")    },
  { I18N_NOOP("Windows") }
};

const Compile::DirectoryTypeData Compile::DIRECTORY_TYPE_DATA[Nb_DirectoryTypes] = {
  { "executable",    I18N_NOOP("Executable directory")    },
  { "include",       I18N_NOOP("Header directory")        },
  { "linker_script", I18N_NOOP("Linker Script Directory") },
  { "library",       I18N_NOOP("Library Directory")       },
  { "source",        I18N_NOOP("Source Directory")        }
};

//-----------------------------------------------------------------------------
PURL::Directory Tool::Base::executableDirectory() const
{
  const Compile::Config *config = _group->createConfig(0);
  PURL::Directory dir = config->directory(Compile::ExecutableDirectory);
  delete config;
  return dir;
}

::Process::LineOutput *Tool::Base::checkExecutableProcess(const PURL::Directory &dir, bool withWine) const
{
  ::Process::LineOutput *process = new ::Process::LineOutput;
  process->setup(dir.path(false) + baseExecutable(withWine), checkExecutableOptions(withWine), withWine);
  PURL::Directory wdir = checkExecutableWorkingDirectory();
  if ( !wdir.isEmpty() ) process->setWorkingDirectory(wdir.path(false));
  return process;
}
