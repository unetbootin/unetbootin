include($$QT_SOURCE_TREE/tools/shared/fontpanel/fontpanel.pri)

TEMPLATE = app
LANGUAGE = C++
TARGET = assistant_adp

CONFIG += qt warn_on

unix:contains(QT_CONFIG, dbus):QT += dbus

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}
QT += xml network

PROJECTNAME = Assistant
DESTDIR = ../../../bin

FORMS += helpdialog.ui \
        mainwindow.ui \
        tabbedbrowser.ui \
        topicchooser.ui

SOURCES += main.cpp \
        helpwindow.cpp \
        topicchooser.cpp \
        docuparser.cpp \
        index.cpp \
        profile.cpp \
        config.cpp \
        helpdialog.cpp \
        mainwindow.cpp \
        tabbedbrowser.cpp \
        fontsettingsdialog.cpp

HEADERS += helpwindow.h \
        topicchooser.h \
        docuparser.h \
        index.h \
        profile.h \
        helpdialog.h \
        mainwindow.h \
        tabbedbrowser.h \
        config.h \
        fontsettingsdialog.h

RESOURCES += assistant.qrc

contains(QT_PRODUCT, OpenSource.*):DEFINES *= QT_OPENSOURCE
DEFINES += QT_KEYWORDS
#DEFINES +=  QT_PALMTOPCENTER_DOCS

win32 {
    !wince*:LIBS += -lshell32
    RC_FILE = assistant.rc
}

mac {
    ICON = assistant.icns
    TARGET = Assistant_adp
    QMAKE_INFO_PLIST = Info_mac.plist
}

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

TRANSLATIONS = assistant_de.ts

unix:!contains(QT_CONFIG, zlib):LIBS += -lz

contains(CONFIG, static): {
    win32 {
        exists($$[QT_INSTALL_PLUGINS]/imageformats/qjpeg.lib) {
            QTPLUGIN += qjpeg
            DEFINES += USE_STATIC_JPEG_PLUGIN            
        }
    } else {
        exists($$[QT_INSTALL_PLUGINS]/imageformats/qjpeg.a) {
            QTPLUGIN += qjpeg
            DEFINES += USE_STATIC_JPEG_PLUGIN            
        }        
    }
}
