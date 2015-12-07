TEMPLATE = app
TARGET = browser
QT += webkit network

CONFIG += qt warn_on
contains(QT_BUILD_PARTS, tools): CONFIG += uitools
else: DEFINES += QT_NO_UITOOLS

FORMS += \
    addbookmarkdialog.ui \
    bookmarks.ui \
    cookies.ui \
    cookiesexceptions.ui \
    downloaditem.ui \
    downloads.ui \
    history.ui \
    passworddialog.ui \
    proxy.ui \
    settings.ui

HEADERS += \
    autosaver.h \
    bookmarks.h \
    browserapplication.h \
    browsermainwindow.h \
    chasewidget.h \
    cookiejar.h \
    downloadmanager.h \
    edittableview.h \
    edittreeview.h \
    history.h \
    modelmenu.h \
    networkaccessmanager.h \
    searchlineedit.h \
    settings.h \
    squeezelabel.h \
    tabwidget.h \
    toolbarsearch.h \
    urllineedit.h \
    webview.h \
    xbel.h

SOURCES += \
    autosaver.cpp \
    bookmarks.cpp \
    browserapplication.cpp \
    browsermainwindow.cpp \
    chasewidget.cpp \
    cookiejar.cpp \
    downloadmanager.cpp \
    edittableview.cpp \
    edittreeview.cpp \
    history.cpp \
    modelmenu.cpp \
    networkaccessmanager.cpp \
    searchlineedit.cpp \
    settings.cpp \
    squeezelabel.cpp \
    tabwidget.cpp \
    toolbarsearch.cpp \
    urllineedit.cpp \
    webview.cpp \
    xbel.cpp \
    main.cpp

RESOURCES += data/data.qrc htmls/htmls.qrc

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

win32 {
   RC_FILE = browser.rc
}

mac {
    ICON = browser.icns
    QMAKE_INFO_PLIST = Info_mac.plist
    TARGET = Browser
}

# install
target.path = $$[QT_INSTALL_DEMOS]/browser
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.plist *.icns *.ico *.rc *.pro *.html *.doc images htmls
sources.path = $$[QT_INSTALL_DEMOS]/browser
INSTALLS += target sources
