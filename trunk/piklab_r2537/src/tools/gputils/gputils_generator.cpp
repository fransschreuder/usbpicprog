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
#include "devices/list/device_list.h"

//----------------------------------------------------------------------------
QValueList<const Device::Data *> GPUtils::getSupportedDevices(const QString &s)
{
  QStringList devices = QStringList::split(' ', s.simplifyWhiteSpace().upper());
  QValueList<const Device::Data *> list;
  for (uint i=0; i<devices.count(); i++) {
    QString name = devices[i];
    if ( devices[i].startsWith("P1") ) name = name.mid(1);
    const Device::Data *data = Device::lister().data(name);
    if (data) list.append(data);
  }
  return list;
}

//----------------------------------------------------------------------------
SourceLine::List GPUtils::SourceGenerator::configLines(PURL::ToolType, const Device::Memory &memory, bool &ok) const
{
  return GPUtils::generateConfigLines(static_cast<const Pic::Memory &>(memory), ok);
}

SourceLine::List GPUtils::SourceGenerator::includeLines(PURL::ToolType, const Device::Data &data) const
{
  return GPUtils::includeLines(data);
}

SourceLine::List GPUtils::SourceGenerator::sourceFileContent(PURL::ToolType, const Device::Data &data, bool &ok) const
{
  ok = true;
  SourceLine::List lines;
  lines.appendSeparator();
  const Pic::Data &pdata = static_cast<const Pic::Data &>(data);
  const Pic::RegistersData &rdata = static_cast<const Pic::RegistersData &>(*data.registersData());
  switch (pdata.architecture().type()) {
  case Pic::Architecture::P10X:
    lines.appendTitle(i18n("relocatable code"));
    lines.appendNotIndentedCode("PROG CODE");
    lines.appendNotIndentedCode("start");
    lines.appendIndentedCode(QString::null, "<< " + i18n("insert code") + " >>");
    lines.appendIndentedCode("goto    $", i18n("loop forever"));
    lines.appendEmpty();
    lines.appendNotIndentedCode("END");
    break;
  case Pic::Architecture::P16X: {
    lines.appendTitle(i18n("Variables declaration"));
    bool needPCLATH = ( pdata.nbWords(Pic::MemoryRangeType::Code)>0x400 );
    uint first;
    bool allShared;
    bool hasShared = rdata.hasSharedGprs(first, allShared);
    if ( hasShared && !allShared ) lines.appendNotIndentedCode("INT_VAR UDATA_SHR");
    else {
      first = rdata.firstGprIndex();
      lines.appendNotIndentedCode("INT_VAR UDATA " + toHexLabel(first, rdata.nbCharsAddress()));
    }
    lines.appendNotIndentedCode("w_saved      RES 1", i18n("variable used for context saving"));
    lines.appendNotIndentedCode("status_saved RES 1", i18n("variable used for context saving"));
    first += 2;
    if (needPCLATH) {
      lines.appendNotIndentedCode("pclath_saved RES 1", i18n("variable used for context saving"));
      first++;
    }
    lines.appendEmpty();
    lines.appendNotIndentedCode("var1         RES 1", i18n("example variable"));
    if ( !hasShared ) {
      for (uint i=1; i<rdata.nbBanks; i++) {
        uint address = first + i*rdata.nbBytesPerBank();
        lines.appendNotIndentedCode(QString("INT_VAR%1 UDATA ").arg(i) + toHexLabel(address, rdata.nbCharsAddress()), i18n("variables used for context saving"));
        lines.appendNotIndentedCode(QString("w_saved%1     RES 1").arg(i), i18n("variable used for context saving"));
      }
    }
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
  case Pic::Architecture::P17C:
    lines.appendTitle(i18n("Variables declaration"));
    lines.appendNotIndentedCode("INT_VAR UDATA " + toHexLabel(rdata.accessBankSplit, rdata.nbCharsAddress()));
    lines.appendNotIndentedCode("wred_saved   RES 1", i18n("variable used for context saving (for low-priority interrupts only)"));
    lines.appendNotIndentedCode("alusta_saved RES 1", i18n("variable used for context saving (for low-priority interrupts only)"));
    lines.appendNotIndentedCode("bsr_saved    RES 1", i18n("variable used for context saving (for low-priority interrupts only)"));
    lines.appendNotIndentedCode("pclath_saved RES 1", i18n("variable used for context saving (for low-priority interrupts only)"));
    lines.appendEmpty();
    lines.appendNotIndentedCode("var1 RES 1", i18n("example variable"));
    lines.appendEmpty();
    lines.appendSeparator();
    lines.appendTitle(i18n("Macros"));
    lines.appendNotIndentedCode("PUSH MACRO", i18n("for saving context"));
    lines.appendIndentedCode("movpf   WREG,wred_saved");
    lines.appendIndentedCode("movpf   ALUSTA,alusta_saved");
    lines.appendIndentedCode("movpf   BSR,bsr_saved");
    lines.appendIndentedCode("movpf   PCLATH,pclath_saved");
    lines.appendIndentedCode("ENDM");
    lines.appendEmpty();
    lines.appendNotIndentedCode("POP MACRO", i18n("for restoringing context"));
    lines.appendIndentedCode("movfp   pclath_saved,PCLATH");
    lines.appendIndentedCode("movfp   bsr_saved,BSR");
    lines.appendIndentedCode("movfp   alusta_saved,ALUSTA");
    lines.appendIndentedCode("movfp   wred_saved,WREG");
    lines.appendIndentedCode("ENDM");
    lines.appendSeparator();
    lines.appendTitle(i18n("reset vector"));
    lines.appendNotIndentedCode("STARTUP CODE 0x0000");
    lines.appendIndentedCode("nop", i18n("needed for ICD2 debugging"));
    lines.appendIndentedCode("nop", i18n("needed for ICD2 debugging"));
    lines.appendIndentedCode("goto    start", i18n("go to start of main code"));
    lines.appendEmpty();
    lines.appendNotIndentedCode("INT_PIN_VECTOR CODE 0x0008");
    lines.appendIndentedCode("PUSH");
    lines.appendIndentedCode("goto    int_pin_isr", i18n("go to start of int pin interrupt code"));
    lines.appendEmpty();
    lines.appendNotIndentedCode("TIMER0_VECTOR CODE 0x0010");
    lines.appendIndentedCode("PUSH");
    lines.appendIndentedCode("goto    timer0_isr", i18n("go to start of timer0 interrupt code"));
    lines.appendEmpty();
    lines.appendNotIndentedCode("T0CKI_VECTOR CODE 0x00018");
    lines.appendIndentedCode("PUSH");
    lines.appendIndentedCode("goto    t0cki_isr", i18n("go to start of t0cki interrupt code"));
    lines.appendEmpty();
    lines.appendNotIndentedCode("PERIPHERAL_VECTOR CODE 0x0020");
    lines.appendIndentedCode("PUSH");
    lines.appendIndentedCode("goto    peripheral_isr", i18n("go to start of peripheral interrupt code"));
    lines.appendEmpty();
    lines.appendNotIndentedCode("start:");
    lines.appendIndentedCode(QString::null, "<< " + i18n("insert main code") + " >>");
    lines.appendIndentedCode("goto    $", i18n("loop forever"));
    lines.appendEmpty();
    lines.appendTitle(i18n("INT pin interrupt service routine"));
    lines.appendNotIndentedCode("int_pin_isr:");
    lines.appendIndentedCode(QString::null, "<< " + i18n("insert INT pin interrupt code") + " >>");
    lines.appendIndentedCode("POP");
    lines.appendIndentedCode("retfie");
    lines.appendEmpty();
    lines.appendTitle(i18n("TIMER0 interrupt service routine"));
    lines.appendNotIndentedCode("timer0_isr:");
    lines.appendIndentedCode(QString::null, "<< " + i18n("insert TIMER0 interrupt code") + " >>");
    lines.appendIndentedCode("POP");
    lines.appendIndentedCode("retfie");
    lines.appendEmpty();
    lines.appendTitle(i18n("T0CKI interrupt service routine"));
    lines.appendNotIndentedCode("t0cki_isr:");
    lines.appendIndentedCode(QString::null, "<< " + i18n("insert T0CKI interrupt code") + " >>");
    lines.appendIndentedCode("POP");
    lines.appendIndentedCode("retfie");
    lines.appendEmpty();
    lines.appendTitle(i18n("peripheral interrupt service routine"));
    lines.appendNotIndentedCode("peripheral_isr:");
    lines.appendIndentedCode(QString::null, "<< " + i18n("insert peripheral interrupt code") + " >>");
    lines.appendIndentedCode("POP");
    lines.appendIndentedCode("retfie");
    lines.appendEmpty();
    lines.appendNotIndentedCode("END");
    break;
  case Pic::Architecture::P18C:
  case Pic::Architecture::P18F:
  case Pic::Architecture::P18J: // ??
    lines.appendTitle(i18n("Variables declaration"));
    lines.appendNotIndentedCode("INT_VAR UDATA " + toHexLabel(rdata.accessBankSplit, rdata.nbCharsAddress()));
    lines.appendNotIndentedCode("w_saved      RES 1", i18n("variable used for context saving (for low-priority interrupts only)"));
    lines.appendNotIndentedCode("status_saved RES 1", i18n("variable used for context saving (for low-priority interrupts only)"));
    lines.appendNotIndentedCode("bsr_saved    RES 1", i18n("variable used for context saving (for low-priority interrupts only)"));
    lines.appendEmpty();
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
    lines.appendIndentedCode("bra    high_interrupt", i18n("go to start of high priority interrupt code"));
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
  case Pic::Architecture::P24F:
  case Pic::Architecture::P24H:
  case Pic::Architecture::P30F:
  case Pic::Architecture::P33F: ok = false; break;
  case Pic::Architecture::Nb_Types: Q_ASSERT(false); break;
  }
  return lines;
}
