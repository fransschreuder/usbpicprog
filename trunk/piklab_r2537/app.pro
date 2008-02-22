include($${STOPDIR}/all.pro)

TEMPLATE           = app
unix:QMAKE_CLEAN  += $${TARGET}
win32:DESTDIR      = .\
win32:QMAKE_CLEAN += $${TARGET}.exe
