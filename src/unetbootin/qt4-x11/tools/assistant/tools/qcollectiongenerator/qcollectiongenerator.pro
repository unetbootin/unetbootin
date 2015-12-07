QT += xml network
TEMPLATE = app
DESTDIR = ../../../../bin
TARGET = qcollectiongenerator
CONFIG += qt warn_on help console
CONFIG -= app_bundle

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

SOURCES += ../shared/helpgenerator.cpp \
           main.cpp

HEADERS += ../shared/helpgenerator.h
