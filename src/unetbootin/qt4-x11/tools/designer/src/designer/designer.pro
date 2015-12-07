
DESTDIR = ../../../../bin
QT += xml network script
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

INCLUDEPATH += \
    ../lib/sdk \
    ../lib/extension \
    ../lib/shared \
    ../lib/uilib \
    extra

LIBS += -L../../lib \
    -L../../../../lib \
    -lQtDesignerComponents \
    -lQtDesigner

RESOURCES += designer.qrc

contains(CONFIG, static) {
    DEFINES += QT_DESIGNER_STATIC
}

TARGET = designer

include($$QT_SOURCE_TREE/tools/shared/fontpanel/fontpanel.pri)
include($$QT_SOURCE_TREE/tools/shared/qttoolbardialog/qttoolbardialog.pri)

HEADERS += \
    qdesigner.h \
    qdesigner_toolwindow.h \
    qdesigner_formwindow.h \
    qdesigner_workbench.h \
    qdesigner_settings.h \
    qdesigner_actions.h \
    qdesigner_server.h \
    qdesigner_appearanceoptions.h \
    saveformastemplate.h \
    newform.h \
    versiondialog.h \
    plugindialog.h \
    designer_enums.h \
    appfontdialog.h \
    preferencesdialog.h \
    assistantclient.h \
    mainwindow.h

SOURCES += main.cpp \
    qdesigner.cpp \
    qdesigner_toolwindow.cpp \
    qdesigner_formwindow.cpp \
    qdesigner_workbench.cpp \
    qdesigner_settings.cpp \
    qdesigner_server.cpp \
    qdesigner_actions.cpp \
    qdesigner_appearanceoptions.cpp \
    saveformastemplate.cpp \
    newform.cpp \
    versiondialog.cpp \
    plugindialog.cpp \
    appfontdialog.cpp \
    preferencesdialog.cpp \
    assistantclient.cpp \
    mainwindow.cpp

PRECOMPILED_HEADER=qdesigner_pch.h

FORMS += plugindialog.ui \
    saveformastemplate.ui \
    preferencesdialog.ui \
    qdesigner_appearanceoptions.ui

win32 {
   RC_FILE      = designer.rc
}

mac {
    ICON = designer.icns
    QMAKE_INFO_PLIST = Info_mac.plist
    TARGET = Designer
}

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

include(../sharedcomponents.pri)

unix:!mac:LIBS += -lm
TRANSLATIONS = designer_de.ts
