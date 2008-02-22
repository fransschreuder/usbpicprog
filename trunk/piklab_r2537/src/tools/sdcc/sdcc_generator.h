/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef SDCC_GENERATOR_H
#define SDCC_GENERATOR_H

#include "coff/base/disassembler.h"

namespace SDCC
{
//----------------------------------------------------------------------------
enum Family { P14 = 0, P16, Nb_Families };
struct FamilyData {
  const char *name;
};
extern const FamilyData FAMILY_DATA[Nb_Families];
extern Family family(const QString &device);
extern QString toDeviceName(const QString &device);
extern QValueList<const Device::Data *> getSupportedDevices(const QString &s);

//----------------------------------------------------------------------------
class SourceGenerator : public Tool::SourceGenerator
{
public:
  virtual SourceLine::List configLines(PURL::ToolType type, const Device::Memory &memory, bool &ok) const;
  virtual SourceLine::List sourceFileContent(PURL::ToolType type, const Device::Data &data, bool &ok) const;
  virtual SourceLine::List includeLines(PURL::ToolType type, const Device::Data &data) const;

private:
  struct P16CNameData {
    const char *cname, *sdccName;
  };
  static const P16CNameData P16_CNAME_DATA[];
  static void transformCName(QString &cname, const Pic::Data &data);

};

} // namespace

#endif
