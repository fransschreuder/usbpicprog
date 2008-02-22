#ifndef _KURL_H_
#define _KURL_H_

#include <qfile.h>

#include "common/global/global.h"

class KURL : public Q3Url
{
public:
  KURL() {}
  KURL(const QString &s) : Q3Url(s) {}
  void cleanPath() {}
  bool isEmpty() const { return toString(false, false).isEmpty(); }
  QString fileName(bool b) const { Q_UNUSED(b); Q_ASSERT(!b); return Q3Url::fileName(); }
  static KURL fromPathOrURL(const QString &s) { return KURL(s); }
#if QT_VERSION>=0x040000
  bool operator <(const KURL &url) const { return path()<url.path(); }
  bool operator ==(const KURL &url) const { return path()==url.path(); }
  bool operator !=(const KURL &url) const { return path()!=url.path(); }
#endif
};

// ### DUMMY
class KTempFile
{
public:
  const QFile *file() const { return 0; }
  QFile *file() { return 0; }
};

#endif
