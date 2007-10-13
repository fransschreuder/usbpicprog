/***************************************************************************
 * Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "pickit1.h"

bool Pickit1::Baseline::init()
{
  Array cmd;
  cmd[0] = 'c';
  cmd[1] = 0x00;  // Load Configuration
  cmd[2] = 0x02;  // Load Program latches
  cmd[3] = 0x03;  // Load Data latches
  cmd[4] = 0x04;  // Read Program memory
  cmd[5] = 0x05;  // Read Data latches
  cmd[6] = 0x06;  // Increment Address
  cmd[7] = 0x08;  // Begin programming internally timed
  if ( !hardware().port().command(cmd) ) return false;
  cmd[0] = 0x08;  // Begin Programming externally timed
  cmd[1] = 0x0E;  // End externally time programming cycle
  cmd[2] = 0x09;  // Bulk Erase program memory
  cmd[3] = 0x0B;  // Bulk Erase Data memory
  cmd[4] = 0xFF;  // Read Program memory
  cmd[5] = 0xFF;  // Read Data latches
  cmd[6] = 0xFF;  // Increment Address
  cmd[7] = 0xFF;  // Begin programming internally timed
  return hardware().port().command(cmd);
}

bool Pickit1::Baseline::incrementPC(uint nb)
{
  if ( (nb & 0xFF)!=0 ) return Pickit::Baseline::incrementPC(nb);
  // work around bugs in firmware
  Array cmd;
  uint high = (nb >> 8) & 0xFF;
  log(Log::ExtraDebug, QString("work around bug in firmware (nb_inc=%1 high=%2)")
                       .arg(toHexLabel(nb, 4)).arg(toHexLabel(high, 2)));
  if ( high==1 ) {
      cmd[0] = 'I';
      cmd[1] = 0x40;
      cmd[2] = 0x00;
      cmd[3] = 'I';
      cmd[4] = 0xC0;
      cmd[5] = 0x00;
  } else {
    cmd[0] = 'I';
    cmd[1] = 0x00;
    cmd[2] = high-1;
  }
  return hardware().port().command(cmd);
}

//----------------------------------------------------------------------------
bool Pickit1::P16F::init()
{
  Array cmd;
  cmd[0] = 'c';
  cmd[1] = 0x00;   // Load Configuration
  cmd[2] = 0x02;   // Load Program latches
  cmd[3] = 0x03;   // Load Data latches
  cmd[4] = 0x04;   // Read Program memory
  cmd[5] = 0x05;   // Read Data latches
  cmd[6] = 0x06;   // Increment Address
  cmd[7] = 0x08;   // Begin programming internally timed
  if ( !hardware().port().command(cmd) ) return false;
  cmd[0] = 0x18;   // Begin Programming externally timed
  cmd[1] = 0x0A;   // End externally time programming cycle
  cmd[2] = 0x09;  // Bulk Erase program memory
  cmd[3] = 0x0B;  // Bulk Erase Data memory
  cmd[4] = 0xFF;  // Read Program memory
  cmd[5] = 0xFF;  // Read Data latches
  cmd[6] = 0xFF;  // Increment Address
  cmd[7] = 0xFF;  // Begin programming internally timed
  return hardware().port().command(cmd);
}

bool Pickit1::P16F716::init()
{
  Array cmd;
  cmd[0] = 'c';
  cmd[1] = 0x00;   // Load Configuration
  cmd[2] = 0x02;   // Load Program latches
  cmd[3] = 0x03;   // Load Data latches
  cmd[4] = 0x04;   // Read Program memory
  cmd[5] = 0x05;   // Read Data latches
  cmd[6] = 0x06;   // Increment Address
  cmd[7] = 0x08;   // Begin programming internally timed
  if ( !hardware().port().command(cmd) ) return false;
  cmd[0] = 0x18;   // Begin Programming externally timed
  cmd[1] = 0x0E;   // End externally time programming cycle
  cmd[2] = 0x09;  // Bulk Erase program memory
  cmd[3] = 0x0B;  // Bulk Erase Data memory
  cmd[4] = 0xFF;  // Read Program memory
  cmd[5] = 0xFF;  // Read Data latches
  cmd[6] = 0xFF;  // Increment Address
  cmd[7] = 0xFF;  // Begin programming internally timed
  return hardware().port().command(cmd);
}
