TARGET    = configure
DESTDIR   = ../..

CONFIG   += console flat
CONFIG   -= moc qt
DEFINES  = QT_NODLL QT_NO_CODECS QT_NO_TEXTCODEC QT_NO_UNICODETABLES QT_LITE_COMPONENT QT_NO_STL QT_NO_COMPRESS QT_BUILD_QMAKE QT_NO_THREAD QT_NO_QOBJECT _CRT_SECURE_NO_DEPRECATE


win32 : LIBS += -lole32 -ladvapi32
win32-msvc.net | win32-msvc2* : QMAKE_CXXFLAGS += /EHsc
win32-g++ : LIBS += -luuid

win32-msvc* {
    QMAKE_CFLAGS_RELEASE -= -MD
    QMAKE_CFLAGS_DEBUG -= -MDd
    QMAKE_CXXFLAGS_RELEASE -= -MD
    QMAKE_CXXFLAGS_DEBUG -= -MDd
}

PRECOMPILED_HEADER = configure_pch.h

INCPATH += $$QT_SOURCE_TREE/src/corelib/arch/generic \
           $$QT_SOURCE_TREE/src/corelib/global \
           $$QT_BUILD_TREE/include \
           $$QT_BUILD_TREE/include/QtCore \

HEADERS  = configureapp.h environment.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qbytearray.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qbytearraymatcher.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qchar.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qhash.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qlist.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qlocale.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qvector.h \
           $$QT_SOURCE_TREE/src/corelib/codecs/qtextcodec.h \
           $$QT_SOURCE_TREE/src/corelib/global/qglobal.h \
           $$QT_SOURCE_TREE/src/corelib/global/qnumeric.h \
           $$QT_SOURCE_TREE/src/corelib/io/qbuffer.h \
           $$QT_SOURCE_TREE/src/corelib/io/qdatastream.h \
           $$QT_SOURCE_TREE/src/corelib/io/qdir.h \
           $$QT_SOURCE_TREE/src/corelib/io/qdiriterator.h \
           $$QT_SOURCE_TREE/src/corelib/io/qfile.h \
           $$QT_SOURCE_TREE/src/corelib/io/qfileinfo.h \
           $$QT_SOURCE_TREE/src/corelib/io/qfsfileengine.h \
           $$QT_SOURCE_TREE/src/corelib/io/qfsfileengine_iterator_p.h \
           $$QT_SOURCE_TREE/src/corelib/io/qiodevice.h \
           $$QT_SOURCE_TREE/src/corelib/io/qtextstream.h \
           $$QT_SOURCE_TREE/src/corelib/io/qtemporaryfile.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qbitarray.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qdatetime.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qmap.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qregexp.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qstring.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qstringlist.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qstringmatcher.h \
           $$QT_SOURCE_TREE/src/corelib/tools/qunicodetables_p.h


SOURCES  = main.cpp configureapp.cpp environment.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qbytearray.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qbytearraymatcher.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qchar.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qhash.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qlistdata.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qlocale.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qvector.cpp \
           $$QT_SOURCE_TREE/src/corelib/codecs/qtextcodec.cpp \
           $$QT_SOURCE_TREE/src/corelib/global/qglobal.cpp \
           $$QT_SOURCE_TREE/src/corelib/global/qnumeric.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qbuffer.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qdatastream.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qdir.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qdiriterator.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qfile.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qfileinfo.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qabstractfileengine.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qfsfileengine.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qfsfileengine_iterator.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qfsfileengine_iterator_win.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qiodevice.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qtextstream.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qtemporaryfile.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qbitarray.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qdatetime.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qmap.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qregexp.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qstring.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qstringlist.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qstringmatcher.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qunicodetables.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qvsnprintf.cpp \
           $$QT_SOURCE_TREE/src/corelib/kernel/qvariant.cpp \
           $$QT_SOURCE_TREE/src/corelib/io/qurl.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qline.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qsize.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qpoint.cpp \
           $$QT_SOURCE_TREE/src/corelib/tools/qrect.cpp \
           $$QT_SOURCE_TREE/src/corelib/kernel/qmetatype.cpp \
           $$QT_SOURCE_TREE/src/corelib/global/qmalloc.cpp

win32:SOURCES += $$QT_SOURCE_TREE/src/corelib/io/qfsfileengine_win.cpp

exists(../../util/configure/commercial/tools.h):exists(../../util/configure/commercial/tools.cpp) {
    DEFINES += COMMERCIAL_VERSION
    HEADERS += ../../util/configure/commercial/tools.h
    SOURCES += ../../util/configure/commercial/tools.cpp
    INCLUDEPATH += ../../util/configure/commercial
} else {
    DEFINES += GPL_VERSION
}

INCLUDEPATH += $$QT_SOURCE_TREE/src/corelib/arch/generic \
               $$QT_SOURCE_TREE/include/QtCore \
