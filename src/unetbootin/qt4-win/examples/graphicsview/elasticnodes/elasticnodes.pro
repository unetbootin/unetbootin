HEADERS += \
        edge.h \
        node.h \
        graphwidget.h

SOURCES += \
        edge.cpp \
        main.cpp \
        node.cpp \
        graphwidget.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/elasticnodes
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS elasticnodes.pro 
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/elasticnodes
INSTALLS += target sources
