/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// this program generate xml files for kate highlighting
// the original syntax file was created by Alain Gibaud

#include <qfile.h>
#include <qtextstream.h>

const char * const DIRECTIVES[] = {
"__badram", "__config", "__idlocs", "__maxram",
"bankisel", "banksel", "cblock", "code", "cblock", "constant", "da", "data", "db",
"de", "dt", "dw", "endm", "endc", "endw", "equ", "error", "errorlevel", "extern",
"exitm", "expand", "fill", "global", "idata", "list", "local", "macro", "messg",
"noexpand", "nolist", "org", "page", "processor", "pagesel", "radix", "res", "set",
"space", "subtitle", "title", "udata", "udata_acs", "udata_ovr", "udata_shr",
"variable", "end", 0
};
const char * const CONDITIONNALS[] = {
"if", "else", "ifdef", "ifndef", "endif", "while", "include", "endw", "{", "}", 0
};
const char * const GPASM_MACROS[] = {
"addcf", "b", "bc", "bz", "bnc", "bnz", "clrc", "clrz", "setc", "setz", "movfw",
"negf", "skpc", "skpz", "skpnc", "skpnz", "subcf", "tstf", 0
};
const char * const CPIK_MACROS[] = {
"IBZ", "IBNZ", "IBRA", "ICALL", 0
};
const char * const SFRS[] = { // #### should be extracted from dev files...
"TOSU", "TOSH", "TOSL", "STKPTR", "PCLATU", "PCLATH", "PCL", "TBLPTRU", "TBLPTRH",
"TBLPTRL", "TABLAT", "PRODH", "PRODL", "INTCON", "INTCON2", "INTCON3", "INDF0",
"POSTINC0", "POSTDEC0", "PREINC0", "PLUSW0", "FSR0H", "FSR0L", "WREG", "INDF1",
"POSTINC1", "POSTDEC1", "PREINC1", "PLUSW1", "FSR1H", "FSR1L", "BSR", "INDF2",
"POSTINC2", "POSTDEC2", "PREINC2", "PLUSW2", "FSR2H", "FSR2L", "STATUS", "TMR0H",
"TMR0L", "T0CON", "OSCCON", "LVDCON", "WDTCON", "RCON", "TMR1H", "TMR1L","T1CON",
"TMR2", "PR2", "T2CON", "SSPBUF", "SSPADD", "SSPSTAT", "SSPCON1", "SSPCON2", "ADRESH",
"ADRESL", "ADCON0", "ADCON1", "ADCON2", "CCPR1H", "CCPR1L", "CCP1CON", "CCPR2H",
"CCPR2L", "CCP2CON", "PWM1CON", "ECCPAS", "CVRCON", "CMCON", "TMR3H", "TMR3L",
"T3CON", "SPBRG", "RCREG", "TXREG", "TXSTA", "RCSTA", "EEADR", "EEDATA", "EECON2",
"EECON1", "IPR2", "PIR2", "PIE2", "IPR1", "PIR1", "PIE1", "OSCTUNE", "TRISE",
"TRISD", "TRISC", "TRISB", "TRISA", "LATE", "LATD", "LATC", "LATB", "LATA",
"PORTE", "PORTD", "PORTC", "PORTB", "PORTA", "TMR0", "TMR1", "OPTION_REG", 0
};
const char * const INSTRUCTIONS[] = {
"addlw", "addwf", "addwfc", "andlw",  "andwf", "bcf", "bc", "bn", "bnc", "bnn", "bnov",
"bnz", "bz", "bov", "bra", "bsf", "btfsc", "btg", "btfss", "call", "clrf", "clrw",
"clrwdt", "comf", "cpfseq", "cpfsgt", "cpfslt", "daw", "decf", "decfsz", "dcfsnz",
"goto", "incf", "incfsz", "infsnz", "iorlw", "iorwf", "lfsr", "movf", "movff",
"movlb", "movlw", "movwf", "mullw", "mulwf", "negf", "nop", "option", "pop", "push",
"rcall", "reset", "retfie", "retlw", "rlcf", "rlncf", "rrcf", "rrncf", "return", "rlf",
"rrf", "setf", "sleep", "sublw", "subfwb", "subwf", "subwfb", "swapf", "tstfsz",
"tris", "tblrd", "tblwt", "xorlw", "xorwf", 0
};

