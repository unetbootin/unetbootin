HEADERS     = draglabel.h \
              dragwidget.h
RESOURCES   = fridgemagnets.qrc
SOURCES     = draglabel.cpp \
              dragwidget.cpp \
              main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/draganddrop/fridgemagnets
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro *.txt
sources.path = $$[QT_INSTALL_EXAMPLES]/draganddrop/fridgemagnets
INSTALLS += target sources
