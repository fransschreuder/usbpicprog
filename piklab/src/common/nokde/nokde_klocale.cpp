#include "nokde_klocale.h"

QString i18n(const QString &, const QString &text) { return text; }
QString locate(const QString &, const QString &) { return QString::null; }
