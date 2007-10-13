/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pic_config.h"

#include <qregexp.h>

const Pic::Config::Data Pic::Config::DATA[] = {
  { { "CP",    I18N_NOOP("Code code-protection")         }, MemoryRange, { { 0, 0 } } },
  { { "CPD",   I18N_NOOP("Data code-protection")         }, MemoryRange, { { 0, 0 } } },
  { { "CPC",   I18N_NOOP("Calibration code-protection")  }, MemoryRange, { { 0, 0 } } },
  { { "CPB",   I18N_NOOP("Boot code-protection")         }, MemoryRange, { { 0, 0 } } },

  { { "WRT",   I18N_NOOP("Code write-protection")           }, MemoryRange, { { 0, 0 } } },
  { { "WRTD",  I18N_NOOP("Data write-protection")           }, MemoryRange, { { 0, 0 } } },
  { { "WRTB",  I18N_NOOP("Boot write-protection")           }, MemoryRange, { { 0, 0 } } },
  { { "WRTC",  I18N_NOOP("Configuration write-protection")  }, MemoryRange, { { 0, 0 } } },

  { { "EBTR",  I18N_NOOP("Table read-protection")      }, MemoryRange, { { 0, 0 } } },
  { { "EBTRB", I18N_NOOP("Boot table read-protection") }, MemoryRange, { { 0, 0 } } },

  { { "WDT",  I18N_NOOP("Watchdog timer") }, Toggle, { { 0, 0 } } },
  { { "MCLRE", I18N_NOOP("Master clear reset"), }, Fixed, {
    { "External", I18N_NOOP("External"),                   },
    { "Internal", I18N_NOOP("Disabled (connected to Vdd)") }, { 0, 0 } } },
  { { "PWRTE", I18N_NOOP("Power-up timer") }, Toggle, { { 0, 0 } } },

  { { "FOSC",  I18N_NOOP("Oscillator") }, Fixed,  {
    { "EXTRC", I18N_NOOP("External RC oscillator")                   },
    { "EXTRC_CLKOUT", I18N_NOOP("External RC oscillator with CLKOUT") },
    { "EXTRC_IO", I18N_NOOP("External RC oscillator (no CLKOUT)")   },
    { "INTRC", I18N_NOOP("Internal oscillator")                      },
    { "INTRC_CLKOUT", I18N_NOOP("Internal oscillator with CLKOUT")   },
    { "INTRC_IO", I18N_NOOP("Internal oscillator (no CLKOUT)")       },
    { "XT", I18N_NOOP("Crystal/resonator")                          },
    { "XTPLL", I18N_NOOP("Crystal/resonator, PLL enabled") },
    { "LP", I18N_NOOP("Low power crystal")                           },
    { "EC", I18N_NOOP("External clock")                              },
    { "EC_CLKOUT", I18N_NOOP("External clock with CLKOUT")           },
    { "EC_IO", I18N_NOOP("External clock (no CLKOUT)")               },
    { "ECPLL_CLKOUT", I18N_NOOP("External clock with CLKOUT, PLL enabled")           },
    { "ECPLL_IO", I18N_NOOP("External clock (no CLKOUT), PLL enabled")               },
    { "E4_CLKOUT", I18N_NOOP("External clock with 4x PLL and with CLKOUT") },
    { "E4_IO", I18N_NOOP("External clock with 4x PLL (no CLKOUT)")   },
    { "E4S_IO", I18N_NOOP("External clock with software controlled 4x PLL (no CLKOUT)") },
    { "ER", I18N_NOOP("External resistor")                           },
    { "ER_CLKOUT", I18N_NOOP("External resistor with CLKOUT")        },
    { "ER_IO", I18N_NOOP("External resistor (no CLKOUT)")            },
    { "HS", I18N_NOOP("High speed crystal/resonator")                },
    { "HSPLL", I18N_NOOP("High speed crystal/resonator, PLL enabled") },
    { "H4", I18N_NOOP("High speed crystal/resonator with 4x PLL")    },
    { "H4S", I18N_NOOP("High speed crystal/resonator with software controlled 4x PLL") },
    { "INTXT", I18N_NOOP("Internal oscillator, XT used by USB") },
    { "INTHS", I18N_NOOP("Internal oscillator, HS used by USB") },
    { 0, 0 } } },

  { { "BG",    I18N_NOOP("Bandgap voltage calibration") }, Fixed, {
    { "Lowest",   I18N_NOOP("Lowest")   },
    { "Mid/Low",  I18N_NOOP("Mid/Low")  },
    { "Mid/High", I18N_NOOP("Mid/High") },
    { "Highest",  I18N_NOOP("Highest")  }, { 0, 0 } } },
  { { "TRIM", I18N_NOOP("Internal Trim") }, Fixed, {
    { "00", I18N_NOOP("00") }, { "01", I18N_NOOP("01") },
    { "10", I18N_NOOP("10") }, { "11", I18N_NOOP("11") }, { 0, 0 } } },
  { { "BODEN", I18N_NOOP("Brown-out detect")       }, Toggle, {
    { "On_run",   I18N_NOOP("Enabled in run - Disabled in sleep") },
    { "Software", I18N_NOOP("SBODEN controls BOD function")       }, { 0, 0 } } },
  { { "FCMEN", I18N_NOOP("Fail-safe clock monitor")      }, Toggle, { { 0, 0 } } },
  { { "IESO",  I18N_NOOP("Internal-external switchover") }, Toggle, { { 0, 0 } } },
  { { "WUREN",  I18N_NOOP("Wake-up reset")               }, Toggle, { { 0, 0 } } },
  { { "DEBUG", I18N_NOOP("In-circuit debugger")          }, Toggle, { { 0, 0 } } },
  { { "MPEEN", I18N_NOOP("Memory parity error")          }, Toggle, { { 0, 0 } } },
  { { "BORV",  I18N_NOOP("Brown-out reset voltage") }, ValueDouble, {
    { "0", I18N_NOOP("Undefined")  }, { 0, 0 } } },
  { { "LVP",   I18N_NOOP("Low voltage programming")   }, Toggle, { { 0, 0 } } },
  { { "CCP2MX", I18N_NOOP("CCP2 multiplex")           }, Pin, { { 0, 0 } } },
  { { "CCP1MX", I18N_NOOP("CCP1 multiplex")           }, Pin, { { 0, 0 } } },
  { { "BORSEN", I18N_NOOP("Brown-out reset software") }, Toggle, { { 0, 0 } } },
  { { "WDTPS", I18N_NOOP("WDT post-scaler") }, Ratio, {
    { "Disabled", I18N_NOOP("Disabled") }, { 0, 0 } } },
  { { "PM",    I18N_NOOP("Processor mode") }, Fixed, {
    { "Extended microcontroller", I18N_NOOP("Extended microcontroller")            },
    { "Microcontroller", I18N_NOOP("Microcontroller")                              },
    { "Microprocessor", I18N_NOOP("Microprocessor")                                },
    { "Code-protected microcontroller", I18N_NOOP("Code protected microcontroller") },
    { "Microprocessor with boot", I18N_NOOP("Microprocessor with boot block")       },
    { 0, 0 } } },

  { { "OSCSEN", I18N_NOOP("Oscillator system clock switch") }, Toggle, { { 0, 0 } } },
  { { "STVREN", I18N_NOOP("Stack full/underflow reset") }, Toggle, { { 0, 0 } } },
  { { "BW",    I18N_NOOP("External bus data width (in bits)") }, ValueUInt, { { 0, 0 } } },
  { { "PBADEN", I18N_NOOP("PORTB A/D")              }, Fixed, {
      { "digital", I18N_NOOP("Digital") },
      { "analog",  I18N_NOOP("Analog")  }, { 0, 0 } } },
  { { "WINEN", I18N_NOOP("Watchdog timer window")   }, Toggle, { { 0, 0 } } },
  { { "HPOL", I18N_NOOP("Odd PWM output polarity")  }, Fixed, {
    { "high", I18N_NOOP("Active high")  },
    { "low",  I18N_NOOP("Active low")   }, { 0, 0 } } },
  { { "LPOL", I18N_NOOP("Even PWM output polarity") }, Fixed, {
    { "high", I18N_NOOP("Active high")  },
    { "low",  I18N_NOOP("Active low")   }, { 0, 0 } } },
  { { "PWMPIN", I18N_NOOP("PWM output pin reset state") }, Toggle, { { 0, 0 } } },
  { { "T1OSCMX", I18N_NOOP("Timer1 oscillator mode") }, Fixed, {
    { "Legacy",    I18N_NOOP("Standard operation")      },
    { "Low Power", I18N_NOOP("Low power in sleep mode") },
    { "RA6",       I18N_NOOP("T1OSO/T1CKI on RA6") },
    { "RB2",       I18N_NOOP("T1OSO/T1CKI on RB2") }, { 0, 0 } } },
  { { "EXCLKMX", I18N_NOOP("TMR0/T5CKI external clock mux") }, Pin, { { 0, 0 } } },
  { { "FLTAMX", I18N_NOOP("FLTA mux")               }, Pin, { { 0, 0 } } },
  { { "PWM4MX", I18N_NOOP("PWM4 mux")               }, Pin, { { 0, 0 } } },
  { { "SSPMX", I18N_NOOP("SSP I/O mux (SCK/SLC, SDA/SDI, SD0)") }, Pins, { { 0, 0 } } },
  { { "LPT1OSC", I18N_NOOP("Low-power timer1 oscillator") }, Toggle, { { 0, 0 } } },
  { { "XINST", I18N_NOOP("Extended instruction set") }, Toggle, { { 0, 0 } } },
  { { "BBSIZ", I18N_NOOP("Boot block size") }, ValueUInt, { { 0, 0 } } },
  { { "ICPORT", I18N_NOOP("Dedicated in-circuit port") }, Toggle, { { 0, 0 } } },
  { { "VREGEN", I18N_NOOP("USB voltage regulator")  }, Toggle, { { 0, 0 } } },
  { { "WAIT", I18N_NOOP("External bus data wait")   }, Toggle, { { 0, 0 } } },
  { { "ABW", I18N_NOOP("Address bus width (in bits)") }, ValueUInt, { { 0, 0 } } },
  { { "ECCPMX", I18N_NOOP("ECCP mux") }, Fixed, {
    { "RE6-RE3", I18N_NOOP("PWM multiplexed onto RE6 and RE3") },
    { "RH7-RH4", I18N_NOOP("PWM multiplexed onto RH7 and RH4") },
    { "RE6-RE5", I18N_NOOP("PWM multiplexed onto RE6 and RE5") },
    { "RH7-RH6", I18N_NOOP("PWM multiplexed onto RH7 and RH6") }, { 0, 0 } } },

  { { "FCKSM", I18N_NOOP("Clock switching mode") }, Fixed, {
    { "Switching off, monitor off", I18N_NOOP("Switching off, monitor off") },
    { "Switching on, monitor off",  I18N_NOOP("Switching on, monitor off")  },
    { "Switching on, monitor on",   I18N_NOOP("Switching on, monitor on")   }, { 0, 0 } } },
  { { "FOS", I18N_NOOP("Oscillator source") }, Fixed, {
    { "INTRC_F",  I18N_NOOP("Internal fast RC")      },
    { "INTRC_LP", I18N_NOOP("Internal low-power RC") },
    { "PRIM",     I18N_NOOP("Primary")               },
    { "TMR1",     I18N_NOOP("Timer1")                }, { 0, 0 } } },
  { { "FPR", I18N_NOOP("Primary oscillator mode") }, Fixed, {
    { "XTL",          I18N_NOOP("Low-power/low-frequency crystal") },
    { "HS",           I18N_NOOP("High speed crystal") },
    { "XT",           I18N_NOOP("XT Crystal") },
    { "XT4",           I18N_NOOP("XT Crystal with 4x PLL") },
    { "XT8",           I18N_NOOP("XT Crystal with 8x PLL") },
    { "XT16",          I18N_NOOP("XT Crystal with 16x PLL") },
    { "EC_CLKOUT",    I18N_NOOP("External clock with CLKOUT") },
    { "EC_IO",        I18N_NOOP("External clock (no CLKOUT)") },
    { "EC4",    I18N_NOOP("External clock with 4x PLL") },
    { "EC8",    I18N_NOOP("External clock with 8x PLL") },
    { "EC16",   I18N_NOOP("External clock with 16x PLL") },
    { "FRC8",    I18N_NOOP("Internal fast RC oscillator with 8x PLL") },
    { "EXTRC_CLKOUT", I18N_NOOP("External RC oscillator with CLKOUT") },
    { "EXTRC_IO",     I18N_NOOP("External RC oscillator (no CLKOUT)") }, { 0, 0 } } },
  { { "FOSFPR", I18N_NOOP("Oscillator mode") }, Fixed, {
    { "XTL",          I18N_NOOP("Low-power/low-frequency crystal") },
    { "HS",           I18N_NOOP("High speed crystal") },
    { "XT",           I18N_NOOP("XT Crystal") },
    { "XT4",           I18N_NOOP("XT Crystal with 4x PLL") },
    { "XT8",           I18N_NOOP("XT Crystal with 8x PLL") },
    { "XT16",          I18N_NOOP("XT Crystal with 16x PLL") },
    { "HS2_4",           I18N_NOOP("HS/2 Crystal with 4x PLL") },
    { "HS2_8",           I18N_NOOP("HS/2 Crystal with 8x PLL") },
    { "HS2_16",          I18N_NOOP("HS/2 Crystal with 16x PLL") },
    { "HS3_4",           I18N_NOOP("HS/3 Crystal with 4x PLL") },
    { "HS3_8",           I18N_NOOP("HS/3 Crystal with 8x PLL") },
    { "HS3_16",          I18N_NOOP("HS/3 Crystal with 16x PLL") },
    { "EC_CLKOUT",    I18N_NOOP("External clock with CLKOUT") },
    { "EC_IO",        I18N_NOOP("External clock (no CLKOUT)") },
    { "EC4",    I18N_NOOP("External clock with 4x PLL") },
    { "EC8",    I18N_NOOP("External clock with 8x PLL") },
    { "EC16",   I18N_NOOP("External clock with 16x PLL") },
    { "FRC4",    I18N_NOOP("Internal fast RC oscillator with 4x PLL") },
    { "FRC8",    I18N_NOOP("Internal fast RC oscillator with 8x PLL") },
    { "FRC16",   I18N_NOOP("Internal fast RC oscillator with 16x PLL") },
    { "TMR1",   I18N_NOOP("Low-power 32 kHz oscillator (TMR1 oscillator)") },
    { "INTRC_F", I18N_NOOP("Internal fast RC oscillator (no PLL)") },
    { "INTRC_LP", I18N_NOOP("Internal low-power RC oscillator") },
    { "EXTRC_CLKOUT", I18N_NOOP("External RC oscillator with CLKOUT") },
    { "EXTRC_IO",     I18N_NOOP("External RC oscillator (no CLKOUT)") }, { 0, 0 } } },
  { { "FWPSA", I18N_NOOP("Watchdog timer prescaler A") }, Ratio, { { 0, 0 } } },
  { { "FWPSB", I18N_NOOP("Watchdog timer prescaler B") }, Ratio, { { 0, 0 } } },
  { { "FWDTEN", I18N_NOOP("Watchdog") }, Toggle, {
    { "Software", I18N_NOOP("Software") }, { 0, 0 } } },
  { { "FPWRT", I18N_NOOP("Power-on reset timer value (ms)") }, ValueUInt, {
    { "0", I18N_NOOP("Disabled") }, { 0, 0 } } },
  { { "GCP", I18N_NOOP("General code segment read-protection") }, MemoryRange, { { 0, 0 } } },
  { { "GWRP", I18N_NOOP("General code segment write-protection") }, MemoryRange, { { 0, 0 } } },
  { { "COE", I18N_NOOP("Reset into clip on emulation mode") }, Toggle, { { 0, 0 } } },
  { { "ICS", I18N_NOOP("ICD communication channel") }, Pins, { { 0, 0 } } },

  { { "USBDIV", I18N_NOOP("USB clock (PLL divided by)") }, ValueUInt, {
    { "1", I18N_NOOP("not divided") }, { 0, 0 } } },
  { { "CPUDIV", I18N_NOOP("CPU system clock (divided by)") }, ValueUInt, {
    { "1", I18N_NOOP("not divided") }, { 0, 0 } } },
  { { "PLLDIV", I18N_NOOP("PLL clock (divided by)") }, ValueUInt, {
    { "1", I18N_NOOP("not divided") }, { 0, 0 } } },

  { { "MCPU", I18N_NOOP("Master clear pull-up resistor") }, Toggle, { { 0, 0 } } },
  { { "IOSCFS", I18N_NOOP("Internal oscillator speed") }, Fixed, {
    { "8MHZ", I18N_NOOP("8 MHz") },
    { "4MHZ", I18N_NOOP("4 MHz") }, { 0, 0 } } },

  // 18J specific
  { { "ETHLED", I18N_NOOP("Ethernet LED enable") }, Toggle, { { 0, 0 } } },
  { { "FOSC2", I18N_NOOP("Default system clock select") }, Fixed, {
    { "FOSC1:FOSC0", I18N_NOOP("FOSC1:FOSC0") },
    { "INTRC", I18N_NOOP("INTRC") }, { 0, 0 } } },
  { { "EMB", I18N_NOOP("External memory bus") }, Fixed, {
    { "Disabled", I18N_NOOP("Disabled") },
    { "12BIT", I18N_NOOP("12-bit external bus") },
    { "16BIT", I18N_NOOP("16-bit external bus") },
    { "20BIT", I18N_NOOP("20-bit external bus") } } },
  { { "EASHFT", I18N_NOOP("External address bus shift") }, Toggle, { { 0, 0 } } },

  // 24X specific / 30F1010 / 30F202X
  { { "WRTBS", I18N_NOOP("Boot segment write-protection") }, MemoryRange, { { 0, 0 } } },
  { { "BSSIZ", I18N_NOOP("Boot segment size") }, ValueUInt, { { 0, 0 } } },
  { { "BSSEC", I18N_NOOP("Boot segment security") }, Fixed, {
    { "High Security",     I18N_NOOP("High Security") },
    { "Standard Security", I18N_NOOP("Standard Security") }, { 0, 0 } } },
  { { "EBSSIZ", I18N_NOOP("Boot segment EEPROM size") }, ValueUInt, { { 0, 0 } } },
  { { "RBSSIZ", I18N_NOOP("Boot segment RAM size") }, ValueUInt, { { 0, 0 } } },
  { { "WRTSS", I18N_NOOP("Secure segment write-protection") }, MemoryRange, { { 0, 0 } } },
  { { "SSSIZ", I18N_NOOP("Secure segment size") }, ValueUInt, { { 0, 0 } } },
  { { "SSSEC", I18N_NOOP("Secure segment security") }, Fixed, {
    { "High Security",     I18N_NOOP("High Security") },
    { "Standard Security", I18N_NOOP("Standard Security") }, { 0, 0 } } },
  { { "ESSSIZ", I18N_NOOP("Secure segment EEPROM size") }, ValueUInt, { { 0, 0 } } },
  { { "RSSSIZ", I18N_NOOP("Secure segment RAM size") }, ValueUInt, { { 0, 0 } } },
  { { "WRTGS", I18N_NOOP("General segment write-protection") }, MemoryRange, { { 0, 0 } } },
  { { "GSSEC", I18N_NOOP("General segment security") }, Fixed, {
    { "Off",                I18N_NOOP("Off") },
    { "High Security",     I18N_NOOP("High security") },
    { "Standard Security", I18N_NOOP("Standard security") }, { 0, 0 } } },
  { { "FNOSC", I18N_NOOP("Initial oscillator source") }, Fixed, {
    { "EXTRC_F" ,    I18N_NOOP("Fast RC oscillator") },
    { "INTRC_F",     I18N_NOOP("Internal fast RC oscillator") },
    { "INTRC_F_PLL", I18N_NOOP("Internal fast RC oscillator with PLL") },
    { "PRIM",        I18N_NOOP("Primary oscillator") },
    { "PRIM_PLL",    I18N_NOOP("Primary oscillator with PLL") },
    { "SECOND",      I18N_NOOP("Secondary oscillator (LP)") },
    { "EXTRC_LP",    I18N_NOOP("Low power RC oscillator") },
    { "INTRC_F_POST",  I18N_NOOP("Internal fast RC oscillator with postscaler") }, { 0, 0 } } },
  { { "POSCMD", I18N_NOOP("Primary oscillator mode") }, Fixed, {
    { "Off", I18N_NOOP("Off") },
    { "HS",  I18N_NOOP("HS crystal oscillator") },
    { "XT",  I18N_NOOP("XT crystal oscillator") },
    { "EC",  I18N_NOOP("External clock") }, { 0, 0 } } },
  { { "TEMP", I18N_NOOP("Temperature protection") }, Toggle, { { 0, 0 } } },
  { { "OSCIOFNC", I18N_NOOP("OSC2 pin function") }, Fixed, {
    { "IO", I18N_NOOP("Digital I/O") },
    { "Clock", I18N_NOOP("Clock output") }, { 0, 0 } } },
  { { "WINDIS", I18N_NOOP("Watchdog timer window") }, Toggle, { { 0, 0 } } },
  { { "WDTPRE", I18N_NOOP("Watchdog timer prescaler") }, Ratio, { { 0, 0 } } },
  { { "WDTPOST", I18N_NOOP("Watchdog timer postscaler") }, Ratio, { { 0, 0 } } },
  { { "JTAGEN", I18N_NOOP("JTAG port enabled") }, Toggle, { { 0, 0 } } },
  { { "IOL1WAY", I18N_NOOP("Peripheral pin select configuration") }, Fixed, {
    { "One reconfiguration", I18N_NOOP("Allow only one reconfiguration") },
    { "Multiple reconfigurations", I18N_NOOP("Allow multiple reconfigurations") }, { 0, 0 } } },
  { { "ALTI2C", I18N_NOOP("Alternate I2C pins") }, Pin, { { 0, 0 } } },
  { { "FRANGE", I18N_NOOP("Frequency range selection for FRC oscillator") }, Fixed, {
    { "High range", I18N_NOOP("High range (nominal FRC frequency is 14.1 MHz)") },
    { "Low range", I18N_NOOP("Low range (nominal FRC frequency is 9.7 MHz)") }, { 0, 0 } } },

  { { 0, 0 }, Fixed, { { 0, 0 } } }
};

