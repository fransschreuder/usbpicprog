## adjust or comment the following lines to reflect your configuration
DEFINES      += HAVE_USB
unix:DEFINES += HAVE_PPDEV
unix:DEFINES += HAVE_READLINE
## for directories with a space in their name, use the short filename format
## for e.g. "Program Files" should be "Progra~1"
win32: LIBUSB_PATH = "C:\Progra~1\LibUSB-Win32"

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
