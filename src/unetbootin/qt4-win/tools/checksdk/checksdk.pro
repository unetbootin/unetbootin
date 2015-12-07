TEMPLATE = app
DESTDIR = ../../bin
TARGET = checksdk
DEPENDPATH += .
INCLUDEPATH += .
QT = 
CONFIG += console

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

DEFINES        += QT_BOOTSTRAPPED QT_NO_CODECS QT_LITE_UNICODE QT_NO_LIBRARY \
                  QT_NO_STL QT_NO_COMPRESS QT_NO_DATASTREAM  \
                  QT_NO_TEXTCODEC QT_NO_UNICODETABLES QT_NO_THREAD \
                  QT_NO_SYSTEMLOCALE QT_NO_GEOM_VARIANT \
                  QT_NODLL QT_NO_QOBJECT 
                  
INCLUDEPATH = \
              $$QT_BUILD_TREE/src/corelib/arch \
              $$QT_BUILD_TREE/include \
              $$QT_BUILD_TREE/include/QtCore

DEPENDPATH += $$INCLUDEPATH $$QT_BUILD_TREE/src/corelib/base $$QT_BUILD_TREE/src/corelib/tools $$QT_BUILD_TREE/src/corelib/io

# Input
SOURCES += \
           main.cpp \
           cesdkhandler.cpp

HEADERS += \
           cesdkhandler.h

# Bootstrapped Input
SOURCES += \
           $$QT_SOURCE_TREE/src/corelib/tools/qstring.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qstringlist.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qfile.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qdir.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qfsfileengine.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qabstractfileengine.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qfsfileengine_win.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qfsfileengine_iterator.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qfsfileengine_iterator_win.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qfileinfo.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qtemporaryfile.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qdiriterator.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qiodevice.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qbuffer.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qtextstream.cpp \           
           $$QT_SOURCE_TREE/src/corelib/tools/qdatetime.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qlocale.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qbytearray.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qbytearraymatcher.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qvector.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qvsnprintf.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qlistdata.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qhash.cpp \
           $$QT_SOURCE_TREE/src/corelib/global/qglobal.cpp \
           $$QT_SOURCE_TREE/src/corelib/global/qmalloc.cpp \
           $$QT_SOURCE_TREE/src/corelib/global/qnumeric.cpp \
           $$QT_SOURCE_TREE/src/corelib/xml/qxmlstream.cpp \
           $$QT_SOURCE_TREE/src/corelib/xml/qxmlutils.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qregexp.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qmap.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qbitarray.cpp \           
           $$QT_BUILD_TREE/src/corelib/global/qconfig.cpp
