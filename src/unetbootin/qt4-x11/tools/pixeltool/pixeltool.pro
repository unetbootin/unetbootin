TEMPLATE = app
CONFIG  += qt warn_on
QT += network

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

DESTDIR     = ../../bin

DEPENDPATH += .
INCLUDEPATH += .
TARGET = pixeltool

mac {
    QMAKE_INFO_PLIST=Info_mac.plist
}

# Input
SOURCES += main.cpp qpixeltool.cpp
HEADERS += qpixeltool.h

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target
