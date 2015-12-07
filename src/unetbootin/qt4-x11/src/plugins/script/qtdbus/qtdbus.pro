TARGET  = qtscriptdbus
include(../../qpluginbase.pri)
QT = core gui script
CONFIG += qdbus

SOURCES += main.cpp
HEADERS += main.h

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/script
target.path += $$[QT_INSTALL_PLUGINS]/script
INSTALLS += target
