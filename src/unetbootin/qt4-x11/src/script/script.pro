TARGET     = QtScript
QPRO_PWD   = $$PWD
QT         = core
DEFINES   += QT_BUILD_SCRIPT_LIB
DEFINES   += QT_NO_USING_NAMESPACE
DEFINES   += QLALR_NO_QSCRIPTGRAMMAR_DEBUG_INFO
#win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x66000000       ### FIXME

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore

include(../qbase.pri)
include(script.pri)