QTextStream *initFile(QFile &file)
{
  if ( !file.open(IO_WriteOnly) ) qFatal("Cannot create \"%s\".", file.name().latin1());
  QTextStream *s = new QTextStream(&file);
  (*s) << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  (*s) << "<!DOCTYPE language SYSTEM \"language.dtd\">" << endl;
  (*s) << "<!--This file is generated. Do not edit.-->" << endl;
  return s;
}

void addList(QTextStream &s, const QString &name, const char * const *items, bool upper)
{
  s << "  <list name=\"" << name << "\">" << endl;
  for (uint i=0; items[i]; i++) {
    s << "    <item>" << items[i] << "</item>";
    if (upper) s << "<item>" << QString(items[i]).upper() << "</item>";
    s << endl;
  };
  s <<"  </list>" << endl;
}

void addCommon(QTextStream &s)
{
  addList(s, "instructions", INSTRUCTIONS, true);
  s << endl;
  s << "  <itemDatas>" << endl;
  s << "    <itemData name=\"Normal Text\"       defStyleNum=\"dsNormal\"/>" << endl;
  s << "    <itemData name=\"Directives\"        defStyleNum=\"dsOthers\"/>" << endl;
  s << "    <itemData name=\"Preprocessor\"      defStyleNum=\"dsNormal\" color=\"#D819D8\" selColor=\"#D819D8\" bold=\"0\" italic=\"0\"/>" << endl;
  s << "    <itemData name=\"Instructions\"      defStyleNum=\"dsNormal\"  color=\"#0060FF\" selColor=\"#8293CE\" bold=\"1\" italic=\"0\"/>" << endl;
  s << "    <itemData name=\"CPIK-macros\"       defStyleNum=\"dsNormal\" color=\"#0045B3\" selcolor=\"#8293CE\" bold=\"1\" italic=\"0\"/>" << endl;
  s << "    <itemData name=\"SFRS\"              defStyleNum=\"dsNormal\" color=\"#2A8A19\" selcolor=\"#000000\" bold=\"1\" italic=\"0\"/>" << endl;
  s << "    <itemData name=\"ModuleMarks\"       defStyleNum=\"dsNormal\"   color=\"#FF20FF\" selColor=\"#8293CE\" bold=\"1\" italic=\"0\"/>" << endl;
  s << "    <itemData name=\"Non Based Numbers\" defStyleNum=\"dsBaseN\"/>" << endl;
  s << "    <itemData name=\"Char\"              defStyleNum=\"dsChar\"/>" << endl;
  s << "    <itemData name=\"String\"            defStyleNum=\"dsString\"/>" << endl;
  s << "    <itemData name=\"Comment\"           defStyleNum=\"dsNormal\" color=\"#FF3118\" selColor=\"#FF3118\" bold=\"0\" italic=\"1\" />" << endl;
  s << "    <itemData name=\"Symbol\"            defStyleNum=\"dsNormal\" color=\"#FF0000\" selColor=\"#083194\" bold=\"0\" italic=\"0\" />" << endl;
  s << "    <itemData name=\"Prep. Lib\"         defStyleNum=\"dsOthers\"/>" << endl;
  s << "    <itemData name=\"Based Numbers\"     defStyleNum=\"dsBaseN\" />" << endl;
  s << "    <itemData name=\"GPASM-macros\"      defStyleNum=\"dsNormal\" color=\"#000000\" selcolor=\"#000000\" bold=\"1\" italic=\"0\"/>" << endl;
  s << "    <itemData name=\"Destination\"       defStyleNum=\"dsNormal\" color=\"#000000\" selcolor=\"#000000\" bold=\"1\" italic=\"0\"/>" << endl;
  s << "  </itemDatas>" << endl;
  s << endl;
  s << "  <contexts>" << endl;
  s << "    <context name=\"normal\" attribute=\"Normal Text\" lineEndContext=\"#stay\">" << endl;
}

