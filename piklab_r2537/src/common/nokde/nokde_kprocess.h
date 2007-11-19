#ifndef _KPROCESS_H_
#define _KPROCESS_H_

#include <qdir.h>
#include "common/global/global.h"
#include "common/common/synchronous.h"
#if QT_VERSION<0x040000
class QProcess;
#else
class Q3Process;
#endif

class KProcess : public QObject
{
Q_OBJECT
public:
  KProcess(QObject *parent = 0, const char *name = 0);
  void clearArguments() { _arguments.clear(); }
  KProcess &operator<< (const QString &arg) { _arguments += arg; return *this; }
  KProcess &operator<< (const QStringList &args) { _arguments += args; return *this; }
  QStringList args() const { return _arguments; }
  void setEnvironment(const QString &name, const QString &value) { _environment += name + "=" + value; }
  bool start();
  bool writeStdin(const char *buffer, int len);
  bool kill();
  bool kill(int n);
  int exitStatus() const;
  bool isRunning() const;
  void setWorkingDirectory(const QDir &dir);
  void setUseShell(bool useShell);

signals:
  void processExited(KProcess *process);
  void receivedStdout(KProcess *process, char *buffer, int len);
  void receivedStderr(KProcess *process, char *buffer, int len);

private slots:
  void processExitedSlot();
  void readyReadStdoutSlot();
  void readyReadStderrSlot();

private:
#if QT_VERSION<0x040000
  QProcess *_process;
#else
  Q3Process  *_process;
#endif
  QStringList _arguments,_environment;
};

#endif