QMap<QString, Pic::Config::MapData> *Pic::Config::_masks = 0;
QMap<QString, Pic::Config::MapData> &Pic::Config::masks()
{
  if ( _masks==0 ) {
    _masks = new QMap<QString, MapData>;
    for (uint i=0; DATA[i].mask.name; i++) {
      (*_masks)[DATA[i].mask.name] = MapData(i, -1);
      if ( DATA[i].type==MemoryRange ) {
        for (uint k=0; k<Protection::MAX_NB_BLOCKS; k++)
          (*_masks)[QString("%1_%2").arg(DATA[i].mask.name).arg(k)] = MapData(i, k);
      }
    }
  }
  return *_masks;
}

bool Pic::Config::hasMaskName(const QString &mask)
{
  return masks().contains(mask);
}

QString Pic::Config::maskLabel(const QString &mask)
{
  const MapData &mp = masks()[mask];
  QString s = i18n(DATA[mp.index].mask.label);
  if ( mp.block>=0 ) return i18n("%1 for block %2").arg(s).arg(mp.block);
  return s;
}

const Pic::Config::Mask *Pic::Config::findMask(const QString &mask, uint *wordIndex) const
{
  for (uint i=0; i<uint(_words.count()); i++)
    for (uint k=0; k<uint(_words[i].masks.count()); k++) {
      if ( _words[i].masks[k].name==mask ) {
        if (wordIndex) *wordIndex = i;
        return &_words[i].masks[k];
      }
    }
  return 0;
}

