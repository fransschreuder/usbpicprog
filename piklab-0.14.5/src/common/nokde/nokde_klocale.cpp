#include "nokde_klocale.h"

QString i18n(const QString &s, const QString &) { return s; }
QString locate(const QString &, const QString &) { return QString::null; }
