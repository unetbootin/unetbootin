include($$QT_SOURCE_TREE/tools/shared/fontpanel/fontpanel.pri)

TEMPLATE = app
LANGUAGE = C++
TARGET = assistant

DEFINES += QT_CLUCENE_SUPPORT

#contains(QT_CONFIG, webkit) {
#    DEFINES += USE_WEBKIT
#    QT += webkit
#}

CONFIG += qt warn_on help

QT += network

PROJECTNAME = Assistant
DESTDIR = ../../../../bin

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

HEADERS += helpviewer.h \
           mainwindow.h \
           indexwindow.h \
           topicchooser.h \
           contentwindow.h \
           searchwidget.h \
           preferencesdialog.h \
           filternamedialog.h \
           centralwidget.h \
           installdialog.h \
           bookmarkdialog.h \
           bookmarkwidget.h \
           remotecontrol.h \
           cmdlineparser.h \
           aboutdialog.h \
           qtdocinstaller.h
           
win32 {
    HEADERS += remotecontrol_win.h
}

SOURCES += helpviewer.cpp \
           main.cpp \
           mainwindow.cpp \
           indexwindow.cpp \
           topicchooser.cpp \
           contentwindow.cpp \
           searchwidget.cpp \
           preferencesdialog.cpp \
           filternamedialog.cpp \
           centralwidget.cpp \
           installdialog.cpp \
           bookmarkdialog.cpp \
           bookmarkwidget.cpp \
           remotecontrol.cpp \
           cmdlineparser.cpp \
           aboutdialog.cpp \
           qtdocinstaller.cpp

FORMS += topicchooser.ui \
         preferencesdialog.ui \
         filternamedialog.ui \
         installdialog.ui \
         bookmarkdialog.ui

RESOURCES += assistant.qrc

win32 {
    !wince*:LIBS += -lshell32
    RC_FILE = assistant.rc
}

mac {
    ICON = assistant.icns
    TARGET = Assistant
    QMAKE_INFO_PLIST = Info_mac.plist
}

contains(CONFIG, static): {
    win32 {
        exists($$[QT_INSTALL_PLUGINS]/sqldrivers/qsqlite.lib) {
            QTPLUGIN += qsqlite
            DEFINES += USE_STATIC_SQLITE_PLUGIN
        }
    } else {
        exists($$[QT_INSTALL_PLUGINS]/sqldrivers/libqsqlite.a) {
            QTPLUGIN += qsqlite
            DEFINES += USE_STATIC_SQLITE_PLUGIN
        }        
    }
}

