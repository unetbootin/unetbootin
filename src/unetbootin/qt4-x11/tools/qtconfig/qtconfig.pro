TEMPLATE = app
CONFIG        += qt warn_on x11
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}
LANGUAGE = C++
QT += qt3support

contains(QT_CONFIG, gstreamer):LIBS += $$QT_LIBS_GSTREAMER -lgstinterfaces-0.10 -lgstvideo-0.10 -lgstbase-0.10
contains(QT_CONFIG, gstreamer):QMAKE_CXXFLAGS += $$QT_CFLAGS_GSTREAMER
contains(QT_CONFIG, gstreamer):QT += phonon
SOURCES        += colorbutton.cpp main.cpp previewframe.cpp previewwidget.cpp mainwindow.cpp paletteeditoradvanced.cpp \
    mainwindowbase.cpp paletteeditoradvancedbase.cpp previewwidgetbase.cpp
HEADERS        += colorbutton.h previewframe.h previewwidget.h mainwindow.h paletteeditoradvanced.h \
    mainwindowbase.h paletteeditoradvancedbase.h previewwidgetbase.h

FORMS        = mainwindowbase.ui paletteeditoradvancedbase.ui previewwidgetbase.ui
RESOURCES    = qtconfig.qrc

PROJECTNAME        = Qt Configuration
TARGET                = qtconfig
DESTDIR                = ../../bin

target.path=$$[QT_INSTALL_BINS]
INSTALLS        += target
INCLUDEPATH        += .
DBFILE                 = qtconfig.db
