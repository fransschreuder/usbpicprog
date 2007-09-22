/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include "common/common/storage.h"
#include "common/global/purl.h"
#include "devices/base/generic_device.h"

namespace Breakpoint
{
class List;
enum State { Unknown, Active, Disabled };
enum MarkType { ProgramCounterActive = 0, ProgramCounterDisabled,
                BreakpointActive, BreakpointDisabled, BreakpointReached,
                BreakpointInvalid, Nb_MarkTypes };

//----------------------------------------------------------------------------
class Data {
public:
  Data(const PURL::Url &purl = PURL::Url(), uint pline = 0) : url(purl), line(pline) {}
  bool operator <(const Data &data) const { return ( url<data.url || line<data.line ); }
  bool operator ==(const Data &data) const { return ( url==data.url && line==data.line ); }
  PURL::Url url;
  uint line;
};
extern void updateActions(const Data *data);

//----------------------------------------------------------------------------
class List;
extern List &list();

class List : public GenericStorage
{
Q_OBJECT
public:
  List() {}
  void append(const Data &data);
  void remove(const Data &data);
  void clear();
  uint count() const { return _list.count(); }
  const Data &data(uint i) const { return _list[i].data; }
  bool contains(const Data &data) const { return find(data)!=_list.end(); }
  State state(const Data &data) const { return (*find(data)).state; }
  uint address(const Data &data) const { return (*find(data)).address; }
  void setState(const Data &data, State state);
  void setAddress(const Data &data, uint address);

private:
  class StateData {
  public:
    StateData() : address(Device::INVALID), state(Unknown) {}
    Data  data;
    uint  address;
    State state;
  };
  QValueList<StateData> _list;
  QValueList<StateData>::const_iterator find(const Data &data) const;
  QValueList<StateData>::iterator find(const Data &data);
};

} // namespace

#endif
