# Qt network module

TARGET   = QtNetwork
QPRO_PWD = $$PWD
DEFINES += QT_BUILD_NETWORK_LIB QT_NO_USING_NAMESPACE
QT = core
win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x64000000

include(../qbase.pri)
include(access/access.pri)
include(kernel/kernel.pri)
include(socket/socket.pri)
include(ssl/ssl.pri)

QMAKE_LIBS += $$QMAKE_LIBS_NETWORK
