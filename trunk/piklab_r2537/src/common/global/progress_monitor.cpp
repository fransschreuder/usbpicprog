/***************************************************************************
 *   Copyright (C) 2007 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#include "progress_monitor.h"

ProgressMonitor::ProgressMonitor(QObject* parent)
  : QObject(parent, "progress_monitor")
{
  _current = _tasks.end();
}

void ProgressMonitor::clear()
{
  _tasks.clear();
  _current = _tasks.end();
  emit showProgress(false);
}

void ProgressMonitor::appendTask(const QString &label, uint nbSteps)
{
  Task task;
  task.label = label;
  task.nbSteps = nbSteps;
  task.nbDoneSteps = 0;
  _tasks.append(task);
}

void ProgressMonitor::insertTask(const QString &label, uint nbSteps)
{
  Task task;
  task.label = label;
  task.nbSteps = nbSteps;
  task.nbDoneSteps = 0;
  if ( _current==_tasks.end() ) _current = _tasks.prepend(task);
  else _current = _tasks.insert(_current, task);
  update();
}

uint ProgressMonitor::nbSteps() const
{
  uint nb = 0;
  for (uint i=0; i<uint(_tasks.count()); i++) nb += _tasks[i].nbSteps;
  return nb;
}

uint ProgressMonitor::nbDoneSteps() const
{
  uint nb = 0;
  for (uint i=0; i<uint(_tasks.count()); i++) nb += _tasks[i].nbDoneSteps;
  return nb;
}

void ProgressMonitor::update()
{
  QString label = (_current==_tasks.end() ? QString::null : (*_current).label);
  emit setLabel(label);
  emit setTotalProgress(nbSteps());
  emit setProgress(nbDoneSteps());
  emit showProgress(true);
}

void ProgressMonitor::startNextTask()
{
  if ( _current==_tasks.end() ) _current = _tasks.begin();
  else ++_current;
  Q_ASSERT( _current!=_tasks.end() ) ;
  update();
}

void ProgressMonitor::addTaskProgress(uint nbSteps)
{
  Q_ASSERT( _current!=_tasks.end() ) ;
  if ( _current==_tasks.end() ) return;
  uint nb = (*_current).nbDoneSteps + nbSteps;
  Q_ASSERT( nb<=(*_current).nbSteps );
  if ( nb>(*_current).nbSteps ) qDebug("%s %i+%i > %i", (*_current).label.latin1(), (*_current).nbDoneSteps, nbSteps, (*_current).nbSteps);
  (*_current).nbDoneSteps = QMIN(nb, (*_current).nbSteps);
  update();
}
