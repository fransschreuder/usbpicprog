/***************************************************************************
 * Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PICKIT2V2_H
#define PICKIT2V2_H

#include "progs/pickit2/base/pickit2.h"
#include "pickit2v2_data.h"

namespace Pickit2V2
{
//-----------------------------------------------------------------------------
enum FirmwareCommand {
  EnterBootloader = 0x42, NoOperation = 0x5A, FirmwareVersion = 0x76,

  SetVdd = 0xA0, SetVpp = 0xA1, ReadStatus = 0xA2, ReadVoltages = 0xA3,
  DownloadScript = 0xA4, RunScript = 0xA5, ExecuteScript = 0xA6,
  ClearDownloadBuffer = 0xA7, DownloadData = 0xA8, ClearUploadBuffer = 0xA9,
  UploadData = 0xAA, ClearScriptBuffer = 0xAB, UploadDataNoLen = 0xAC,
  EndOfBuffer = 0xAD, Reset = 0xAE, ScriptBufferChecksum = 0xAF,

  SetVoltageCalibrations = 0xB0, WriteInternalEEprom = 0xB1,
  ReadInternalEEprom = 0xB2, EnterUARTMode = 0xB3, ExitUARTMode = 0xB4
};

enum ScriptCommand {
  VddOn = 0xFF, VddOff = 0xFE, VddGroundOn = 0xFD, VddGroundOff = 0xFC,
  VppOn = 0xFB, VppOff = 0xFA, VppPwmOn = 0xF9, VppPwmOff = 0xF8,
  MclrGroundOn = 0xF7, MclrGroundOff = 0xF6, BusyLedOn = 0xF5, BusyLedOff = 0xF4,
  SetIcspPins = 0xF3, WriteByteLiteral = 0xF2, WriteByteBuffer = 0xF1,
  ReadByteBuffer = 0xF0, ReadByte = 0xEF, WriteBitsLiteral = 0xEE,
  WriteBitsBuffer = 0xED, ReadBitsBuffer = 0xEC, ReadBits = 0xEB,
  SetIcspSpeed = 0xEA, Loop = 0xE9, DelayLong = 0xE8, DelayShort = 0xE7,
  IfEqGoto = 0xE6, IfGtGoto = 0xE5, GotoIndex = 0xE4, ExitScript = 0xE3,

  PeekSfr = 0xE2, PokeSfr = 0xE1, IcdSlaveRx = 0xE0, IcdSlaveTxLiteral = 0xDF,
  IcdSlaveTxBuffer = 0xDE, LoopBuffer = 0xDD, IcspStatesBuffer = 0xDC,
  PopDownload = 0xDB, CoreInst18 = 0xDA, CoreInst24 = 0xD9, Nop24 = 0xD8,
  Visi24 = 0xD7, Rd2ByteBuffer = 0xD6, Rd2BitsBuffer = 0xD5, WriteBufWordW = 0xD4,
  WriteBufByteW = 0xD3, ConstWriteDl = 0xD2, WriteBitsLitHld = 0xD1,
  WriteBitsBufHld = 0xD0, SetAux = 0xCF, AuxStateBuffer = 0xCE, I2cStart = 0xCD,
  I2cStop = 0xCC, I2cWriteByteLiteral = 0xCB, I2cWriteByteBuffer = 0xCA,
  I2cReadByteAck = 0xC9, I2cReadByteNack = 0xC8, SpiWriteByteLiteral = 0xC7,
  SpiWriteByteBuffer = 0xC6, SpiReadByteBuffer = 0xC5, SpiReadWriteByteLiteral = 0xC4,
  SpiReadWriteByteBuffer = 0xC3
};
extern const FamilyData *familyData(const Pic::Data &device);

//-----------------------------------------------------------------------------
class Array : public Pickit::Array
{
public:
  Array() : Pickit::Array(64, EndOfBuffer, PrintEscapeAll) {}
};

//-----------------------------------------------------------------------------
class USBPort : public Pickit2::USBPort
{
public:
  USBPort(Log::Base &manager) : Pickit2::USBPort(manager) {}
  virtual Pickit::Array array() const { return Array(); }
};

enum StatusFlag {
  DownloadBufferOverflow = 0x8000, ScriptBufferOverflow = 0x4000,
  RunScriptOnEmptyScript = 0x2000, ScriptAbortDownloadEmpty = 0x1000,
  ScriptAbortUploadFull = 0x0800, IcdTransferTimeout = 0x0400,
  UARTModeEnabled = 0x0200, ResetSinceLastStatusRead = 0x0100,
  ButtonPressed = 0x0040, VppError = 0x0020, VddError = 0x0010,
  VppIsOn = 0x0008, VppGroundIsOn = 0x0004,
  VddIsOn = 0x0002, VddGroundIsOn = 0x0001 };
Q_DECLARE_FLAGS(StatusFlags, StatusFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(StatusFlags)

//-----------------------------------------------------------------------------
class Hardware : public ::Programmer::PicHardware
{
public:
  Hardware(::Programmer::Base &base);
  USBPort &port() { return static_cast<USBPort &>(*_port); }
  const USBPort &port() const { return static_cast<USBPort &>(*_port); }

  bool getMode(VersionData &version, ::Programmer::Mode &mode);
  bool setTargetReset(Pic::ResetMode mode);
  bool setVddVoltage(double value, double threshold);
  bool setVppVoltage(double value, double threshold);
  bool setVddOn(bool on);
  bool executeScript(uint i);
  bool setTarget();
  bool setFastProgramming(bool fast);
  virtual bool readVoltages(VoltagesData &voltagesData);
  bool readStatus(ushort &status);
  bool readMemory(Pic::MemoryRangeType type, ::Device::Array &data, const ::Programmer::VerifyData *vdata);
  bool writeMemory(Pic::MemoryRangeType type, const ::Device::Array &data, bool force);
  bool eraseAll();
  bool eraseRange(Pic::MemoryRangeType type);

private:
  enum { UploadBufferNbBytes = 128, DownloadBufferNbBytes = 256 };
  uint _scriptBufferChecksum;
  bool _deviceSet, _fastProgramming;

  virtual bool internalConnectHardware();
  bool getScriptBufferChecksum(uint &checksum);
  bool downloadScript(ScriptType type, uint i);
  bool downloadScripts();
  bool sendScript(const ushort *script, uint length);
  bool resetPickit2() { return port().command(Reset); }
  bool setIcspSpeed(uchar speed);
  bool downloadAddress(uint address);
  bool runScript(ScriptType stype, uint repetitions = 1, uint nbNoLens = 0);
  bool prepareRead(Pic::MemoryRangeType type, uint wordIndex);
  bool prepareWrite(Pic::MemoryRangeType type, uint wordIndex);
};

//-----------------------------------------------------------------------------
class DeviceSpecific : public ::Programmer::PicDeviceSpecific
{
public:
  DeviceSpecific(::Programmer::Base &base) : ::Programmer::PicDeviceSpecific(base) {}
  virtual bool canEraseAll() const { return true; }
  virtual bool canEraseRange(Pic::MemoryRangeType type) const;
  virtual bool canReadRange(Pic::MemoryRangeType) const;
  virtual bool canWriteRange(Pic::MemoryRangeType) const;
  Hardware &hardware() { return static_cast<Hardware &>(*_base.hardware()); }
  virtual bool setPowerOn() { return true; }
  virtual bool setPowerOff() { return true; }
  virtual bool setTargetPowerOn(bool on) { return hardware().setVddOn(on); }
  virtual bool doErase(bool) { return hardware().eraseAll(); }
  virtual bool doEraseRange(Pic::MemoryRangeType type) { return hardware().eraseRange(type); }
  virtual bool doRead(Pic::MemoryRangeType type, ::Device::Array &data, const ::Programmer::VerifyData *vdata) {
    (void)hardware().readMemory(type, data, vdata);
    // ### REMOVE ME
    ushort status;
    if ( !hardware().readStatus(status) ) return false;
    return true;
  }
  virtual bool doWrite(Pic::MemoryRangeType type, const Device::Array &data, bool force);

};

} // namespace

#endif
