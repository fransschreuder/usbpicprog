/***************************************************************************
 *   Copyright (C) 2006 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef GLOBAL_H
#define GLOBAL_H

#include <qglobal.h>

#if QT_VERSION<0x040000
#  include <qapplication.h>
#  include <qvaluelist.h>
#  include <qvaluevector.h>
#  include <qmemarray.h>
#  include "common/common/qflags.h"
#  define qMax QMAX
#  define qMin QMIN
#  include <qurl.h>
#  define Q3Url QUrl
#  include <qguardedptr.h>
#else
#  include <qcoreapplication.h>
#  include <Qt3Support/Q3ValueList>
#  define QValueList Q3ValueList
#  include <Qt3Support/Q3ValueVector>
#  define QValueVector Q3ValueVector
#  include <Qt3Support/Q3MemArray>
#  define QMemArray Q3MemArray
#  define qHeapSort qSort
#  include <Qt3Support/Q3Url>
#  include <Qt3Support/Q3MimeSourceFactory>
#  define QMimeSourceFactory Q3MimeSourceFactory
#  include <qpointer.h>
#  define QGuardedPtr QPointer
#endif

#if defined(NO_KDE)
#  include "qt_config.h"
#  include "common/nokde/nokde_kurl.h"
#  include "common/nokde/nokde_klocale.h"
#  include "common/nokde/nokde_kaboutdata.h"
#  include "common/nokde/nokde_kcmdlineargs.h"
#else
#  include "config.h"
#  include <kapplication.h>
#  include <klocale.h>
#  include <kaboutdata.h>
#  include <kcmdlineargs.h>
#endif

#if defined(Q_OS_WIN)
#  include <windows.h>
#endif

#endif
