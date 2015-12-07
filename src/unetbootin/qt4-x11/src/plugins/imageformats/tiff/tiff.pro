TARGET  = qtiff
include(../../qpluginbase.pri)

QTDIR_build:REQUIRES = "!contains(QT_CONFIG, no-tiff)"

HEADERS += qtiffhandler.h
SOURCES += main.cpp \
           qtiffhandler.cpp

contains(QT_CONFIG, system-tiff) {
        unix:LIBS += -ltiff
        win32:LIBS += libtiff.lib
}
!contains(QT_CONFIG, system-tiff) {
	INCLUDEPATH += ../../../3rdparty/libtiff/libtiff
	SOURCES  += \
	    ../../../3rdparty/libtiff/libtiff/tif_aux.c \
	    ../../../3rdparty/libtiff/libtiff/tif_close.c \
	    ../../../3rdparty/libtiff/libtiff/tif_codec.c \
	    ../../../3rdparty/libtiff/libtiff/tif_color.c \
	    ../../../3rdparty/libtiff/libtiff/tif_compress.c \
	    ../../../3rdparty/libtiff/libtiff/tif_dir.c \
	    ../../../3rdparty/libtiff/libtiff/tif_dirinfo.c \
	    ../../../3rdparty/libtiff/libtiff/tif_dirread.c \
	    ../../../3rdparty/libtiff/libtiff/tif_dirwrite.c \
	    ../../../3rdparty/libtiff/libtiff/tif_dumpmode.c \
	    ../../../3rdparty/libtiff/libtiff/tif_error.c \
	    ../../../3rdparty/libtiff/libtiff/tif_extension.c \
	    ../../../3rdparty/libtiff/libtiff/tif_fax3.c \
	    ../../../3rdparty/libtiff/libtiff/tif_fax3sm.c \
	    ../../../3rdparty/libtiff/libtiff/tif_flush.c \
	    ../../../3rdparty/libtiff/libtiff/tif_getimage.c \
	    ../../../3rdparty/libtiff/libtiff/tif_luv.c \
	    ../../../3rdparty/libtiff/libtiff/tif_lzw.c \
	    ../../../3rdparty/libtiff/libtiff/tif_next.c \
	    ../../../3rdparty/libtiff/libtiff/tif_open.c \
	    ../../../3rdparty/libtiff/libtiff/tif_packbits.c \
	    ../../../3rdparty/libtiff/libtiff/tif_pixarlog.c \
	    ../../../3rdparty/libtiff/libtiff/tif_predict.c \
	    ../../../3rdparty/libtiff/libtiff/tif_print.c \
	    ../../../3rdparty/libtiff/libtiff/tif_read.c \
	    ../../../3rdparty/libtiff/libtiff/tif_strip.c \
	    ../../../3rdparty/libtiff/libtiff/tif_swab.c \
	    ../../../3rdparty/libtiff/libtiff/tif_thunder.c \
	    ../../../3rdparty/libtiff/libtiff/tif_tile.c \
	    ../../../3rdparty/libtiff/libtiff/tif_version.c \
	    ../../../3rdparty/libtiff/libtiff/tif_warning.c \
	    ../../../3rdparty/libtiff/libtiff/tif_write.c \
	    ../../../3rdparty/libtiff/libtiff/tif_zip.c
	    win32 {
	       SOURCES += ../../../3rdparty/libtiff/libtiff/tif_win32.c
	    }
            unix: {
	       SOURCES += ../../../3rdparty/libtiff/libtiff/tif_unix.c
            }
            wince*: {
               SOURCES += ../../../corelib/kernel/qfunctions_wince.cpp
            }
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