const Pic::Config::Value *Pic::Config::findValue(const QString &mask, const QString &value) const
{
  const Mask *cmask = findMask(mask);
  if ( cmask==0 ) return 0;
  for (uint i=0; i<uint(cmask->values.count()); i++)
    if ( cmask->values[i].name==value ) return &cmask->values[i];
  return 0;
}

bool Pic::Config::checkValueName(const QString &mask, const QString &name) const
{
  const Data &data = DATA[masks()[mask].index];
  QString pinRegexp = "[A-Z]+\\d*(/[A-Z]+\\d*)?";
  switch (data.type) {
    case Fixed: break;
    case ValueDouble: {
      bool ok;
      (void)name.toDouble(&ok);
      if (ok) return true;
      break;
    }
    case ValueUInt: {
      bool ok;
      (void)name.toUInt(&ok);
      if (ok) return true;
      break;
    }
    case Ratio: {
      QRegExp regexp("(\\d+):(\\d+)");
      if ( regexp.exactMatch(name) ) {
        bool ok1, ok2;
        (void)regexp.cap(1).toUInt(&ok1);
        (void)regexp.cap(2).toUInt(&ok2);
        if ( ok1 && ok2 ) return true;
      }
      break;
    }
    case MemoryRange:
      return _protection.checkRange(mask, name);
    case Toggle:
      if ( name=="On" || name=="Off" ) return true;
      break;
    case Pin: {
      QRegExp regexp(pinRegexp);
      if ( regexp.exactMatch(name) ) return true;
      break;
    }
    case Pins: {
      QRegExp regexp(pinRegexp + "(, " + pinRegexp + ")+");
      if ( regexp.exactMatch(name) ) return true;
      break;
    }
  }
  for (uint i=0; data.values[i].name; i++)
    if ( data.values[i].name==name ) return true;
  return false;
}

