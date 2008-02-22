/***************************************************************************
 *   Copyright (C) 2003-2005 Alain Gibaud <alain.gibaud@free.fr>           *
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef DIRECT_16_H
#define DIRECT_16_H

#include "direct_pic.h"

namespace Direct
{
//----------------------------------------------------------------------------
class pic16 : public Pic8DeviceSpecific
{
public:
  pic16(::Programmer::Base &base) : Pic8DeviceSpecific(base) {}
  virtual BitValue get_word();
  virtual BitValue get_byte() { return get_word().maskWith(0xFF); }
  virtual void send_word(BitValue word);
  virtual void send_bits(BitValue d, uint nbBits);
  virtual void send_cmd(BitValue d) { send_bits(d, 6); }
  virtual bool doRead(Pic::MemoryRangeType type, Device::Array &data, const ::Programmer::VerifyData *vdata);
  virtual bool doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force);

protected:
  virtual bool setPowerOn() { return setPowerOnVddFirst(); }
  virtual bool skipMaskWords(Pic::MemoryRangeType type, const Device::Array &data, uint &i, bool force);
  virtual bool incrementPC(uint steps);
  virtual bool gotoMemory(Pic::MemoryRangeType type) = 0;
  virtual uint nbWordsCodeProg() const { return 1; }
  virtual void startProg(Pic::MemoryRangeType) { pulseEngine("k8,"); }
  virtual uint waitProgTime(Pic::MemoryRangeType type) const = 0;
  virtual void endProg(Pic::MemoryRangeType) {}
  virtual bool writeWords(Pic::MemoryRangeType type, const Device::Array &data, uint &i, bool force);
  virtual bool writeWords(Pic::MemoryRangeType type, const Device::Array &data, uint i);
  virtual BitValue readWord(Pic::MemoryRangeType type);
};

} // namespace

#endif
