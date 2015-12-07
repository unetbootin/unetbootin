TARGET   = QtTest
QPRO_PWD = $$PWD
QT       = core
INCLUDEPATH += .

unix:!embedded {
   QMAKE_PKGCONFIG_DESCRIPTION = Qt Unit Testing Library
   QMAKE_PKGCONFIG_REQUIRES = QtCore
}

# Input
HEADERS = qtest_global.h qtestcase.h qtestdata.h qtesteventloop.h
SOURCES = qtestcase.cpp qtestlog.cpp qtesttable.cpp qtestdata.cpp qtestresult.cpp qasciikey.cpp qplaintestlogger.cpp qxmltestlogger.cpp qsignaldumper.cpp qabstracttestlogger.cpp qbenchmark.cpp qbenchmarkmeasurement.cpp qbenchmarkvalgrind.cpp qbenchmarkevent.cpp

DEFINES += QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII QTESTLIB_MAKEDLL QT_NO_DATASTREAM

wince*:{
   LIBS += libcmt.lib corelibc.lib ole32.lib oleaut32.lib uuid.lib commctrl.lib coredll.lib winsock.lib
}

mac {
    LIBS += -framework IOKit -framework Security
}

include(../qbase.pri)
QMAKE_TARGET_PRODUCT = QTestLib
QMAKE_TARGET_DESCRIPTION = Qt Unit Testing Library
