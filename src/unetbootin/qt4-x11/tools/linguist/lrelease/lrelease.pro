TEMPLATE        = app
TARGET          = lrelease
DESTDIR         = ../../../bin

QT              -= gui

CONFIG          += qt warn_on console
CONFIG          -= app_bundle

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
SOURCES += main.cpp

include(../../../src/qt_professional.pri)
include(../shared/formats.pri)
include(../shared/proparser.pri)
include(../shared/translatortools.pri)

target.path=$$[QT_INSTALL_BINS]
INSTALLS        += target
