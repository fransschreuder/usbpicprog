## adjust or comment the following lines to reflect your configuration
DEFINES      += HAVE_USB
unix:DEFINES += HAVE_PPDEV
unix:DEFINES += HAVE_READLINE
win32: LIBUSB_PATH = "C:\Program Files\LibUSB-Win32"

###############################################################################
DEFINES      += NO_KDE QT_NO_ASCII_CAST
OBJECTS_DIR   = .libs
MOC_DIR       = .libs
QT            = qt3support core xml network
CONFIG       += qt thread warn_on console
unix:CONFIG  += release #debug
win32:CONFIG += release
INCLUDEPATH  += $${STOPDIR}/src $${STOPDIR}
DEPENDPATH   += $${STOPDIR}/app.pro $${STOPDIR}/all.pro $${STOPDIR}/lib.pro
