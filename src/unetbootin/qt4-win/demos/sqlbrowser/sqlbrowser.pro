TEMPLATE        = app
TARGET          = sqlbrowser

QT              += sql

HEADERS         = browser.h connectionwidget.h qsqlconnectiondialog.h
SOURCES         = main.cpp browser.cpp connectionwidget.cpp qsqlconnectiondialog.cpp

FORMS           = browserwidget.ui qsqlconnectiondialog.ui
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# install
target.path = $$[QT_INSTALL_DEMOS]/sqlbrowser
sources.files = $$SOURCES $$HEADERS $$FORMS *.pro
sources.path = $$[QT_INSTALL_DEMOS]/sqlbrowser
INSTALLS += target sources

wince*: {
    CONFIG(debug, debug|release):sqlPlugins.sources = $$QT_BUILD_TREE/plugins/sqldrivers/*d4.dll
    CONFIG(release, debug|release):sqlPlugins.sources = $$QT_BUILD_TREE/plugins/sqldrivers/*[^d]4.dll
    sqlPlugins.path = sqldrivers
    DEPLOYMENT += sqlPlugins
}