# Qt network socket

HEADERS += socket/qabstractsocketengine_p.h \
           socket/qnativesocketengine_p.h \
           socket/qhttpsocketengine_p.h \
           socket/qsocks5socketengine_p.h \
           socket/qabstractsocket.h \
           socket/qabstractsocket_p.h \
           socket/qtcpsocket.h \
           socket/qudpsocket.h \
           socket/qtcpserver.h \
           socket/qlocalserver.h \
           socket/qlocalserver_p.h \
           socket/qlocalsocket.h \
           socket/qlocalsocket_p.h

SOURCES += socket/qabstractsocketengine.cpp \
           socket/qnativesocketengine.cpp \
           socket/qhttpsocketengine.cpp \
           socket/qsocks5socketengine.cpp \
           socket/qabstractsocket.cpp \
           socket/qtcpsocket.cpp \
           socket/qudpsocket.cpp \
           socket/qtcpserver.cpp \
           socket/qlocalsocket.cpp \
           socket/qlocalserver.cpp

unix:SOURCES += socket/qnativesocketengine_unix.cpp \
                socket/qlocalsocket_unix.cpp \
                socket/qlocalserver_unix.cpp


win32:SOURCES += socket/qnativesocketengine_win.cpp \
                socket/qlocalsocket_win.cpp \
                socket/qlocalserver_win.cpp

wince*: {
    SOURCES -= socket/qlocalsocket_win.cpp \
               socket/qlocalserver_win.cpp
    SOURCES += socket/qlocalsocket_tcp.cpp \
               socket/qlocalserver_tcp.cpp

    DEFINES += QT_LOCALSOCKET_TCP
}


