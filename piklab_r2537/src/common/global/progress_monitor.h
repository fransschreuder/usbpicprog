/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef PROGRESS_MONITOR_H
#define PROGRESS_MONITOR_H

#include "global.h"

class ProgressMonitor : public QObject
{
Q_OBJECT
public:
  ProgressMonitor(QObject *parent = 0);
  void clear();
  void appendTask(const QString &label, uint nbSteps);
  void insertTask(const QString &label, uint nbSteps);
  void startNextTask();
  void addTaskProgress(uint nbSteps);
  uint nbSteps() const;
  uint nbDoneSteps() const;

public slots:
  void update();

signals:
  void showProgress(bool show);
  void setLabel(const QString &label);
  void setTotalProgress(uint nbSteps);
  void setProgress(uint nbSteps);

private:
  class Task {
  public:
    QString label;
    uint nbSteps, nbDoneSteps;
  };
  QValueList<Task> _tasks;
  QValueList<Task>::iterator _current;
};

#endif
