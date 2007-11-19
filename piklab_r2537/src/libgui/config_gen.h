/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef CONFIG_GEN_H
#define CONFIG_GEN_H

class QHBoxLayout;
class QLabel;

#include "common/gui/dialog.h"
#include "tools/base/tool_group.h"
namespace DeviceChooser { class Button; }
namespace Device {
  class Memory;
  class MemoryEditor;
}
class SimpleTextEditor;
class KeyComboBox;

//-----------------------------------------------------------------------------
class GeneratorDialog : public Dialog
{
Q_OBJECT
public:
  GeneratorDialog(const QString &title, QWidget *parent, const char *name);
  void set(const Device::Data *data, const Tool::Group &group, PURL::ToolType stype);

protected slots:
  void typeChanged();
  virtual void reset() { compute(); }
  virtual void compute();
  virtual void slotUser1();

protected:
  QHBoxLayout       *_hbox;
  DeviceChooser::Button *_deviceChooser;
  KeyComboBox       *_configType, *_toolType;
  SimpleTextEditor  *_text;
  QLabel            *_warning;

  PURL::ToolType toolType() const;
  void setToolType(PURL::ToolType stype);
  virtual SourceLine::List generateLines(bool &ok) const = 0;
};

//-----------------------------------------------------------------------------
class ConfigGenerator : public GeneratorDialog
{
Q_OBJECT
public:
  ConfigGenerator(QWidget *parent);
  virtual ~ConfigGenerator();

private slots:
  virtual void reset();

private:
  Device::Memory       *_memory;
  Device::MemoryEditor *_configEditor;

  void setToolType(PURL::ToolType stype);
  virtual SourceLine::List generateLines(bool &ok) const;
};

//-----------------------------------------------------------------------------
class TemplateGenerator : public GeneratorDialog
{
Q_OBJECT
public:
  TemplateGenerator(QWidget *parent);

private:
  virtual SourceLine::List generateLines(bool &ok) const;
};

#endif
