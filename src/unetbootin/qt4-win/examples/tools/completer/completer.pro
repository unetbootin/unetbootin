HEADERS   = dirmodel.h \
            mainwindow.h
SOURCES   = dirmodel.cpp \
            main.cpp \
            mainwindow.cpp
RESOURCES = completer.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tools/completer
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS completer.pro resources
sources.path = $$[QT_INSTALL_EXAMPLES]/tools/completer
INSTALLS += target sources
