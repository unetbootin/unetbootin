QT += network

HEADERS       = mainwindow.h \
                findfiledialog.h \
                assistant.h \
                textedit.h
SOURCES       = main.cpp \
                mainwindow.cpp \
                findfiledialog.cpp \
                assistant.cpp \
                textedit.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/help/simpletextviewer
sources.files = $$SOURCES $$HEADERS $$RESOURCES documentation *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/help/simpletextviewer
INSTALLS += target sources

