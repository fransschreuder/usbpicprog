/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "gputils_generator.h"

#include "devices/pic/pic/pic_memory.h"
#include "devices/pic/base/pic_register.h"

SourceLine::List GPUtils::SourceGenerator::configLines(PURL::ToolType, const Device::Memory &memory, bool &ok) const
{
  return GPUtils::generateConfigLines(static_cast<const Pic::Memory &>(memory), ok);
}

SourceLine::List GPUtils::SourceGenerator::includeLines(PURL::ToolType, const Device::Data &data) const
{
  SourceLine::List lines;
  lines.appendIndentedCode("#include <" + toDeviceName(data.name()) + ".inc>");
  return lines;
}

SourceLine::List GPUtils::SourceGenerator::sourceFileContent(PURL::ToolType, const Device::Data &data, bool &) const
{
  SourceLine::List lines;
  lines.appendSeparator();
  const Pic::Data &pdata = static_cast<const Pic::Data &>(data);
  const Pic::RegistersData &rdata = static_cast<const Pic::RegistersData &>(*data.registersData());
  switch (pdata.architecture()) {
  case Pic::Arch_10X:
    lines.appendTitle(i18n("relocatable code"));
    lines.appendNotIndentedCode("PROG CODE");
    lines.appendNotIndentedCode("start");
    lines.appendIndentedCode(QString::null, "<< " + i18n("insert code") + " >>");
    lines.appendIndentedCode("goto    $", i18n("loop forever"));
    lines.appendEmpty();
    lines.appendNotIndentedCode("END");
    break;
  case Pic::Arch_16X: {
    if ( rdata.nbRegisters()==0 ) return lines;
    lines.appendTitle(i18n("Variables declaration"));
    bool needPCLATH = ( pdata.nbWords(Pic::MemoryCode)>0x400 );
    uint first;
    bool allShared;
    bool hasShared = rdata.hasSharedGprs(first, allShared);
    if (hasShared) {
      if (allShared) lines.appendNotIndentedCode("INT_VAR UDATA_SHR", i18n("variables used for context saving"));
      else lines.appendNotIndentedCode("INT_VAR UDATA " + toHexLabel(first, rdata.nbCharsAddress()), i18n("variables used for context saving"));
    } else {
      first = rdata.firstGprIndex();
      lines.appendNotIndentedCode("INT_VAR UDATA " + toHexLabel(first, rdata.nbCharsAddress()), i18n("variables used for context saving"));
    }
    lines.appendNotIndentedCode("w_saved      RES 1");
    lines.appendNotIndentedCode("status_saved RES 1");
    if (needPCLATH) lines.appendNotIndentedCode("pclath_saved RES 1");
    if ( !hasShared ) {
      for (uint i=1; i<rdata.nbBanks; i++) {
        uint address = first + i*rdata.nbBytesPerBank();
        lines.appendNotIndentedCode(QString("INT_VAR%1 UDATA ").arg(i) + toHexLabel(address, rdata.nbCharsAddress()), i18n("variables used for context saving"));
        lines.appendNotIndentedCode(QString("w_saved%1     RES 1").arg(i));
      }
    }
    lines.appendEmpty();
    if (hasShared) lines.appendNotIndentedCode("VAR_VAR UDATA_SHR");
    else lines.appendNotIndentedCode("VAR_VAR UDATA");
    lines.appendNotIndentedCode("var1         RES 1", i18n("example variable"));
    lines.appendEmpty();
    lines.appendSeparator();
    lines.appendTitle(i18n("reset vector"));
    lines.appendNotIndentedCode("STARTUP CODE 0x000");
    lines.appendIndentedCode("nop", i18n("needed for ICD2 debugging"));
    lines.appendIndentedCode("movlw   high start", i18n("load upper byte of 'start' label"));
    lines.appendIndentedCode("movwf   PCLATH", i18n("initialize PCLATH"));
    lines.appendIndentedCode("goto    start", i18n("go to start of main code"));
    lines.appendEmpty();
    lines.appendTitle(i18n("interrupt vector"));
    lines.appendNotIndentedCode("INT_VECTOR CODE 0x004");
    lines.appendIndentedCode("goto    interrupt", i18n("go to start of interrupt code"));
    lines.appendEmpty();
    lines.appendTitle(i18n("relocatable code"));
    lines.appendNotIndentedCode("PROG CODE");
    lines.appendNotIndentedCode("interrupt");
    lines.appendIndentedCode("movwf   w_saved", i18n("save context"));
    lines.appendIndentedCode("swapf   STATUS,w");
    if ( !hasShared ) lines.appendIndentedCode("clrf    STATUS");
    lines.appendIndentedCode("movwf   status_saved");
    if (needPCLATH) {
      lines.appendIndentedCode("movf    PCLATH,w", i18n("only required if using more than first page"));
      lines.appendIndentedCode("movwf   pclath_saved");
      lines.appendIndentedCode("clrf    PCLATH");
    }
    lines.appendIndentedCode(QString::null, "<< " + i18n("insert interrupt code") + " >>");
    if (needPCLATH) {
      lines.appendIndentedCode("movf    pclath_saved,w", i18n("restore context"));
      lines.appendIndentedCode("movwf   PCLATH");
      lines.appendIndentedCode("swapf   status_saved,w");
    } else lines.appendIndentedCode("swapf   status_saved,w", i18n("restore context"));
    lines.appendIndentedCode("movwf   STATUS");
    lines.appendIndentedCode("swapf   w_saved,f");
    lines.appendIndentedCode("swapf   w_saved,w");
    lines.appendIndentedCode("retfie");
    lines.appendEmpty();
    lines.appendNotIndentedCode("start");
    lines.appendIndentedCode(QString::null, "<< " + i18n("insert main code") + " >>");
    lines.appendIndentedCode("goto    $", i18n("loop forever"));
    lines.appendEmpty();
    lines.appendNotIndentedCode("END");
    break;
  }
  case Pic::Arch_17X:
  case Pic::Arch_18C:
  case Pic::Arch_18F:
  case Pic::Arch_18J: // ??
    lines.appendTitle(i18n("Variables declaration"));
    lines.appendNotIndentedCode("INT_VAR UDATA " + toHexLabel(rdata.accessBankSplit, rdata.nbCharsAddress()), i18n("variables used for context saving (for low-priority interrupts only)"));
    lines.appendNotIndentedCode("w_saved      RES 1");
    lines.appendNotIndentedCode("status_saved RES 1");
    lines.appendNotIndentedCode("bsr_saved    RES 1");
    lines.appendEmpty();
    lines.appendNotIndentedCode("VAR_VAR UDATA");
    lines.appendNotIndentedCode("var1 RES 1", i18n("example variable"));
    lines.appendEmpty();
    lines.appendSeparator();
    lines.appendTitle(i18n("reset vector"));
    lines.appendNotIndentedCode("STARTUP CODE 0x0000");
    lines.appendIndentedCode("nop", i18n("needed for ICD2 debugging"));
    lines.appendIndentedCode("nop", i18n("needed for ICD2 debugging"));
    lines.appendIndentedCode("goto    start", i18n("go to start of main code"));
    lines.appendEmpty();
    lines.appendTitle(i18n("high priority interrupt vector"));
    lines.appendNotIndentedCode("HIGH_INT_VECTOR CODE 0x0008");
    lines.appendIndentedCode("goto    high_interrupt", i18n("go to start of high priority interrupt code"));
    lines.appendEmpty();
    lines.appendTitle(i18n("low priority interrupt vector"));
    lines.appendNotIndentedCode("LOW_INT_VECTOR CODE 0x0018");
    lines.appendIndentedCode("movff   STATUS,status_saved", i18n("save context"));
    lines.appendIndentedCode("movff   WREG,w_saved");
    lines.appendIndentedCode("movff   BSR,bsr_saved");
    lines.appendIndentedCode(QString::null, "<< " + i18n("insert low priority interrupt code") + " >>");
    lines.appendIndentedCode("movff   bsr_saved,BSR", i18n("restore context"));
    lines.appendIndentedCode("movff   w_saved,WREG");
    lines.appendIndentedCode("movff   status_saved,STATUS");
    lines.appendIndentedCode("retfie");
    lines.appendEmpty();
    lines.appendTitle(i18n("high priority interrupt service routine"));
    lines.appendNotIndentedCode("high_interrupt:");
    lines.appendIndentedCode(QString::null, "<< " + i18n("insert high priority interrupt code") + " >>");
    lines.appendIndentedCode("retfie  FAST");
    lines.appendEmpty();
    lines.appendNotIndentedCode("start:");
    lines.appendIndentedCode(QString::null, "<< " + i18n("insert main code") + " >>");
    lines.appendIndentedCode("goto    $", i18n("loop forever"));
    lines.appendEmpty();
    lines.appendNotIndentedCode("END");
    break;
  case Pic::Arch_24F:
  case Pic::Arch_24H:
  case Pic::Arch_30X:
  case Pic::Nb_Architectures: Q_ASSERT(false); break;
  }
  return lines;
}
