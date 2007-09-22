#ifndef WIN_KLOCALE_H
#define WIN_KLOCALE_H

#undef KDECORE_EXPORT
#define KDECORE_EXPORT

#undef KDE_DEPRECATED
#define KDE_DEPRECATED

#undef I18N_NOOP
#define I18N_NOOP(x) (x)
#include <qstring.h>
extern QString i18n(const QString &s, const QString & = QString::null);
extern QString locate(const QString &, const QString &);

#endif
