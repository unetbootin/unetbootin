
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
    qdesigner_server.h \
    qdesigner_widgetbox.h \
    qdesigner_propertyeditor.h \
    qdesigner_objectinspector.h \
    qdesigner_actioneditor.h \
    qdesigner_actions.h \
    qdesigner_resourceeditor.h \
    saveformastemplate.h \
    newform.h \
    versiondialog.h \
    qdesigner_signalsloteditor.h \
    formwindowsettings.h \
    plugindialog.h \
    extra/cursor.h \
    extra/fov.h \
    extra/itemdialog.h \
    extra/oubliette.h \
    extra/oublietteplan.h \
    extra/oublietteview.h \
    designer_enums.h \
    preferencesdialog.h \
    preferences.h \
    assistantclient.h

SOURCES += main.cpp \
    qdesigner.cpp \
    qdesigner_toolwindow.cpp \
    qdesigner_formwindow.cpp \
    qdesigner_workbench.cpp \
    qdesigner_settings.cpp \
    qdesigner_server.cpp \
    qdesigner_widgetbox.cpp \
    qdesigner_propertyeditor.cpp \
    qdesigner_objectinspector.cpp \
    qdesigner_actioneditor.cpp \
    qdesigner_actions.cpp \
    qdesigner_resourceeditor.cpp \
    saveformastemplate.cpp \
    newform.cpp \
    versiondialog.cpp \
    qdesigner_signalsloteditor.cpp \
    plugindialog.cpp \
    formwindowsettings.cpp \
    extra/cursor.cpp \
    extra/fov.cpp \
    extra/itemdialog.cpp \
    extra/oubliette.cpp \
    extra/oublietteplan.cpp \
    extra/oublietteresource.cpp \
    extra/oublietteresource1.cpp \
    extra/oublietteresource2.cpp \
    extra/oublietteresource3.cpp \
    extra/oublietteview.cpp \
    preferencesdialog.cpp \
    preferences.cpp \
    assistantclient.cpp

PRECOMPILED_HEADER=qdesigner_pch.h

FORMS += \
    newform.ui \
    plugindialog.ui \
    saveformastemplate.ui \
    formwindowsettings.ui \
    preferencesdialog.ui

win32 {
   RC_FILE	= designer.rc
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
