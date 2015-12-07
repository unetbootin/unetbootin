TARGET	   = Qt3Support
QPRO_PWD   = $$PWD
QT         = core gui network sql
DEFINES   += QT_BUILD_COMPAT_LIB
DEFINES   += QT_NO_USING_NAMESPACE
win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x60000000

include(../qbase.pri)
DEFINES -= QT_ASCII_CAST_WARNINGS

PRECOMPILED_HEADER = other/qt_compat_pch.h

include(tools/tools.pri)
include(sql/sql.pri)
include(other/other.pri)
include(itemviews/itemviews.pri)
include(widgets/widgets.pri)
include(dialogs/dialogs.pri)
include(text/text.pri)
include(canvas/canvas.pri)
include(network/network.pri)
include(painting/painting.pri)

unix {
   QMAKE_PKGCONFIG_CFLAGS += -DQT3_SUPPORT
   QMAKE_PKGCONFIG_REQUIRES = QtCore QtGui QtNetwork QtSql
}
mac:LIBS += -framework Carbon

QMAKE_LIBS += $$QMAKE_LIBS_COMPAT $$QMAKE_LIBS_NETWORK
DEFINES -= QT3_SUPPORT_WARNINGS
DEFINES += QT3_SUPPORT
MOCDIR = .moc

*-g++*: QMAKE_CXXFLAGS += -fno-strict-aliasing

CONFIG -= separate_debug_info
CONFIG += no_debug_info

