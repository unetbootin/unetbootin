TEMPLATE = lib
TARGET = bootstrap
CONFIG += staticlib

CONFIG += console qtinc 
CONFIG -= qt
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}
mac:CONFIG -= app_bundle incremental

DEFINES += \
        QT_BOOTSTRAPPED \
        QT_LITE_UNICODE \
        QT_NO_CAST_FROM_ASCII \
        QT_NO_CAST_TO_ASCII \
        QT_NO_CODECS \
        QT_NO_DATASTREAM \
        QT_NO_GEOM_VARIANT \
        QT_NO_LIBRARY \
        QT_NO_QOBJECT \
        QT_NO_STL \
        QT_NO_SYSTEMLOCALE \
        QT_NO_TEXTSTREAM \
        QT_NO_THREAD \
        QT_NO_UNICODETABLES \
        QT_NO_USING_NAMESPACE
win32:DEFINES += QT_NODLL

INCLUDEPATH += $$QT_BUILD_TREE/include \
            $$QT_BUILD_TREE/include/QtCore \
            $$QT_BUILD_TREE/include/QtXml \

DEPENDPATH += $$INCLUDEPATH \
              ../../corelib/global \
              ../../corelib/kernel \
              ../../corelib/tools \
              ../../corelib/io \
              ../../corelib/codecs \
              ../../xml

SOURCES += \
           ../../corelib/codecs/qisciicodec.cpp \
           ../../corelib/codecs/qlatincodec.cpp \
           ../../corelib/codecs/qsimplecodec.cpp \
           ../../corelib/codecs/qtextcodec.cpp \
           ../../corelib/codecs/qtsciicodec.cpp \
           ../../corelib/codecs/qutfcodec.cpp \
           ../../corelib/global/qglobal.cpp \
           ../../corelib/global/qmalloc.cpp \
           ../../corelib/global/qnumeric.cpp \
           ../../corelib/io/qabstractfileengine.cpp \
           ../../corelib/io/qbuffer.cpp \
           ../../corelib/io/qdir.cpp \
           ../../corelib/io/qdiriterator.cpp \
           ../../corelib/io/qfile.cpp \
           ../../corelib/io/qfileinfo.cpp \
           ../../corelib/io/qfsfileengine.cpp \
           ../../corelib/io/qfsfileengine_iterator.cpp \
           ../../corelib/io/qiodevice.cpp \
           ../../corelib/io/qtemporaryfile.cpp \
           ../../corelib/io/qtextstream.cpp \
           ../../corelib/io/qurl.cpp \
           ../../corelib/kernel/qmetatype.cpp \
           ../../corelib/kernel/qvariant.cpp \
           ../../corelib/tools/qbitarray.cpp \
           ../../corelib/tools/qbytearray.cpp \
           ../../corelib/tools/qbytearraymatcher.cpp \
           ../../corelib/tools/qdatetime.cpp \
           ../../corelib/tools/qhash.cpp \
           ../../corelib/tools/qlistdata.cpp \
           ../../corelib/tools/qlocale.cpp \
           ../../corelib/tools/qmap.cpp \
           ../../corelib/tools/qregexp.cpp \
           ../../corelib/tools/qstring.cpp \
           ../../corelib/tools/qstringlist.cpp \
           ../../corelib/tools/qvector.cpp \
           ../../corelib/tools/qvsnprintf.cpp \
           ../../corelib/xml/qxmlutils.cpp \
	   ../../corelib/xml/qxmlstream.cpp \
           ../../xml/dom/qdom.cpp \
           ../../xml/sax/qxml.cpp

unix:SOURCES += ../../corelib/io/qfsfileengine_unix.cpp \
                ../../corelib/io/qfsfileengine_iterator_unix.cpp

win32:SOURCES += ../../corelib/io/qfsfileengine_win.cpp \
                 ../../corelib/io/qfsfileengine_iterator_win.cpp

macx: {
   QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.3 #enables weak linking for 10.3 (exported)
   SOURCES += ../../corelib/kernel/qcore_mac.cpp
   LIBS += -framework CoreServices
}

contains(QT_CONFIG, zlib) {
   INCLUDEPATH += ../../3rdparty/zlib
   SOURCES+= \
        ../3rdparty/zlib/adler32.c \
        ../3rdparty/zlib/compress.c \
        ../3rdparty/zlib/crc32.c \
        ../3rdparty/zlib/deflate.c \
        ../3rdparty/zlib/gzio.c \
        ../3rdparty/zlib/inffast.c \
        ../3rdparty/zlib/inflate.c \
        ../3rdparty/zlib/inftrees.c \
        ../3rdparty/zlib/trees.c \
        ../3rdparty/zlib/uncompr.c \
        ../3rdparty/zlib/zutil.c
}

lib.CONFIG = dummy_install
INSTALLS += lib
