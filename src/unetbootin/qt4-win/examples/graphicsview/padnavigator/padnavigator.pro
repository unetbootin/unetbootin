HEADERS += \
        panel.h \
        roundrectitem.h \
        splashitem.h

SOURCES += \
        panel.cpp \
        roundrectitem.cpp \
        splashitem.cpp \
        main.cpp

RESOURCES += \
        padnavigator.qrc

FORMS += \
        backside.ui

contains(QT_CONFIG, opengl):QT += opengl

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/padnavigator
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS padnavigator.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/padnavigator
INSTALLS += target sources
CONFIG += console
