TEMPLATE = app
CONFIG += console qt_no_compat_warning
CONFIG -= app_bundle
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

QT += xml qt3support

DESTDIR = ../../../bin

include(../uic/uic.pri)
include(../uic/cpp/cpp.pri)

INCLUDEPATH += .

HEADERS += ui3reader.h \
           parser.h \
           domtool.h \
           widgetinfo.h \
           qt3to4.h \
           uic.h

SOURCES += main.cpp \
           ui3reader.cpp \
           parser.cpp \
           domtool.cpp \
           object.cpp \
           subclassing.cpp \
           form.cpp \
           converter.cpp \
           widgetinfo.cpp \
           embed.cpp \
           qt3to4.cpp \
           deps.cpp \
           uic.cpp

DEFINES -= QT_COMPAT_WARNINGS
DEFINES += QT_COMPAT QT_UIC3

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target
