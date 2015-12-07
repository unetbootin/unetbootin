#! [0] #! [1]
CONFIG      += designer plugin debug_and_release
#! [0]
TARGET      = $$qtLibraryTarget($$TARGET)
#! [2]
TEMPLATE    = lib
#! [1] #! [2]
QTDIR_build:DESTDIR     = $$QT_BUILD_TREE/plugins/designer

#! [3]
HEADERS     = analogclock.h \
              customwidgetplugin.h
SOURCES     = analogclock.cpp \
              customwidgetplugin.cpp
#! [3]

# install
target.path = $$[QT_INSTALL_PLUGINS]/designer
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/designer/customwidgetplugin
INSTALLS += target sources
