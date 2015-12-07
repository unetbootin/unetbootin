CONFIG += depend_includepath

QMAKE_INCREMENTAL =
SKIP_DEPENDS += qconfig.h qmodules.h
DEFINES += QT_NO_TEXTCODEC QT_NO_LIBRARY QT_NO_STL QT_NO_COMPRESS QT_NO_UNICODETABLES \
           QT_NO_GEOM_VARIANT QT_NO_DATASTREAM

#qmake code
SOURCES += project.cpp property.cpp main.cpp generators/makefile.cpp \
           generators/unix/unixmake2.cpp generators/unix/unixmake.cpp meta.cpp \
           option.cpp generators/win32/winmakefile.cpp generators/win32/mingw_make.cpp \
           generators/makefiledeps.cpp generators/metamakefile.cpp generators/mac/pbuilder_pbx.cpp \
           generators/xmloutput.cpp generators/win32/borland_bmake.cpp \
           generators/win32/msvc_nmake.cpp generators/projectgenerator.cpp \
           generators/win32/msvc_dsp.cpp generators/win32/msvc_vcproj.cpp \
           generators/win32/msvc_objectmodel.cpp
HEADERS += project.h property.h generators/makefile.h \
           generators/unix/unixmake.h meta.h option.h cachekeys.h \
           generators/win32/winmakefile.h generators/projectgenerator.h \
           generators/makefiledeps.h generators/metamakefile.h generators/mac/pbuilder_pbx.h \
           generators/xmloutput.h generators/win32/borland_bmake.h generators/win32/msvc_nmake.h \
           generators/win32/msvc_dsp.h generators/win32/msvc_vcproj.h \
           generators/win32/mingw_make.h generators/win32/msvc_objectmodel.h

contains(QT_EDITION, OpenSource) {
   DEFINES += QMAKE_OPENSOURCE_EDITION
}

bootstrap { #Qt code
   DEFINES+=QT_NODLL QT_NO_THREAD
   SOURCES+= \
        qbitarray.cpp \
        qbuffer.cpp \
        qbytearray.cpp \
        qbytearraymatcher.cpp \
        qcryptographichash.cpp \
        qdatetime.cpp \
        qdir.cpp \
        qdiriterator.cpp \
        qfile.cpp \
        qabstractfileengine.cpp \
        qfileinfo.cpp \
        qfsfileengine.cpp \
        qfsfileengine_iterator.cpp \
        qglobal.cpp \
        qnumeric.cpp \
        qhash.cpp \
        qiodevice.cpp \
        qlistdata.cpp \
        qlinkedlist.cpp \
        qlocale.cpp \
        qmalloc.cpp \
        qmap.cpp \
        qmetatype.cpp \
        qregexp.cpp \
        qstring.cpp \
        qstringlist.cpp \
        qtemporaryfile.cpp \
        qtextstream.cpp \
        qurl.cpp \
        quuid.cpp \
        qsettings.cpp \
        qlibraryinfo.cpp \
        qvariant.cpp \
        qvector.cpp \
        qvsnprintf.cpp

   HEADERS+= \
        qbitarray.h \
        qbuffer.h \
        qbytearray.h \
        qbytearraymatcher.h \
        qchar.h \
        qcryptographichash.h \
        qdatetime.h \
        qdatetime_p.h \
        qdir.h \
        qdiriterator.h \
        qfile.h \
        qabstractfileengine.h \
        qfileinfo.h \
        qfileinfo_p.h \
        qglobal.h \
        qnumeric.h \
        qhash.h \
        qiodevice.h \
        qlist.h \
        qlinkedlist.h \
        qlocale.h \
        qmalloc.h \
        qmap.h \
        qmetatype.h \
        qregexp.h \
        qstring.h \
        qstringlist.h \
        qstringmatcher.h \
        qtemporaryfile.h \
        qtextstream.h \
        qurl.h \
        quuid.h \
        qvector.h

    unix {
        SOURCES += qfsfileengine_unix.cpp qfsfileengine_iterator_unix.cpp
        mac {
          SOURCES += qcore_mac.cpp qsettings_mac.cpp
          QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.3 #enables weak linking for 10.3 (exported)
          LIBS += -framework ApplicationServices
        }
    } else:win32 {
	SOURCES += qfsfileengine_win.cpp qfsfileengine_iterator_win.cpp qsettings_win.cpp
        win32-msvc*:LIBS += ole32.lib advapi32.lib
    }

    qnx {
        CFLAGS += -fhonor-std
        LFLAGS += -lcpp
    }
    DEFINES *= QT_NO_QOBJECT
    include($$QT_SOURCE_TREE/src/script/script.pri)
} else {
    CONFIG += qt 
    QT = core script
}
*-g++:profiling {
  QMAKE_CFLAGS = -pg
  QMAKE_CXXFLAGS = -pg
  QMAKE_LFLAGS = -pg
}

PRECOMPILED_HEADER = qmake_pch.h
