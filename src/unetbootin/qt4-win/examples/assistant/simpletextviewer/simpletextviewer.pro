CONFIG += assistant

QT += network

HEADERS       = mainwindow.h \
                findfiledialog.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                findfiledialog.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/assistant/simpletextviewer
sources.files = $$SOURCES $$HEADERS $$RESOURCES documentation *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/assistant/simpletextviewer
INSTALLS += target sources

