include($${STOPDIR}/all.pro)

TEMPLATE           = lib
CONFIG            += staticlib
win32:DESTDIR      = .\
unix:CLEAN_FILES  += $${DESTDIR_TARGET}
win32:QMAKE_CLEAN += $${DESTDIR_TARGET} #not working ??