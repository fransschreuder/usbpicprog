TEMPLATE = app
QT = gui core
CONFIG += qt \
 warn_on \
 console \
 build_all \
 debug_and_release \
 qtestlib
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/mainwindow.ui
HEADERS = src/mainwindowimpl.h src/hexparse.h
SOURCES = src/mainwindowimpl.cpp src/main.cpp src/hexparse.cpp
INCLUDEPATH += /home/damunix/Desktop/piklab/trunk/piklab/src