QString Pic::Config::valueLabel(const QString &mask, const QString &name)
{
  const Data &data = DATA[masks()[mask].index];
  switch (data.type) {
    case Fixed:
    case ValueDouble:
    case ValueUInt:
    case Pin:
    case Pins:
    case Ratio: break;
    case MemoryRange:
      if ( name=="All" ) return i18n("All");
      if ( name=="Off" ) return i18n("Disabled");
      break;
    case Toggle:
      if ( name=="On" ) return i18n("Enabled");
      if ( name=="Off" ) return i18n("Disabled");
      break;
  }
  for (uint i=0; data.values[i].name; i++)
    if ( data.values[i].name==name ) return i18n(data.values[i].label);
  return name;
}

BitValue Pic::Config::Word::usedMask() const
{
  BitValue mask = 0x0;
  for (uint i=0; i<uint(masks.count()); i++) mask |= masks[i].value;
  return mask;
}

//-----------------------------------------------------------------------------
QDataStream &Pic::operator <<(QDataStream &s, const Config::Value &value)
{
  s << value.name << value.cnames << value.extraCnames << value.value;
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, Config::Value &value)
{
  s >> value.name >> value.cnames >> value.extraCnames >> value.value;
  return s;
}

QDataStream &Pic::operator <<(QDataStream &s, const Config::Mask &mask)
{
  s << mask.name << mask.value << mask.values;
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, Config::Mask &mask)
{
  s >> mask.name >> mask.value >> mask.values;
  return s;
}

QDataStream &Pic::operator <<(QDataStream &s, const Config::Word &word)
{
  s << word.name << word.ignoredCNames << word.wmask << word.pmask << word.cmask << word.bvalue << word.masks;
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, Config::Word &word)
{
  s >> word.name >> word.ignoredCNames >> word.wmask >> word.pmask >> word.cmask >> word.bvalue >> word.masks;
  return s;
}

QDataStream &Pic::operator <<(QDataStream &s, const Config &config)
{
  s << config._words;
  return s;
}
QDataStream &Pic::operator >>(QDataStream &s, Config &config)
{
  s >> config._words;
  return s;
}
