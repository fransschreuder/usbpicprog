/***************************************************************************
 *   Copyright (C) 2006-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic30_generator.h"

#include "devices/pic/pic/pic_memory.h"

SourceLine::List PIC30::SourceGenerator::configLines(PURL::ToolType type, const Device::Memory &memory, bool &ok) const
{
  const Pic::Memory &pmemory = static_cast<const Pic::Memory &>(memory);
  const Pic::Data &data = pmemory.device();
  const Pic::Config &config = data.config();
  SourceLine::List lines;
  for (uint i=0; i<data.nbWords(Pic::MemoryRangeType::Config); i++) {
    const Pic::Config::Word &cword = config._words[i];
    QStringList cnames = SourceLine::configNames(Pic::ConfigNameType::Default, pmemory, i, ok);
    if ( cnames.isEmpty() ) continue;
    QString code;
    if ( type==PURL::ToolType::Assembler ) code += "config __" + cword.name + ", ";
    else code += "_" + cword.name + "(";
    code += cnames.join(" & ");
    if ( type==PURL::ToolType::Compiler ) code += ");";
    lines.appendNotIndentedCode(code);
  }
  return lines;
}

SourceLine::List PIC30::SourceGenerator::includeLines(PURL::ToolType type, const Device::Data &data) const
{
  SourceLine::List lines;
  if ( type==PURL::ToolType::Assembler ) lines.appendIndentedCode(".include \"p" + data.name().lower() + ".inc\"");
  else lines.appendNotIndentedCode("#include <p" + data.name().lower() + ".h>");
  return lines;
}

SourceLine::List PIC30::SourceGenerator::sourceFileContent(PURL::ToolType type, const Device::Data &, bool &ok) const
{
  SourceLine::List lines;
  if ( type==PURL::ToolType::Assembler ) {
    lines.appendTitle(i18n("Global declarations"));
    lines.appendIndentedCode(".global _wreg_init");
    lines.appendIndentedCode(".global __reset", i18n("label for code start"));
    lines.appendIndentedCode(".global __T1Interrrupt", i18n("declare Timer1 ISR"));
    lines.appendEmpty();
    lines.appendSeparator();
    lines.appendTitle(i18n("Constants in program space"));
    lines.appendIndentedCode(".section .constbuffer, code");
    lines.appendIndentedCode(".palign 2");
    lines.appendNotIndentedCode("const1:");
    lines.appendIndentedCode(".hword 0x0001, 0x0002, 0X0003");
    lines.appendEmpty();
    lines.appendTitle(i18n("Unitialized variables in X-space"));
    lines.appendIndentedCode(".section .xbss, bss, xmemory");
    lines.appendNotIndentedCode("x_var1: .space 4", i18n("allocating 4 bytes"));
    lines.appendEmpty();
    lines.appendTitle(i18n("Unitialized variables in Y-space"));
    lines.appendIndentedCode(".section .ybss, bss, ymemory");
    lines.appendNotIndentedCode("y_var1: .space 2", i18n("allocating 2 bytes"));
    lines.appendEmpty();
    lines.appendTitle(i18n("Unitialized variables in near data memory"));
    lines.appendIndentedCode(".section .nbss, bss, near");
    lines.appendNotIndentedCode("var1: .space 4", i18n("allocating 4 bytes"));
    lines.appendEmpty();
    lines.appendSeparator();
    lines.appendTitle("Code section");
    lines.appendNotIndentedCode(".text");
    lines.appendNotIndentedCode("__reset:");
    lines.appendIndentedCode("MOV #__SP_init, W15", i18n("initialize stack pointer"));
    lines.appendIndentedCode("MOV #__SPLIM_init, W0", i18n("initialize stack pointer limit register"));
    lines.appendIndentedCode("MOV W0, SPLIM");
    lines.appendIndentedCode("NOP", i18n("nop after SPLIM initialization"));
    lines.appendIndentedCode("CALL _wreg_init", i18n("call _wreg_init subroutine"));
    lines.appendEmpty();
    lines.appendIndentedCode(QString::null, "<<" + i18n("insert code") + ">>");
    lines.appendEmpty();
    lines.appendNotIndentedCode("done:");
    lines.appendIndentedCode("BRA done", i18n("loop forever"));
    lines.appendEmpty();
    lines.appendSeparator();
    lines.appendTitle(i18n("Subroutine to initialize W registers to 0x0000"));
    lines.appendNotIndentedCode("_wreg_init:");
    lines.appendIndentedCode("CLR W0");
    lines.appendIndentedCode("MOV W0, W14");
    lines.appendIndentedCode("REPEAT #12");
    lines.appendIndentedCode("MOV W0, [++W14]");
    lines.appendIndentedCode("CLR W14");
    lines.appendIndentedCode("RETURN");
    lines.appendEmpty();
    lines.appendSeparator();
    lines.appendTitle(i18n("Timer1 interrupt service routine"));
    lines.appendNotIndentedCode("__T1Interrupt:");
    lines.appendIndentedCode("PUSH.D W4", i18n("example of context saving (push W4 and W5)"));
    lines.appendIndentedCode(QString::null, "<<" + i18n("insert interrupt code") + ">>");
    lines.appendIndentedCode("BCLR IFS0, #T1IF", i18n("clear Timer1 interrupt flag status bit"));
    lines.appendIndentedCode("POP.D W4", i18n("restore context from stack"));
    lines.appendIndentedCode("RETFIE");
    lines.appendEmpty();
    lines.appendSeparator();
    lines.appendTitle(i18n("End of all code sections"));
    lines.appendNotIndentedCode(".end");
  } else {
    // #### TODO
    ok = false;
  }
  return lines;
}
