CONFIG += assistant help x11inc
TARGET = qtdemo
DESTDIR = $$QT_BUILD_TREE/bin
OBJECTS_DIR = .obj
MOC_DIR = .moc
INSTALLS += target sources
QT += xml network

contains(QT_CONFIG, opengl) {
    DEFINES += QT_OPENGL_SUPPORT
    QT += opengl
}

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

RESOURCES = qtdemo.qrc
HEADERS = mainwindow.h \
    demoscene.h \
    demoitem.h \
    score.h \
    demoitemanimation.h \
    itemcircleanimation.h \
    demotextitem.h \
    headingitem.h \
    dockitem.h \
    scanitem.h \
    letteritem.h \
    examplecontent.h \
    menucontent.h \
    guide.h \
    guideline.h \
    guidecircle.h \
    menumanager.h \
    colors.h \
    textbutton.h \
    imageitem.h
SOURCES = main.cpp \
    demoscene.cpp \
    mainwindow.cpp \
    demoitem.cpp \
    score.cpp \
    demoitemanimation.cpp \
    itemcircleanimation.cpp \
    demotextitem.cpp \
    headingitem.cpp \
    dockitem.cpp \
    scanitem.cpp \
    letteritem.cpp \
    examplecontent.cpp \
    menucontent.cpp \
    guide.cpp \
    guideline.cpp \
    guidecircle.cpp \
    menumanager.cpp \
    colors.cpp \
    textbutton.cpp \
    imageitem.cpp

win32:RC_FILE = qtdemo.rc
mac {
ICON = qtdemo.icns
QMAKE_INFO_PLIST = Info_mac.plist
}

# install
target.path = $$[QT_INSTALL_BINS]
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES qtdemo.pro images xml *.ico *.icns *.rc *.plist
sources.path = $$[QT_INSTALL_DEMOS]/qtdemo

