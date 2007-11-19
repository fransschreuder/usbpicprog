/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "purl_base.h"

#include "common/global/global.h"
#include <qfileinfo.h>

#include "data/xpms/project.xpm"
#include "data/xpms/sourcefile.xpm"
#include "data/xpms/includefile.xpm"
#include "data/xpms/objectfile.xpm"

const PURL::ToolType::Data PURL::ToolType::DATA[Nb_Types] = {
  { "assembler", I18N_NOOP("Assembler") },
  { "compiler",  I18N_NOOP("Compiler")  }
};

const PURL::SourceFamily::Data PURL::SourceFamily::DATA[Nb_Types] = {
  { ToolType::Assembler, "asm",   I18N_NOOP("Assembler"),      Inc          },
  { ToolType::Compiler,  "c",     I18N_NOOP("C Compiler"),     CHeader      },
  { ToolType::Compiler,  "jal",   I18N_NOOP("JAL Compiler"),   Nb_FileTypes },
  { ToolType::Compiler,  "cpp",   I18N_NOOP("C++ Compiler"),   CHeader      },
  { ToolType::Compiler,  "basic", I18N_NOOP("Basic Compiler"), Nb_FileTypes }
};

const PURL::FileType::Data PURL::FileType::DATA[Nb_Types] = {
  { "AsmGPAsm",    Source, Editable, SourceFamily::Asm, I18N_NOOP("Assembler File"),            { "asm", "src", "pic", 0 }, sourcefile_xpm,  0,               "XPicAsm"   },
  { "AsmPIC30",    Source, Editable, SourceFamily::Asm, I18N_NOOP("Assembler File for PIC30"),  { "s", 0 },                 sourcefile_xpm,  0,               "XPicAsm"   },
  { "AsmPICC",     Source, Editable, SourceFamily::Asm, I18N_NOOP("Assembler File for PICC"),   { "as", 0 },                sourcefile_xpm,  0,               "XPicAsm"   },
  { "Inc",         Header, Editable, SourceFamily::Asm, I18N_NOOP("Include File"),        { "inc", 0 },               includefile_xpm, 0,               "XPicAsm"   },
  { "CSource",     Source, Editable, SourceFamily::C,   I18N_NOOP("C Source File"),       { "c", 0 },                 0,               "text/x-csrc",   "C"         },
  { "CppSource",   Source, Editable, SourceFamily::C,   I18N_NOOP("C++ Source File"),     { "cpp", "cxx", 0 },        0,               "text/x-c++src", "C++"       },
  { "CHeader",     Header, Editable, SourceFamily::C,   I18N_NOOP("C Header File"),       { "h", 0 },                 0,               "text/x-chdr",   "C"         },
  { "JalSource",   Source, Editable, SourceFamily::JAL, I18N_NOOP("JAL File"),            { "jal", 0},                sourcefile_xpm,  0,               "XPicJal"   },
  { "BasicSource", Source, Editable, SourceFamily::Basic, I18N_NOOP("Basic Source File"), { "bas", 0 },               sourcefile_xpm,  0,               "FreeBASIC" },

  { "Object",      LinkerObject, ReadOnly, SourceFamily::Nb_Types, I18N_NOOP("Object File"),         { "o", "obj", 0 },          objectfile_xpm,  0,               0         },
  { "Library",     LinkerObject, ReadOnly, SourceFamily::Nb_Types, I18N_NOOP("Library File"),        { "a", "lib", 0 },          objectfile_xpm,  0,               0         },

  { "Lkr",         LinkerScript, Editable, SourceFamily::Nb_Types, I18N_NOOP("Linker Script"),       { "lkr", 0 },               includefile_xpm, 0,               0         },
  { "Gld",         LinkerScript, Editable, SourceFamily::Nb_Types, I18N_NOOP("Linker Script for PIC30"), { "gld", 0 },               includefile_xpm, 0,               0         },

  { "Hex",         Nb_FileGroups, Editable,            SourceFamily::Nb_Types, I18N_NOOP("Hex File"),            { "hex", 0 },               0,               "text/x-hex",    0         },
  { "Elf",         Nb_FileGroups, ReadOnly,            SourceFamily::Nb_Types, I18N_NOOP("Elf File"),            { "elf", 0 },               0,               0,               0         },
  { "Project",     Nb_FileGroups, NoProperty,          SourceFamily::Nb_Types, I18N_NOOP("Project File"),        { "piklab", 0 },            project_xpm,     0,               0         },
  { "Lst",         Nb_FileGroups, Editable | ReadOnly, SourceFamily::Nb_Types, I18N_NOOP("List File"),           { "lst", 0 },               0,               "text/plain",    0         },
  { "Map",         Nb_FileGroups, Editable | ReadOnly, SourceFamily::Nb_Types, I18N_NOOP("Map File"),            { "map", 0 },               0,               "text/plain",    0         },
  { "Cod",         Nb_FileGroups, ReadOnly,            SourceFamily::Nb_Types, I18N_NOOP("COD File"),            { "cod", 0 },               0,               0,               0         },
  { "Coff",        Nb_FileGroups, Editable | ReadOnly, SourceFamily::Nb_Types, I18N_NOOP("COFF File"),           { "cof", 0 },               0,               "application/x-object", "XCoffDisasm" },

  { "",            Nb_FileGroups, NoProperty,          SourceFamily::Nb_Types, I18N_NOOP("Unknown File"),        { 0 },                      0,               0,               0         },
  { "",            Nb_FileGroups, NoProperty,          SourceFamily::Nb_Types, I18N_NOOP("Pikdev Project File"), { "pikprj", 0 },            0,               0,               0         }
};

