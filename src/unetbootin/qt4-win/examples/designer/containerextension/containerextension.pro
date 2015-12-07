#! [0]
TEMPLATE = lib
#! [0]
TARGET   = $$qtLibraryTarget($$TARGET)
#! [1]
CONFIG  += designer plugin debug_and_release
#! [1]
QTDIR_build:DESTDIR  = $$QT_BUILD_TREE/plugins/designer

#! [2]
HEADERS += multipagewidget.h \
           multipagewidgetplugin.h \ 
           multipagewidgetcontainerextension.h \
           multipagewidgetextensionfactory.h 

SOURCES += multipagewidget.cpp \
           multipagewidgetplugin.cpp \
           multipagewidgetcontainerextension.cpp \
           multipagewidgetextensionfactory.cpp 
#! [2]

# install
target.path = $$[QT_INSTALL_PLUGINS]/designer
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/designer/containerextension
INSTALLS += target sources