void addAsmContexts(QTextStream &s)
{
  s << "      <keyword attribute=\"Directives\"    context=\"#stay\" String=\"directives\"/>" << endl;
  s << "      <keyword attribute=\"Instructions\"  context=\"#stay\" String=\"instructions\"/>" << endl;
  s << "      <keyword attribute=\"CPIK-macros\"   context=\"#stay\" String=\"cpik_macro\"/>" << endl;
  s << "      <keyword attribute=\"SFRS\"          context=\"#stay\" String=\"sfrs\"/>" << endl;
  s << "      <keyword attribute=\"Preprocessor\"  context=\"#stay\" String=\"conditionnal\"/>" << endl;
  s << "      <keyword attribute=\"GPASM-macros\"  context=\"#stay\" String=\"gpasm_macro\"/>" << endl;
  s << endl;
  s << "      <HlCHex  attribute=\"Based Numbers\" context=\"#stay\"/>" << endl;
  s << "      <RegExpr attribute=\"Based Numbers\" context=\"#stay\" String=\"[0-9A-F]+H($|\\s)\" insensitive=\"TRUE\" />" << endl;
  s << "      <RegExpr attribute=\"Based Numbers\" context=\"#stay\" String=\"[0-9]+D($|\\s)\" insensitive=\"TRUE\" />" << endl;
  s << "      <RegExpr attribute=\"Based Numbers\" context=\"#stay\" String=\"[0-7]+O($|\\s)\" insensitive=\"TRUE\" />" << endl;
  s << "      <RegExpr attribute=\"Based Numbers\" context=\"#stay\" String=\"[0-1]+B($|\\s)\" insensitive=\"TRUE\" />" << endl;
  s << "      <RegExpr attribute=\"Non Based Numbers\" context=\"#stay\" String=\"\\.[0-9]+($|\\s)\" />" << endl;
  s << "      <RegExpr attribute=\"Non Based Numbers\" context=\"#stay\" String=\"[0-9]+($|\\s)\" />" << endl;
  s << endl;
  s << "      <RegExpr attribute=\"Char\"              context=\"#stay\" String=\"A'[^']+'\" insensitive=\"TRUE\" />" << endl;
  s << "      <RegExpr attribute=\"Based Numbers\"     context=\"#stay\" String=\"B'[01]+'\" insensitive=\"TRUE\" />" << endl;
  s << "      <RegExpr attribute=\"Based Numbers\"     context=\"#stay\" String=\"H'[0-9A-F]+'\" insensitive=\"TRUE\" />" << endl;
  s << "      <RegExpr attribute=\"Based Numbers\"     context=\"#stay\" String=\"O'[0-7]+'\" insensitive=\"TRUE\" />" << endl;
  s << "      <RegExpr attribute=\"Non Based Numbers\" context=\"#stay\" String=\"D'[0-9]+'\" insensitive=\"TRUE\" />" << endl;
  s << endl;
  s << "      <DetectChar attribute=\"Destination\"  context=\"#stay\"   char=\"$\" />" << endl;
  s << "      <RegExpr    attribute=\"Normal Text\"  context=\"#stay\"   String=\"[A-Za-z_.$][A-Za-z0-9_.$]*\" />" << endl;
  s << "      <HlCChar    attribute=\"Char\"         context=\"#stay\" />" << endl;
  s << "      <DetectChar attribute=\"String\"       context=\"string\"  char=\"&quot;\" />" << endl;
  s << "      <DetectChar attribute=\"Comment\"      context=\"comment\" char=\";\" />" << endl;
  s << "      <AnyChar    attribute=\"Symbol\"       context=\"#stay\"   String=\"[-/*%+=&gt;&lt;&amp;|^!~]\" />" << endl;
  s << "      <RegExpr    attribute=\"Symbol\"       context=\"#stay\"   String=\"(HIGH|LOW)\" />" << endl;
  s << "      <RegExpr    attribute=\"Destination\"  context=\"#stay\"   String=\",\\s*[fw]\" insensitive=\"TRUE\" />" << endl;
  s << "      <RegExpr    attribute=\"Preprocessor\" context=\"preprocessor\" String=\"(#include|#define|#undefine|#v)\" />" << endl;
  s << "    </context>" << endl;
  s << "    <context name=\"preprocessor\" attribute=\"Preprocessor\" lineEndContext=\"#pop\">" << endl;
  s << "      <DetectChar attribute=\"Comment\" context=\"comment\" char=\";\"/>" << endl;
  s << "    </context>" << endl;
  s << "    <context name=\"string\" attribute=\"String\" lineEndContext=\"#pop\">" << endl;
  s << "      <LineContinue attribute=\"String\" context=\"#stay\"/>" << endl;
  s << "      <HlCStringChar attribute=\"Char\" context=\"#stay\"/>" << endl;
  s << "      <DetectChar attribute=\"String\" context=\"#pop\" char=\"&quot;\"/>" << endl;
  s << "    </context>" << endl;
  s << "    <context name=\"comment\" attribute=\"Comment\" lineEndContext=\"#pop\">" << endl;
  s << "      <RegExpr attribute=\"Instructions\" context=\"#stay\"" << endl;
  s << "        String=\"(FIXME|TODO|fixme|todo|INPUT|OUTPUT|PARAMETERS|AUTHOR|EMAIL)\" />" << endl;
  s << "      <RegExpr attribute=\"ModuleMarks\" context=\"#stay\" beginRegion=\"moduleASM\"" << endl;
  s << "        String=\"(&lt;[+]+[A-Za-z_#]+[A-Za-z0-9_#.]*&gt;)\"  />" << endl;
  s << "      <RegExpr attribute=\"ModuleMarks\" context=\"#stay\"  endRegion=\"moduleASM\"" << endl;
  s << "        String=\"(&lt;[-]+&gt;)\"  />" << endl;
  s << "      <RegExpr attribute=\"ModuleMarks\" context=\"#stay\"" << endl;
  s << "        String=\"(&lt;[=]+[- 0-9]+&gt;)\"  />" << endl;
  s << "      <RegExpr attribute=\"ModuleMarks\" context=\"#stay\"" << endl;
  s << "        String=\"(&lt;[?]+[A-Za-z_#]+[A-Za-z0-9_#]*&gt;)\"  />" << endl;
  s << "    </context>" << endl;
}

