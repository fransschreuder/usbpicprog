/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "gputils_config_generator_check.h"

#include "devices/base/device_group.h"
#include "devices/pic/pic/pic_memory.h"
#include "tools/gputils/gputils_generator.h"
#include "devices/list/device_list.h"

//----------------------------------------------------------------------------
bool GPUtilsConfigGeneratorCheck::init(const Device::Data &data)
{
  if ( !GPUtilsGeneratorCheck::init(data) ) return false;
  _memory2 = static_cast<Pic::Memory *>(data.group().createMemory(data));
  return true;
}

bool GPUtilsConfigGeneratorCheck::execute(const Device::Data &data)
{
  // create configuration
  const Pic::Config &config = static_cast<const Pic::Data &>(data).config();
  for (uint l=0; ; l++) {
    // set config bits
    bool ok = false;
    for (uint i=0; i<config._words.count(); i++) {
      const Pic::Config::Word &cword = config._words[i];
      for (uint k=0; k<cword.masks.count(); k++) {
        const Pic::Config::Mask &cmask = cword.masks[k];
        if ( l<cmask.values.count() ) {
          ok = true;
          if ( !cmask.values[l].name.isEmpty() ) _memory2->setConfigValue(cmask.name, cmask.values[l].name);
        }
      }
    }
    if ( !ok ) break;

    // create source code
    SourceLine::List lines = _generator->includeLines(PURL::ToolType::Assembler, data);
    lines += _generator->configLines(PURL::ToolType::Assembler, *_memory2, ok);
    lines.appendIndentedCode("end");
    _source = SourceLine::text(PURL::SourceFamily::Asm, lines, 2);
    if (!ok) TEST_FAILED("Config lines generation incomplete")

    if ( !GPUtilsGeneratorCheck::execute(data) ) return false;

    // check that config bits are the same
    uint nbChars = static_cast<const Pic::Data &>(data).nbCharsWord(Pic::MemoryRangeType::Config);
    for (uint i=0; i<config._words.count(); i++) {
      const Pic::Config::Word &cword = config._words[i];
      BitValue word1 = _memory1->word(Pic::MemoryRangeType::Config, i);
      BitValue word2 = _memory2->word(Pic::MemoryRangeType::Config, i);
      if ( word1==word2 ) continue;
      for (uint k=0; k<cword.masks.count(); k++) {
        const Pic::Config::Mask &cmask = cword.masks[k];
        if ( cmask.value.isInside(cword.pmask) ) continue;
        if ( word1.maskWith(cmask.value)==word2.maskWith(cmask.value) ) continue;
        TEST_FAILED(QString("Config bits are different in %1 (%2!=%3)").arg(cmask.name).arg(toHexLabel(word1.maskWith(cmask.value), nbChars)).arg(toHexLabel(word2.maskWith(cmask.value), nbChars)))
      }
    }
  }

  TEST_PASSED
}

void GPUtilsConfigGeneratorCheck::cleanup(const Device::Data &)
{
  delete _memory2;
  _memory2 = 0;
}

//----------------------------------------------------------------------------
TEST_MAIN(GPUtilsConfigGeneratorCheck)
