#ifndef _KURL_H_
#define _KURL_H_

#include "common/global/global.h"

class KURL : public Q3Url
{
public:
  KURL() {}
  KURL(const QString &s) {
    Q3Url u(s);
    setPath(u.path());
  }
  void cleanPath() {}
  bool isEmpty() const { return toString(false, false).isEmpty(); }
#if QT_VERSION>=0x040000
  bool operator <(const KURL &url) const { return path()<url.path(); }
  bool operator ==(const KURL &url) const { return path()==url.path(); }
  bool operator !=(const KURL &url) const { return path()!=url.path(); }
#endif
};

#endif
