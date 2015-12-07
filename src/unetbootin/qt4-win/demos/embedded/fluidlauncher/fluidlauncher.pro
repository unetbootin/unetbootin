TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QT += xml

# Input
HEADERS += \
           demoapplication.h \
           fluidlauncher.h \
           pictureflow.h \
           slideshow.h

SOURCES += \
           demoapplication.cpp \
           fluidlauncher.cpp \
           main.cpp \
           pictureflow.cpp \
           slideshow.cpp

embedded:
{
    target.path = $$[QT_INSTALL_DEMOS]/embedded/fluidlauncher
    sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro *.html config.xml screenshots slides
    sources.path = $$[QT_INSTALL_DEMOS]/embedded/fluidlauncher
    INSTALLS += target sources
}

wince*:
{
    QT += svg

    BUILD_DIR = release
    if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
        BUILD_DIR = debug
    }

    executables.sources = \
        $$QT_BUILD_TREE/demos/embedded/embeddedsvgviewer/$${BUILD_DIR}/embeddedsvgviewer.exe \
        $$QT_BUILD_TREE/demos/embedded/styledemo/$${BUILD_DIR}/styledemo.exe \
        $$QT_BUILD_TREE/demos/deform/$${BUILD_DIR}/deform.exe \
        $$QT_BUILD_TREE/demos/pathstroke/$${BUILD_DIR}/pathstroke.exe \
        $$QT_BUILD_TREE/examples/graphicsview/elasticnodes/$${BUILD_DIR}/elasticnodes.exe \
        $$QT_BUILD_TREE/examples/widgets/wiggly/$${BUILD_DIR}/wiggly.exe \
        $$QT_BUILD_TREE/examples/painting/concentriccircles/$${BUILD_DIR}/concentriccircles.exe

    executables.path = .

    files.sources = $$PWD/screenshots $$PWD/slides $$PWD/../embeddedsvgviewer/shapes.svg
    files.path = .

    config.sources = $$PWD/config_wince/config.xml
    config.path = .

    DEPLOYMENT += config files executables

    DEPLOYMENT_PLUGIN += qgif qjpeg qmng qsvg
}
