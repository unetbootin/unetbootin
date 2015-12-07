TEMPLATE    = lib
TARGET      = phononwidgets
CONFIG     += qt warn_on plugin
QT         += phonon

include(../plugins.pri)
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# Input
SOURCES += videoplayerplugin.cpp \
    videoplayertaskmenu.cpp \
    seeksliderplugin.cpp \
    volumesliderplugin.cpp \
    phononcollection.cpp

HEADERS += videoplayerplugin.h \
    videoplayertaskmenu.h \
    seeksliderplugin.h \
    volumesliderplugin.h

RESOURCES += phononwidgets.qrc
