TEMPLATE    = lib
TARGET      = qwebview
CONFIG     += qt warn_on plugin
QT         += webkit

include(../plugins.pri)
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# Input
SOURCES += qwebview_plugin.cpp
HEADERS += qwebview_plugin.h
RESOURCES += qwebview_plugin.qrc
