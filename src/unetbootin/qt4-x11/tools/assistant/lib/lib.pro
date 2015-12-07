QT += sql xml network
TEMPLATE = lib
TARGET = QtHelp
DEFINES += QHELP_LIB QT_CLUCENE_SUPPORT
CONFIG += qt warn_on

include(../../../src/qbase.pri)

QMAKE_TARGET_PRODUCT = Help
QMAKE_TARGET_DESCRIPTION = Help application framework.
DEFINES -= QT_ASCII_CAST_WARNINGS

qclucene = QtCLucene$${QT_LIBINFIX}
if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {
    mac:qclucene = $${qclucene}_debug
    win32:qclucene = $${qclucene}d
}
linux-lsb-g++:LIBS += --lsb-shared-libs=$$qclucene
unix:QMAKE_PKGCONFIG_REQUIRES += QtNetwork QtSql QtXml
LIBS += -l$$qclucene
unix:QMAKE_PKGCONFIG_REQUIRES += QtNetwork QtSql QtXml

RESOURCES += helpsystem.qrc

SOURCES += qhelpenginecore.cpp \
           qhelpengine.cpp \
           qhelpdbreader.cpp \
           qhelpcontentwidget.cpp \
           qhelpindexwidget.cpp \
           qhelpgenerator.cpp \
           qhelpdatainterface.cpp \
           qhelpprojectdata.cpp \
           qhelpcollectionhandler.cpp \
           qhelpsearchengine.cpp \
           qhelpsearchquerywidget.cpp \
           qhelpsearchresultwidget.cpp \
           qhelpsearchindex_default.cpp \
           qhelpsearchindexwriter_default.cpp \
           qhelpsearchindexreader_default.cpp

# access to clucene
SOURCES += qhelpsearchindexwriter_clucene.cpp \
           qhelpsearchindexreader_clucene.cpp

HEADERS += qhelpenginecore.h \
           qhelpengine.h \
           qhelpengine_p.h \
           qhelp_global.h \
           qhelpdbreader_p.h \
           qhelpcontentwidget.h \
           qhelpindexwidget.h \
           qhelpgenerator_p.h \
           qhelpdatainterface_p.h \
           qhelpprojectdata_p.h \
           qhelpcollectionhandler_p.h \
           qhelpsearchengine.h \
           qhelpsearchquerywidget.h \
           qhelpsearchresultwidget.h \
           qhelpsearchindex_default_p.h \
           qhelpsearchindexwriter_default_p.h \
           qhelpsearchindexreader_default_p.h

# access to clucene
HEADERS += qhelpsearchindexwriter_clucene_p.h \
           qhelpsearchindexreader_clucene_p.h