void endFile(QTextStream &s)
{
  s << "  </contexts>" << endl;
  s << "</highlighting>" << endl;
  s << endl;
  s << "<general>" << endl;
  s << "  <comments>" << endl;
  s << "    <comment name=\"singleLine\" start=\";\" />" << endl;
  s << "  </comments>" << endl;
  s << "  <keywords casesensitive=\"1\" />" << endl;
  s << "</general>" << endl;
  s << endl;
  s << "</language>" << endl;
}

int main(int, char **)
{
  QFile afile("asm-pic.xml");
  QTextStream *s = initFile(afile);
  (*s) << "<language name=\"XPicAsm\" version=\"1.5\" kateversion=\"2.0\" section=\"Sources\"" << endl;
  (*s) << "  extensions=\"*.src;*.SRC;*.asm;*.ASM;*.slb;*.SLB;*.pic;*.PIC;*.inc;*.INC;*.pic12;*.pic14;*.PIC12;*.PIC14;*.pic16;*.PIC16\"" << endl;
  (*s) << "  mimetype=\"text/x-PicSrc;text/x-PicHdr\">" << endl;
  (*s) << endl;
  (*s) << "<highlighting>" << endl;
  addList(*s, "directives", DIRECTIVES, true);
  addList(*s, "conditionnal", CONDITIONNALS, false);
  addList(*s, "gpasm_macro", GPASM_MACROS, false);
  addList(*s, "cpik_macro", CPIK_MACROS, false);
  addList(*s, "sfrs", SFRS, false);
  addCommon(*s);
  addAsmContexts(*s);
  endFile(*s);

  QFile cfile("coff-pic.xml");
  s = initFile(cfile);
  (*s) << "<language name=\"XCoffDisasm\" version=\"1.2\" kateversion=\"2.0\" section=\"Sources\">" << endl;
  (*s) << endl;
  (*s) << "<highlighting>" << endl;
  addCommon(*s);
  (*s) << "      <RegExpr attribute=\"Preprocessor\" context=\"cfile\" String=\"^\\-\\-\\-\\s.+\\.(c|h)\\s\\-+\"/>" << endl;
  (*s) << "      <RegExpr attribute=\"Preprocessor\" context=\"asmfile\" String=\"^\\-\\-\\-\\s.+\\s\\-+\"/>" << endl;
  (*s) << "    </context>" << endl;
  (*s) << "    <context name=\"asmfile\" attribute=\"Normal Text\">" << endl;
  (*s) << "      <RegExpr attribute=\"Preprocessor\" context=\"asmline\" String=\"\\d+\\s*:\"/>" << endl;
  (*s) << "      <RegExpr attribute=\"Based Numbers\" context=\"#stay\" String=\"\\s0\\s\" />" << endl;
  (*s) << "      <RegExpr attribute=\"Normal Text\" context=\"#stay\" String=\"^[0-9A-F]+\" />" << endl;
  (*s) << "      <keyword attribute=\"Instructions\" context=\"#stay\" String=\"instructions\"/>" << endl;
  (*s) << "      <HlCHex  attribute=\"Based Numbers\" context=\"#stay\"/>" << endl;
  (*s) << "      <RegExpr attribute=\"Destination\"  context=\"#stay\"   String=\",\\s*[fw]\" insensitive=\"TRUE\" />" << endl;
  (*s) << "      <RegExpr attribute=\"Preprocessor\" context=\"#pop\" String=\"^\\-\\-\\-\\s.+\\-\" lookAhead=\"true\"/>" << endl;
  (*s) << "    </context>" << endl;
  (*s) << "    <context name=\"asmline\" attribute=\"Normal Text\" lineEndContext=\"#pop\">" << endl;
  (*s) << "      <IncludeRules context=\"##XPicAsm\" includeAttrib=\"true\" />" << endl;
  (*s) << "    </context>" << endl;
  (*s) << "    <context name=\"cfile\" attribute=\"Normal Text\">" << endl;
  (*s) << "      <RegExpr attribute=\"Preprocessor\" context=\"cline\" String=\"\\d+\\s*:\"/>" << endl;
  (*s) << "      <RegExpr attribute=\"Based Numbers\" context=\"#stay\" String=\"\\s0\\s\" />" << endl;
  (*s) << "      <RegExpr attribute=\"Normal Text\" context=\"#stay\" String=\"^[0-9A-F]+\" />" << endl;
  (*s) << "      <keyword attribute=\"Instructions\" context=\"#stay\" String=\"instructions\"/>" << endl;
  (*s) << "      <HlCHex  attribute=\"Based Numbers\" context=\"#stay\"/>" << endl;
  (*s) << "      <RegExpr attribute=\"Destination\"  context=\"#stay\"   String=\",\\s*[fw]\" insensitive=\"TRUE\" />" << endl;
  (*s) << "      <RegExpr attribute=\"Preprocessor\" context=\"#pop\" String=\"^\\-\\-\\-\\s.+\\-\" lookAhead=\"true\"/>" << endl;
  (*s) << "    </context>" << endl;
  (*s) << "    <context name=\"cline\" attribute=\"Normal Text\" lineEndContext=\"#pop\">" << endl;
  (*s) << "      <IncludeRules context=\"##XCoffDissC\" includeAttrib=\"true\" />" << endl;
  (*s) << "    </context>" << endl;
  endFile(*s);
}
