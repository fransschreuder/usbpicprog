/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "gputils_template_generator_check.h"

//----------------------------------------------------------------------------
bool GPUtilsTemplateGeneratorCheck::init(const Device::Data &data)
{
  if ( !GPUtilsGeneratorCheck::init(data) ) return false;
  bool ok;
  SourceLine::List lines = _generator->templateSourceFile(PURL::ToolType::Assembler, data, ok);
  _source = SourceLine::text(PURL::SourceFamily::Asm, lines, 2);
  if (!ok) TEST_FAILED(QString("Incomplete template generator for %1").arg(data.name()))
  return true;
}

//----------------------------------------------------------------------------
TEST_MAIN(GPUtilsTemplateGeneratorCheck)
