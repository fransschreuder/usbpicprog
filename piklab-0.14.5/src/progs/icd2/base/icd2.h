/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *   Copyright (C) 2002-2003 Stephen Landamore <stephen@landamore.com>     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef ICD2_H
#define ICD2_H

#include "icd.h"

namespace Icd2
{
//-----------------------------------------------------------------------------
  extern const uchar TARGET_MODE_VALUES[Pic::Nb_TargetModes];
  extern const char * const RESET_MODE_VALUES[Pic::Nb_ResetModes];

  class TestData {
  public:
    TestData();
    TestData(const QString &rx);
    enum VoltageType { TargetVdd = 0, ModuleVpp, MclrGround, MclrVdd, MclrVpp,
                       Nb_VoltageTypes };
    bool pass() const;
    QString result(VoltageType type) const;
    QString pretty(VoltageType type) const;
    static const char * const VOLTAGE_LABELS[Nb_VoltageTypes];

  private:
    int _voltages[Nb_VoltageTypes];
    static const int RESULT_TYPE_VALUES[::Programmer::Nb_ResultTypes+1];
  };

//-----------------------------------------------------------------------------
class Hardware : public Icd::Hardware
{
public:
  Hardware(::Programmer::Base &base, Port::Base *port) : Icd::Hardware(base, port) {}
  bool command(const QString &command, uint responseSize);

// initialization
  virtual bool uploadFirmware(const Pic::Memory &memory);
  virtual bool setTarget();
  bool setup();

// status
  virtual bool getFirmwareVersion(VersionData &version);
  uint getFirmwareId();
  bool getDebugExecVersion(VersionData &version);
  virtual bool setTargetPowerOn(bool on);
  virtual bool readVoltage(Pic::VoltageType type, double &value);
  virtual bool readVoltages(VoltagesData &voltages);
  virtual bool getTargetMode(Pic::TargetMode &mode);
  virtual bool setTargetReset(Pic::ResetMode mode);
  bool selfTest(TestData &test);

// programming
  virtual bool readMemory(Pic::MemoryRangeType type, uint wordOffset, uint nbWords, Device::Array &data, const ::Programmer::VerifyData *vdata);
  virtual bool writeMemory(Pic::MemoryRangeType type, const Device::Array &data, uint wordOffset, uint nbWords);
  virtual bool eraseAll();
  bool setWriteMode(Pic::WriteMode mode);

// debugging
  virtual bool readRegister(uint address, uint &value, uint nbBytes);
  virtual bool writeRegister(uint address, uint value, uint nbBytes);
  virtual bool resumeRun();
  virtual bool step();
  virtual bool haltRun();
  virtual uint getProgramCounter();

protected:
  virtual QString receivedData() const { return _rx.mid(5, _rx.length()-8); }

private:
  struct VoltageTypeData {
    const char *command;
    double factor;
  };
  static const VoltageTypeData VOLTAGE_TYPE_DATA[Pic::Nb_VoltageTypes];
  static const char * const WRITE_MODE_VALUES[Pic::Nb_WriteModes];

  bool sendCommand(const QString &command);
  bool receiveResponse(const QString &command, uint responseSize, bool poll);
  bool readBlock(uint nbBytesWord, uint nbWords, Device::Array &data);
  bool writeBlock(uint nbBytesWord, const Device::Array &data, uint wordOffset, uint nbWords);
  const char *readCommand(Pic::MemoryRangeType type) const;
  const char *writeCommand(Pic::MemoryRangeType type) const;
};

} // namespace

#endif
