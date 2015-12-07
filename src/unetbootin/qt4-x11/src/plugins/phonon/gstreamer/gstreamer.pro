TARGET = phonon_gstreamer
DESTDIR = $$QT_BUILD_TREE/plugins/phonon_backend

DEPENDPATH += .
INCLUDEPATH += .

QT += phonon
contains(QT_CONFIG, opengl):QT += opengl

# GStreamer
QMAKE_CXXFLAGS += $$QT_CFLAGS_GSTREAMER
LIBS += $$QT_LIBS_GSTREAMER -lgstinterfaces-0.10 -lgstvideo-0.10 -lgstbase-0.10 -lgstaudio-0.10

PHONON_GSTREAMER_DIR = $$QT_SOURCE_TREE/src/3rdparty/phonon/gstreamer

HEADERS += $$PHONON_GSTREAMER_DIR/common.h \
 $$PHONON_GSTREAMER_DIR/audiooutput.h \
 $$PHONON_GSTREAMER_DIR/artssink.h \
 $$PHONON_GSTREAMER_DIR/abstractrenderer.h \
 $$PHONON_GSTREAMER_DIR/backend.h \
 $$PHONON_GSTREAMER_DIR/devicemanager.h \
 $$PHONON_GSTREAMER_DIR/effect.h \
 $$PHONON_GSTREAMER_DIR/effectmanager.h \
 $$PHONON_GSTREAMER_DIR/gsthelper.h \
 $$PHONON_GSTREAMER_DIR/mediaobject.h \
 $$PHONON_GSTREAMER_DIR/medianode.h \
 $$PHONON_GSTREAMER_DIR/medianodeevent.h \
 $$PHONON_GSTREAMER_DIR/widgetrenderer.h \
 $$PHONON_GSTREAMER_DIR/videowidget.h \
 $$PHONON_GSTREAMER_DIR/glrenderer.h \
 $$PHONON_GSTREAMER_DIR/qwidgetvideosink.h \
 $$PHONON_GSTREAMER_DIR/phononsrc.h \
 $$PHONON_GSTREAMER_DIR/streamreader.h \
 $$PHONON_GSTREAMER_DIR/message.h \
 $$PHONON_GSTREAMER_DIR/audioeffect.h \
 $$PHONON_GSTREAMER_DIR/volumefadereffect.h

SOURCES += $$PHONON_GSTREAMER_DIR/audiooutput.cpp \
 $$PHONON_GSTREAMER_DIR/abstractrenderer.cpp \
 $$PHONON_GSTREAMER_DIR/artssink.cpp \
 $$PHONON_GSTREAMER_DIR/backend.cpp \
 $$PHONON_GSTREAMER_DIR/devicemanager.cpp \
 $$PHONON_GSTREAMER_DIR/effect.cpp \
 $$PHONON_GSTREAMER_DIR/effectmanager.cpp \
 $$PHONON_GSTREAMER_DIR/gsthelper.cpp \
 $$PHONON_GSTREAMER_DIR/mediaobject.cpp \
 $$PHONON_GSTREAMER_DIR/medianode.cpp \
 $$PHONON_GSTREAMER_DIR/medianodeevent.cpp \
 $$PHONON_GSTREAMER_DIR/widgetrenderer.cpp \
 $$PHONON_GSTREAMER_DIR/videowidget.cpp \
 $$PHONON_GSTREAMER_DIR/glrenderer.cpp \
 $$PHONON_GSTREAMER_DIR/qwidgetvideosink.cpp \
 $$PHONON_GSTREAMER_DIR/phononsrc.cpp \
 $$PHONON_GSTREAMER_DIR/streamreader.cpp \
 $$PHONON_GSTREAMER_DIR/message.cpp \
 $$PHONON_GSTREAMER_DIR/audioeffect.cpp \
 $$PHONON_GSTREAMER_DIR/volumefadereffect.cpp

!embedded {
    HEADERS += $$PHONON_GSTREAMER_DIR/x11renderer.h
    SOURCES += $$PHONON_GSTREAMER_DIR/x11renderer.cpp
}

target.path = $$[QT_INSTALL_PLUGINS]/phonon_backend
INSTALLS += target

include(../../qpluginbase.pri)
