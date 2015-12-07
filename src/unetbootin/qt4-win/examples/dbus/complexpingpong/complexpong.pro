TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QT -= gui
CONFIG += qdbus

# Input
HEADERS += complexpong.h
SOURCES += complexpong.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/dbus/complexpingpong
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/dbus/complexpingpong
INSTALLS += target sources
