DESTDIR = $$QT_BUILD_TREE/plugins/phonon_backend
QT += phonon
win32:!wince*:contains(QT_CONFIG,opengl):LIBS += opengl32.lib
win32:!wince*:LIBS += gdi32.lib
win32-msvc2005:DEFINES += _CRT_SECURE_NO_WARNINGS
LIBS += strmiids.lib Dmoguids.lib uuid.lib msdmo.lib
TARGET = phonon_ds9

DEFINES += PHONON_MAKE_QT_ONLY_BACKEND
PHONON_DS9_DIR = $$QT_SOURCE_TREE/src/3rdparty/phonon/ds9

# Input
HEADERS += \
           $$PHONON_DS9_DIR/abstractvideorenderer.h \
           $$PHONON_DS9_DIR/audiooutput.h \
           $$PHONON_DS9_DIR/backend.h \
           $$PHONON_DS9_DIR/backendnode.h \
           $$PHONON_DS9_DIR/effect.h \
           $$PHONON_DS9_DIR/fakesource.h \
           $$PHONON_DS9_DIR/iodevicereader.h \
           $$PHONON_DS9_DIR/mediagraph.h \
           $$PHONON_DS9_DIR/mediaobject.h \
           $$PHONON_DS9_DIR/videowidget.h \
           $$PHONON_DS9_DIR/videorenderer_soft.h \
           $$PHONON_DS9_DIR/videorenderer_vmr9.h \
           $$PHONON_DS9_DIR/volumeeffect.h \
           $$PHONON_DS9_DIR/qbasefilter.h \
           $$PHONON_DS9_DIR/qpin.h \
           $$PHONON_DS9_DIR/qasyncreader.h \
           $$PHONON_DS9_DIR/qaudiocdreader.h \
           $$PHONON_DS9_DIR/qmeminputpin.h \
           $$PHONON_DS9_DIR/compointer.h \
           $$PHONON_DS9_DIR/phononds9_namespace.h


SOURCES += \
           $$PHONON_DS9_DIR/abstractvideorenderer.cpp \
           $$PHONON_DS9_DIR/audiooutput.cpp \
           $$PHONON_DS9_DIR/backend.cpp \
           $$PHONON_DS9_DIR/backendnode.cpp \
           $$PHONON_DS9_DIR/effect.cpp \
           $$PHONON_DS9_DIR/fakesource.cpp \
           $$PHONON_DS9_DIR/iodevicereader.cpp \
           $$PHONON_DS9_DIR/mediagraph.cpp \
           $$PHONON_DS9_DIR/mediaobject.cpp \
           $$PHONON_DS9_DIR/videowidget.cpp \
           $$PHONON_DS9_DIR/videorenderer_soft.cpp \
           $$PHONON_DS9_DIR/videorenderer_vmr9.cpp \
           $$PHONON_DS9_DIR/volumeeffect.cpp \
           $$PHONON_DS9_DIR/qbasefilter.cpp \
           $$PHONON_DS9_DIR/qpin.cpp \
           $$PHONON_DS9_DIR/qasyncreader.cpp \
           $$PHONON_DS9_DIR/qaudiocdreader.cpp \
           $$PHONON_DS9_DIR/qmeminputpin.cpp


target.path = $$[QT_INSTALL_PLUGINS]/phonon_backend
INSTALLS += target

include(../../qpluginbase.pri)
