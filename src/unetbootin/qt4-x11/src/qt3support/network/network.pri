# Qt compat module

HEADERS +=      network/q3dns.h \
		network/q3ftp.h \
		network/q3http.h \
		network/q3localfs.h \
		network/q3network.h \
		network/q3networkprotocol.h \
                network/q3socket.h \
                network/q3socketdevice.h \
                network/q3serversocket.h \
		network/q3url.h \
		network/q3urloperator.h

SOURCES +=      network/q3dns.cpp \
		network/q3ftp.cpp \
		network/q3http.cpp \
		network/q3localfs.cpp \
		network/q3network.cpp \
		network/q3networkprotocol.cpp \
                network/q3socket.cpp \
                network/q3socketdevice.cpp \
                network/q3serversocket.cpp \
		network/q3url.cpp \
		network/q3urloperator.cpp

win32:SOURCES +=  network/q3socketdevice_win.cpp
unix:SOURCES += network/q3socketdevice_unix.cpp
mac:LIBS += -lresolv