QString PURL::addExtension(const QString &filename, FileType type)
{
  QFileInfo info(filename);
  if ( !info.extension().isEmpty() ) return filename;
  return filename + '.' + extension(type);
}

QString PURL::extension(FileType type)
{
  return type.data().extensions[0];
}

QString PURL::extensions(FileType type)
{
  Q_ASSERT( type!=PURL::Nb_FileTypes );
  QString s;
  for (uint i=0; type.data().extensions[i]; i++) {
    if ( i!=0 ) s += ' ';
    s += QString("*.") + type.data().extensions[i];
  }
  return s;
}

QString PURL::filter(FileType type)
{
  //if ( hasMimetype(type) ) return DATA[type].mimetype; // #### we cannot mix mimetype and regular filters in KFileDialog...
  QString s = extensions(type);
  return s + ' ' + s.upper() + '|' + i18n(type.label()) + " (" + s + ")";
}

QString PURL::extensions(FileGroup group)
{
  QString e;
  FOR_EACH(FileType, type) {
    if ( type.data().group!=group ) continue;
    if ( type!=FileType::Type(0) ) e += ' ';
    QString s = extensions(type);
    e += s + ' ' + s.upper();
  }
  return e;
}

QString PURL::sourceFilter(FilterType type)
{
  QString f = extensions(Source) + ' ' + extensions(Header) + '|' + i18n("All Source Files");
  if ( type==CompleteFilter) {
    FOR_EACH(FileType, type) {
      if ( !(type.data().properties & (Source | Header)) ) continue;
      f += '\n' + filter(type);
    }
  }
  return f;
}

QString PURL::objectFilter(FilterType type)
{
  QString f = extensions(Object) + ' ' + extensions(Library) + '|' + i18n("All Object Files");
  if ( type==CompleteFilter ) {
    f += '\n' + filter(Object);
    f += '\n' + filter(Library);
  }
  return f;
}

QString PURL::projectFilter(FilterType type)
{
  QString f = extensions(Project) + ' ' + extensions(PikdevProject) + '|' + i18n("Project Files");
  if ( type==CompleteFilter ) {
    f += '\n' + filter(Project);
    f += '\n' + filter(PikdevProject);
  }
  return f;
}
