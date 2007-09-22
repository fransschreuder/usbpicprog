#include "nokde_kprocess.h"

#if QT_VERSION<0x040000
#  include <qprocess.h>
#  define Q3Process QProcess
#else
#  include <Qt3Support/Q3Process>
#endif

#if defined(Q_OS_UNIX)
#  include <signal.h>
#endif

KProcess::KProcess(QObject *parent, const char *name)
  : QObject(parent, name)
{
  _process = new Q3Process(this);
  connect(_process, SIGNAL(processExited()), SLOT(processExitedSlot()));
  connect(_process, SIGNAL(readyReadStdout()), SLOT(readyReadStdoutSlot()));
  connect(_process, SIGNAL(readyReadStderr()), SLOT(readyReadStderrSlot()));
}

bool KProcess::start()
{
  _process->setArguments(_arguments);
  QStringList env;
  if ( !_environment.isEmpty() ) {
    for (uint i=0; environ[i]; i++) env += environ[i];
    env += _environment;
  }
  return _process->start(env.isEmpty() ? 0 : &env);
}

void KProcess::processExitedSlot()
{
  readyReadStdoutSlot();
  readyReadStderrSlot();
  emit processExited(this);
}

void KProcess::readyReadStdoutSlot()
{
  QByteArray a = _process->readStdout();
  emit receivedStdout(this, a.data(), a.count());
}

void KProcess::readyReadStderrSlot()
{
  QByteArray a = _process->readStderr();
  emit receivedStderr(this, a.data(), a.count());
}

bool KProcess::writeStdin(const char *buffer, int len)
{
#if QT_VERSION<0x040000
  QByteArray a;
  a.assign(buffer, len);
#else
  QByteArray a(buffer, len);
#endif
  _process->writeToStdin(a);
  return true;
}

bool KProcess::kill()
{
  _process->kill();
  return true;
}

bool KProcess::kill(int n)
{
#if defined(Q_OS_UNIX)
  return ( ::kill(_process->processIdentifier(), n)!=-1 );
#elif defined(Q_OS_WIN)
  // #### impossible to do ??
  return false;
#endif
}

int KProcess::exitStatus() const
{
  return _process->exitStatus();
}

bool KProcess::isRunning() const
{
  return _process->isRunning();
}

void KProcess::setWorkingDirectory(const QDir &dir)
{
  return _process->setWorkingDirectory(dir);
}
