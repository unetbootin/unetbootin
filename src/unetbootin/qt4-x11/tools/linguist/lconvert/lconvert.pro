
TEMPLATE = app
TARGET = lconvert
DESTDIR         = ../../../bin

QT -= gui

CONFIG          += qt warn_on console
CONFIG          -= app_bundle

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

include(../shared/formats.pri)

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
SOURCES += main.cpp

target.path=$$[QT_INSTALL_BINS]
INSTALLS        += target
