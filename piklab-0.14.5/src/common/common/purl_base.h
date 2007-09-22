/***************************************************************************
 *   Copyright (C) 2005-2006 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PURL_BASE_H
#define PURL_BASE_H

#include "common/global/global.h"

//----------------------------------------------------------------------------
namespace PURL
{
enum FileType { AsmGPAsm = 0, AsmPIC30, AsmPICC, Inc, CSource, CppSource, CHeader, JalSource, BasicSource,
                Object, Library, Lkr, Gld, Hex, Elf, Project, Lst, Map, Cod, Coff,
                Unknown, PikdevProject, Nb_FileTypes };
enum ToolType { Assembler = 0, Compiler, Nb_ToolTypes };
extern const char * const TOOL_TYPE_LABELS[Nb_ToolTypes];
enum SourceFamily { Asm = 0, C, JAL, Cpp, Basic, Nb_SourceFamilies };
struct SourceFamilyData {
  ToolType toolType;
  const char *label;
  FileType headerType;
};
extern const SourceFamilyData SOURCE_FAMILY_DATA[Nb_SourceFamilies];
enum FileGroup { Source = 0, Header, LinkerScript, LinkerObject, Nb_FileGroups };
enum FileProperty { NoProperty = 0, Editable = 1, ReadOnly = 2 };
Q_DECLARE_FLAGS(FileProperties, FileProperty)
Q_DECLARE_OPERATORS_FOR_FLAGS(FileProperties)
struct Data {
  const char *key;
  FileGroup group;
  FileProperties properties;
  SourceFamily sourceFamily;
  const char *label;
  const char *extensions[10];
  const char **xpm_icon;
  const char *mimetype;
  const char *highlightModeName;
};
extern const Data DATA[Nb_FileTypes];
// add correct extension if filename has no extension
extern QString addExtension(const QString &filename, FileType type);
extern QString extension(FileType type);
extern QString extensions(FileType type);
extern QString filter(FileType type);
enum FilterType { SimpleFilter, CompleteFilter };
extern QString sourceFilter(FilterType type);
extern QString objectFilter(FilterType type);
extern QString projectFilter(FilterType type);
extern QString extensions(FileGroup group);

} // namespace

#endif
