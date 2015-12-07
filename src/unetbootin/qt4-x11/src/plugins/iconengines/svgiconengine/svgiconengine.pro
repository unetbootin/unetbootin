TARGET  = qsvgicon
include(../../qpluginbase.pri)

HEADERS += qsvgiconengine.h
SOURCES += main.cpp \
           qsvgiconengine.cpp
QT += xml svg

QTDIR_build:DESTDIR  = $$QT_BUILD_TREE/plugins/iconengines
target.path += $$[QT_INSTALL_PLUGINS]/iconengines
INSTALLS += target
