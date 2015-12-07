TARGET  = qmng
include(../../qpluginbase.pri)

QTDIR_build:REQUIRES = "!contains(QT_CONFIG, no-mng)"

HEADERS += qmnghandler.h
SOURCES += main.cpp \
           qmnghandler.cpp

contains(QT_CONFIG, system-mng) {
        unix:LIBS += -lmng
        win32:LIBS += libmng.lib
}
!contains(QT_CONFIG, system-mng) {
        DEFINES += MNG_BUILD_SO
        DEFINES += MNG_NO_INCLUDE_JNG
	INCLUDEPATH += ../../../3rdparty/libmng
	SOURCES  += \
            ../../../3rdparty/libmng/libmng_callback_xs.c \
            ../../../3rdparty/libmng/libmng_chunk_io.c \
            ../../../3rdparty/libmng/libmng_chunk_descr.c \
            ../../../3rdparty/libmng/libmng_chunk_prc.c \
            ../../../3rdparty/libmng/libmng_chunk_xs.c \
            ../../../3rdparty/libmng/libmng_cms.c \
            ../../../3rdparty/libmng/libmng_display.c \
            ../../../3rdparty/libmng/libmng_dither.c \
            ../../../3rdparty/libmng/libmng_error.c \
            ../../../3rdparty/libmng/libmng_filter.c \
            ../../../3rdparty/libmng/libmng_hlapi.c \
            ../../../3rdparty/libmng/libmng_jpeg.c \
            ../../../3rdparty/libmng/libmng_object_prc.c \
            ../../../3rdparty/libmng/libmng_pixels.c \
            ../../../3rdparty/libmng/libmng_prop_xs.c \
            ../../../3rdparty/libmng/libmng_read.c \
            ../../../3rdparty/libmng/libmng_trace.c \
            ../../../3rdparty/libmng/libmng_write.c \
            ../../../3rdparty/libmng/libmng_zlib.c
}

contains(QT_CONFIG, system-zlib) {
        LIBS += -lz
}
!contains(QT_CONFIG, system-zlib) {
        INCLUDEPATH +=  ../../../3rdparty/zlib
}

QTDIR_build:DESTDIR = $$QT_BUILD_TREE/plugins/imageformats
target.path += $$[QT_INSTALL_PLUGINS]/imageformats
INSTALLS += target
