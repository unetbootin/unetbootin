#! [0]
TEMPLATE = lib
#! [0]
TARGET   = $$qtLibraryTarget($$TARGET)
#! [1]
CONFIG  += designer plugin debug_and_release
#! [1]
QTDIR_build:DESTDIR  = $$QT_BUILD_TREE/plugins/designer

#! [2]
HEADERS += tictactoe.h \
           tictactoedialog.h \
           tictactoeplugin.h \
           tictactoetaskmenu.h
SOURCES += tictactoe.cpp \
           tictactoedialog.cpp \
           tictactoeplugin.cpp \
           tictactoetaskmenu.cpp
#! [2]

# install
target.path = $$[QT_INSTALL_PLUGINS]/designer
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/designer/taskmenuextension
INSTALLS += target sources
