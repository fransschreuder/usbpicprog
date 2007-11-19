/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PORT_SELECTOR_H
#define PORT_SELECTOR_H

#include <qradiobutton.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qlabel.h>

#include "common/port/port.h"
namespace Programmer { class Group; }

class PortSelector : public QFrame
{
  Q_OBJECT
public:
  PortSelector(QWidget *parent);
  void setGroup(const Programmer::Group &group);
  Port::Description portDescription() const { return Port::Description(type(), device(type())); }
  void saveConfig();
  void setStatus(PortType type, const QString &message);

signals:
  void changed();

private slots:
  void textChanged();

private:
  const Programmer::Group *_group;
  QGridLayout  *_top, *_grid;
  QWidget      *_main;
  QButtonGroup *_bgroup;
  QComboBox    *_combos[PortType::Nb_Types];
  QLabel       *_status[PortType::Nb_Types];
  bool          _pending;
  static const char * const LABELS[PortType::Nb_Types];

  void addPortType(const Port::Description &pd);
  PortType type() const;
  QString device(PortType type) const;
};

#endif
