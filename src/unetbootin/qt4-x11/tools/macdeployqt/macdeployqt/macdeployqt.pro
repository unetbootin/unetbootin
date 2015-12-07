TEMPLATE = app
TARGET = macdeployqt
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR     = ../../../bin

# Input
SOURCES += main.cpp ../shared/shared.cpp
CONFIG += qt warn_on
CONFIG -= app_bundle

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target
