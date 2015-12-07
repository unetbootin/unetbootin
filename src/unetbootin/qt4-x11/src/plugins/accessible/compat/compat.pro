TARGET  = qtaccessiblecompatwidgets
CONFIG += qt_no_compat_warning
include(../../qpluginbase.pri)
include (../qaccessiblebase.pri)

QTDIR_build:DESTDIR  = $$QT_BUILD_TREE/plugins/accessible

QTDIR_build:REQUIRES += "contains(QT_CONFIG, accessibility)"

QT += qt3support

SOURCES  += main.cpp \
            q3simplewidgets.cpp \
            q3complexwidgets.cpp \
            qaccessiblecompat.cpp

HEADERS  += qaccessiblecompat.h \
            q3complexwidgets.h \
            q3simplewidgets.h

