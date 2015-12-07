TARGET     = QtOpenGL
QPRO_PWD   = $$PWD
QT         = core gui
DEFINES   += QT_BUILD_OPENGL_LIB
DEFINES   += QT_NO_USING_NAMESPACE
win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x63000000
solaris-cc*:QMAKE_CXXFLAGS_RELEASE -= -O2

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore QtGui

include(../qbase.pri)

!win32:!embedded:!mac:CONFIG	   += x11
contains(QT_CONFIG, opengl):CONFIG += opengl
contains(QT_CONFIG, opengles1):CONFIG += opengles1
contains(QT_CONFIG, opengles2):CONFIG += opengles2

!contains(QT_CONFIG, opengles2) {
    HEADERS += qgraphicssystem_gl_p.h qwindowsurface_gl_p.h qpixmapdata_gl_p.h
    SOURCES += qgraphicssystem_gl.cpp qwindowsurface_gl.cpp qpixmapdata_gl.cpp
}

HEADERS += qgl.h \
	   qgl_p.h \
	   qglcolormap.h \
	   qglpixelbuffer.h \
	   qglframebufferobject.h  \
	   qglpixmapfilter_p.h 

SOURCES	+= qgl.cpp \
	   qglcolormap.cpp \
	   qglpixelbuffer.cpp \
	   qglframebufferobject.cpp \
           qglextensions.cpp \
           qglpixmapfilter.cpp

!contains(QT_CONFIG, opengles2) {
    HEADERS += qpaintengine_opengl_p.h
    SOURCES += qpaintengine_opengl.cpp
}

contains(QT_CONFIG, opengles2) {
    SOURCES +=  gl2paintengineex/qglgradientcache.cpp \
                gl2paintengineex/qglpexshadermanager.cpp \
                gl2paintengineex/qglshader.cpp \
                gl2paintengineex/qgl2pexvertexarray.cpp \
                gl2paintengineex/qpaintengineex_opengl2.cpp

    HEADERS +=  gl2paintengineex/qglgradientcache_p.h \
                gl2paintengineex/qglpexshadermanager_p.h \
                gl2paintengineex/qglshader_p.h \
                gl2paintengineex/qgl2pexvertexarray_p.h \
                gl2paintengineex/qpaintengineex_opengl2_p.h
}


x11 {
    contains(QT_CONFIG, opengles1)|contains(QT_CONFIG, opengles1cl)|contains(QT_CONFIG, opengles2) {
        SOURCES +=  qgl_x11egl.cpp \
                    qglpixelbuffer_egl.cpp \
                    qgl_egl.cpp \
                    qegl.cpp \
                    qegl_x11egl.cpp

        HEADERS +=  qegl_p.h \
                    qgl_egl_p.h

    } else {
        SOURCES +=  qgl_x11.cpp \
                    qglpixelbuffer_x11.cpp
    }

    contains(QT_CONFIG, fontconfig) {
            include($$QT_SOURCE_TREE/config.tests/unix/freetype/freetype.pri)
    } else {
        DEFINES *= QT_NO_FREETYPE
    }
}

mac {
    OBJECTIVE_SOURCES += qgl_mac.mm \
                         qglpixelbuffer_mac.mm
    LIBS += -framework AppKit
}
win32:!wince*: {
    SOURCES += qgl_win.cpp \
	       qglpixelbuffer_win.cpp
}
wince*: {
    SOURCES += qgl_wince.cpp \
               qglpixelbuffer_egl.cpp \
               qgl_egl.cpp \
               qegl.cpp \
               qegl_wince.cpp

    HEADERS += qgl_cl_p.h \
               qgl_egl_p.h \
               qegl_p.h
}

embedded {
    SOURCES += qgl_qws.cpp \
               qglpaintdevice_qws.cpp \
               qglpixelbuffer_egl.cpp \
               qglscreen_qws.cpp \
               qglwindowsurface_qws.cpp \
               qegl.cpp \
               qegl_qws.cpp \
               qgl_egl.cpp

    HEADERS += qglpaintdevice_qws_p.h \
               qglscreen_qws.h \
               qglwindowsurface_qws_p.h \
               qgl_egl_p.h \
               qegl_p.h

    contains(QT_CONFIG, fontconfig) {
        include($$QT_SOURCE_TREE/config.tests/unix/freetype/freetype.pri)
    } else {
       DEFINES *= QT_NO_FREETYPE
    }
}

INCLUDEPATH += ../3rdparty/harfbuzz/src

wince*: {
    contains(QT_CONFIG,opengles1) {
        QMAKE_LIBS += "libGLES_CM.lib"
    }
    contains(QT_CONFIG,opengles1cl) {
        QMAKE_LIBS += "libGLES_CL.lib"
    }
} else {
    QMAKE_LIBS += $$QMAKE_LIBS_OPENGL
}
