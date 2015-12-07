TARGET  = qsvg
include(../../qpluginbase.pri)

HEADERS += qsvgiohandler.h
SOURCES += main.cpp \
           qsvgiohandler.cpp
QT += xml svg

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/imageformats
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target
