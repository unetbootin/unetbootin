# Qt network access module

HEADERS += access/qftp.h \
           access/qhttp.h \
	   access/qhttpnetworkconnection_p.h \
           access/qnetworkaccessmanager.h \
           access/qnetworkaccessmanager_p.h \
	   access/qnetworkaccesscache_p.h \
           access/qnetworkaccessbackend_p.h \
           access/qnetworkaccessdatabackend_p.h \
	   access/qnetworkaccessdebugpipebackend_p.h \
	   access/qnetworkaccesshttpbackend_p.h \
           access/qnetworkaccessfilebackend_p.h \
	   access/qnetworkaccessftpbackend_p.h \
	   access/qnetworkcookie.h \
           access/qnetworkrequest.h \
           access/qnetworkrequest_p.h \
           access/qnetworkreply.h \
           access/qnetworkreply_p.h \
	   access/qnetworkreplyimpl_p.h

SOURCES += access/qftp.cpp \
           access/qhttp.cpp \
	   access/qhttpnetworkconnection.cpp \
           access/qnetworkaccessmanager.cpp \
	   access/qnetworkaccesscache.cpp \
           access/qnetworkaccessbackend.cpp \
           access/qnetworkaccessdatabackend.cpp \
	   access/qnetworkaccessdebugpipebackend.cpp \
           access/qnetworkaccessfilebackend.cpp \
	   access/qnetworkaccessftpbackend.cpp \
	   access/qnetworkaccesshttpbackend.cpp \
	   access/qnetworkcookie.cpp \
           access/qnetworkrequest.cpp \
           access/qnetworkreply.cpp \
           access/qnetworkreplyimpl.cpp

#zlib support
contains(QT_CONFIG, zlib) {
   INCLUDEPATH += ../3rdparty/zlib
} else:!contains(QT_CONFIG, no-zlib) {
   unix:LIBS += -lz
#  win32:LIBS += libz.lib
}
