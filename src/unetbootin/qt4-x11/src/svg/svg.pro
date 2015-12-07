TARGET     = QtSvg
QPRO_PWD   = $$PWD
QT         = core gui
DEFINES   += QT_BUILD_SVG_LIB
DEFINES   += QT_NO_USING_NAMESPACE
win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x66000000
solaris-cc*:QMAKE_CXXFLAGS_RELEASE -= -O2

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore QtGui

include(../qbase.pri)


HEADERS += \
	qsvggraphics_p.h        \
	qsvghandler_p.h         \
	qsvgnode_p.h            \
	qsvgstructure_p.h       \
	qsvgstyle_p.h           \
        qsvgfont_p.h            \
	qsvgtinydocument_p.h    \
	qsvgrenderer.h          \
        qsvgwidget.h            \
        qgraphicssvgitem.h      \
        qsvggenerator.h


SOURCES += \
	qsvggraphics.cpp        \
	qsvghandler.cpp         \
	qsvgnode.cpp            \
	qsvgstructure.cpp       \  
	qsvgstyle.cpp           \
        qsvgfont.cpp            \
	qsvgtinydocument.cpp    \
	qsvgrenderer.cpp        \
        qsvgwidget.cpp          \
        qgraphicssvgitem.cpp    \
        qsvggenerator.cpp

INCLUDEPATH += ../3rdparty/harfbuzz/src

#zlib support
contains(QT_CONFIG, zlib) {
   INCLUDEPATH += ../3rdparty/zlib
} else:!contains(QT_CONFIG, no-zlib) {
   unix:LIBS += -lz
}
