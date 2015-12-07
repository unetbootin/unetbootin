
QMAKE_TARGET_PRODUCT = QtCLucene
QMAKE_TARGET_DESCRIPTION = QtCLucene full text search library wrapper.
#if qt is built with frameworks in debug, we must build QtCLucene in debug and release 
#that's a similar logic as in qbase.pri 
mac:!static:contains(QT_CONFIG, qt_framework) { 
   CONFIG(debug, debug|release) { 
      !build_pass:CONFIG += build_all 
   } 
}
QT_CONFIG -= qt_framework 
QT -= gui
TEMPLATE = lib
TARGET = QtCLucene
DEFINES += QHELP_LIB
include(../../../../src/qbase.pri)
include(fulltextsearch.pri)

CONFIG += qt warn_off
contains(QT_CONFIG, reduce_exports) {
    CONFIG += hide_symbols
    # workaround for compiler errors on Ubuntu
    linux*-g++*:DEFINES += _GLIBCXX_EXTERN_TEMPLATE=0
}

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore

# impossible to disable exceptions in clucene atm
CONFIG(exceptions_off) {
    CONFIG -= exceptions_off
    CONFIG += exceptions
    !win32|win32-g++ {
        QMAKE_CFLAGS -= -fno-exceptions
        QMAKE_CXXFLAGS -= -fno-exceptions
        QMAKE_LFLAGS -= -fno-exceptions
        QMAKE_CFLAGS += -fexceptions
        QMAKE_CXXFLAGS += -fexceptions
        QMAKE_LFLAGS += -fexceptions
    }
}

win32-msvc.net | win32-msvc2* {
    QMAKE_CFLAGS_RELEASE	-= -O2
    QMAKE_CXXFLAGS_RELEASE	-= -O2    
}

# the following define could be set globally in case we need it elsewhere
solaris* {
    DEFINES += Q_SOLARIS_VERSION=$$system(uname -r | sed -e 's/5\.//')
}
