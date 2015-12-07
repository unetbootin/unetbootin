
TEMPLATE = lib
QT	+= opengl
CONFIG  += qt warn_on plugin
DESTDIR =
TARGET = view3d

include(../../plugins.pri)
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# Input
SOURCES += view3d.cpp view3d_tool.cpp view3d_plugin.cpp
HEADERS += view3d.h view3d_tool.h view3d_plugin.h view3d_global.h

