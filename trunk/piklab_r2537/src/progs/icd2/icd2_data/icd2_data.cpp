/***************************************************************************
 *   Copyright (C) 2005-2007 Nicolas Hadacek <hadacek@kde.org>             *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "progs/icd2/base/icd2_data.h"

const Icd2::FirmwareVersionData Icd2::MIN_VERSION_DATA = {
  7, 21, // MPLAB 7.21
 {{ 2,  6,  5 }, // 01
  { 0,  0,  0 }, // 02 [none]
  { 0,  0,  0 }, // 03 [none]
  { 2,  6,  6 }, // 04
  { 1,  2,  5 }, // 05
  { 1,  2,  2 }, // 06
  { 1,  2,  9 }, // 07
  { 1,  3,  4 }, // 08
  { 1,  0,  0 }, // 09
  { 1,  3,  1 }, // 10
  { 0,  0, 27 }, // 11
  { 1,  2,  0 }, // 12
  { 0,  0,  0 }, // 13 [none]
  { 0,  0,  0 }, // 14
  { 0,  0,  0 }} // 15
};

const Icd2::FirmwareVersionData Icd2::MAX_VERSION_DATA = {
  7, 41, // MPLAB 7.41
 {{ 2, 7, 0 }, // 01
  { 0, 0, 0 }, // 02 [none]
  { 0, 0, 0 }, // 03 [none]
  { 2, 6, 8 }, // 04
  { 1, 3, 9 }, // 05
  { 1, 2, 2 }, // 06
  { 1, 4, 0 }, // 07
  { 1, 5, 0 }, // 08
  { 1, 0, 0 }, // 09
  { 1, 4, 4 }, // 10
  { 1, 0, 4 }, // 11
  { 1, 2, 0 }, // 12
  { 0, 0, 0 }, // 13 [none]
  { 2, 0, 6 }, // 14
  { 1, 1, 0 }} // 15
};

const Icd2::FamilyData Icd2::FAMILY_DATA[] = {
  { "12F629",       1, "12F629",  0x0300, 0x0DF, 0x0DE, 0x0D4/*?*/ }, // 12F629/675
  { "16F676",       1, "16F676",  0x0300, 0x0DF, 0x0DE, 0x0D4/*?*/ }, // 16F630/676
  { "16F648A",      1, "16F648A", 0x0F00, 0x16F, 0x16E, 0x1F0/*?*/ }, // 16F627A/628A/648A
  { "16F716",       1, "16F716",  0x0700, 0x06F, 0x06E, 0x0F0/*?*/ },
  { "16F7X7",       1, "16F7X7",  0x1F00, 0x16F, 0x16E, 0x1F0/*?*/ }, // 16F737/747/767/777
  { "16F819",       1, "16F819",  0x0700, 0x16F, 0x16E, 0x1F0/*?*/ }, // 16F818/819
  { "16F88",        1, "16F88",   0x0F00, 0x1EF, 0x1EE, 0x1F0/*?*/ }, // 16F87/88
  { "16F872",       1, "16F872",  0x0700, 0x1BF, 0x1BE, 0x1F0 }, // 16F870/871/872
  { "16F874",       1, "16F874",  0x0F00, 0x1FF, 0x1FE, 0x17F/*?*/ }, // 16F873/873A/874/874A
  { "16F877",       1, "16F877",  0x1F00, 0x1EF, 0x1EE, 0x1F0/*?*/ }, // 16F876/876A/877/877A
  { "16F7X",        1, 0,         0,      0,     0,     0          }, // 16F73
  { 0,              2, 0,         0,      0,     0,     0          }, // 30F for revision A2 (legacy: not in MPLAB 7)
  { 0,              3, "30F",     0,      0,     0,     0          }, // 30F for revision A3/B0 (legacy: not in MPLAB 7)
  { "18F_4",        4, "",        0,      0,     0,     0          }, // debug executive filename is computed at runtime
  { "18F_5",        5, "",        0,      0,     0,     0          }, // debug executive filename is computed at runtime
  { "18CX01",       6, 0,/*"18CX01"*/0,      0,     0,     0          }, // 16C601/801
  { "10F2XX",       7, 0,         0,      0,     0,     0          },
  { "16F5X",        7, 0,         0,      0,     0,     0          }, // 16F505/506
  { "12F635",       8, "12F635",  0x0700, 0x06F, 0x06E, 0x0F0/*?*/ }, // 12F635/683 16F636/639
  { "16F785",       8, "16F916",  0x0700, 0x06F, 0x06E, 0x1F0/*?*/ }, // 16F785
  { "16F916",       8, "16F916",  0x1F00, 0x16F, 0x16E, 0x1F0/*?*/ }, // 16F913/914/916/917/946
  { "16F684",       8, "16F684",  0x0700, 0x16F, 0x16E, 0x1F0/*?*/ }, // 16F684
  { "16F688",       8, "16F688",  0x0F00, 0x16F, 0x16E, 0x1F0/*?*/ }, // 16F685/687/688/689/690
  { "16F677",       8, 0,         0,      0,     0,     0          }, // 16F677
  { "16F629",       9, 0,         0,      0,     0,     0          }, // 16F629
  { "30F",         10, 0/*"30F_REVB"*/,0, 0,     0,     0          }, // 30F revision B1 and above
  { "18F_J",       11, 0,         0,      0,     0,     0          }, // 18FXXJXX (?)
  { "16F72",       12, 0,         0,      0,     0,     0          }, // 16F72/8X/627/628/84A
  { "24F",         14, 0/*"24F"*/,0,      0,     0,     0          }, // from MPLAB 7.3
  { "33F",         14, 0/*"33F"*/,0,      0,     0,     0          }, // from MPLAB 7.3
  { "16F61X",      15, 0,         0,      0,     0,     0          }, // 12F61X/16F61X

  { 0,              0, 0,         0,      0,     0,     0          }
};
