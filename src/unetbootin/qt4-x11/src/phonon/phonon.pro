TARGET = phonon
include($$QT_SOURCE_TREE/src/qbase.pri)

PHONON_MAJOR_VERSION = $${QT_MAJOR_VERSION}
PHONON_MINOR_VERSION = 3
PHONON_PATCH_VERSION = 1
VERSION = $${PHONON_MAJOR_VERSION}.$${PHONON_MINOR_VERSION}.$${PHONON_PATCH_VERSION}

DEPENDPATH += .
DEFINES += MAKE_PHONON_LIB

PHONON_DIR = $$QT_SOURCE_TREE/src/3rdparty/phonon/phonon

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore QtGui QtNetwork

# Input
HEADERS += $$PHONON_DIR/abstractaudiooutput.h \
           $$PHONON_DIR/abstractaudiooutput_p.h \
           $$PHONON_DIR/abstractmediastream.h \
           $$PHONON_DIR/abstractmediastream_p.h \
           $$PHONON_DIR/abstractvideooutput.h \
           $$PHONON_DIR/abstractvideooutput_p.h \
           $$PHONON_DIR/addoninterface.h \
           $$PHONON_DIR/audiooutput.h \
           $$PHONON_DIR/audiooutput_p.h \
           $$PHONON_DIR/audiooutputinterface.h \
           $$PHONON_DIR/backendcapabilities.h \
           $$PHONON_DIR/backendcapabilities_p.h \
           $$PHONON_DIR/backendinterface.h \
           $$PHONON_DIR/effect.h \
           $$PHONON_DIR/effect_p.h \
           $$PHONON_DIR/effectinterface.h \
           $$PHONON_DIR/effectparameter.h \
           $$PHONON_DIR/effectparameter_p.h \
           $$PHONON_DIR/effectwidget.h \
           $$PHONON_DIR/effectwidget_p.h \
           $$PHONON_DIR/factory_p.h \
           $$PHONON_DIR/frontendinterface_p.h \
           $$PHONON_DIR/globalconfig_p.h \
           $$PHONON_DIR/iodevicestream_p.h \
           $$PHONON_DIR/mediacontroller.h \
           $$PHONON_DIR/medianode.h \
           $$PHONON_DIR/medianode_p.h \
           $$PHONON_DIR/medianodedestructionhandler_p.h \
           $$PHONON_DIR/mediaobject.h \
           $$PHONON_DIR/mediaobject_p.h \
           $$PHONON_DIR/mediaobjectinterface.h \
           $$PHONON_DIR/mediasource.h \
           $$PHONON_DIR/mediasource_p.h \
           $$PHONON_DIR/objectdescription.h \
           $$PHONON_DIR/objectdescription_p.h \
           $$PHONON_DIR/objectdescriptionmodel.h \
           $$PHONON_DIR/objectdescriptionmodel_p.h \
           $$PHONON_DIR/path.h \
           $$PHONON_DIR/path_p.h \
           $$PHONON_DIR/phonondefs.h \
           $$PHONON_DIR/phonondefs_p.h \
           $$PHONON_DIR/phononnamespace.h \
           $$PHONON_DIR/phononnamespace_p.h \
           $$PHONON_DIR/platform_p.h \
           $$PHONON_DIR/platformplugin.h \
           $$PHONON_DIR/qsettingsgroup_p.h \
           $$PHONON_DIR/seekslider.h \
           $$PHONON_DIR/seekslider_p.h \
           $$PHONON_DIR/streaminterface.h \
           $$PHONON_DIR/streaminterface_p.h \
           $$PHONON_DIR/videoplayer.h \
           $$PHONON_DIR/videowidget.h \
           $$PHONON_DIR/videowidget_p.h \
           $$PHONON_DIR/videowidgetinterface.h \
           $$PHONON_DIR/volumefadereffect.h \
           $$PHONON_DIR/volumefadereffect_p.h \
           $$PHONON_DIR/volumefaderinterface.h \
           $$PHONON_DIR/volumeslider.h \
           $$PHONON_DIR/volumeslider_p.h
SOURCES += $$PHONON_DIR/objectdescription.cpp \
           $$PHONON_DIR/objectdescriptionmodel.cpp \
           $$PHONON_DIR/phononnamespace.cpp \
           $$PHONON_DIR/mediasource.cpp \
           $$PHONON_DIR/abstractmediastream.cpp \
           $$PHONON_DIR/streaminterface.cpp \
           $$PHONON_DIR/mediaobject.cpp \
           $$PHONON_DIR/medianode.cpp \
           $$PHONON_DIR/path.cpp \
           $$PHONON_DIR/effectparameter.cpp \
           $$PHONON_DIR/effect.cpp \
           $$PHONON_DIR/volumefadereffect.cpp \
           $$PHONON_DIR/abstractaudiooutput.cpp \
           $$PHONON_DIR/abstractaudiooutput_p.cpp \
           $$PHONON_DIR/audiooutput.cpp \
           $$PHONON_DIR/audiooutputinterface.cpp \
           $$PHONON_DIR/abstractvideooutput.cpp \
           $$PHONON_DIR/abstractvideooutput_p.cpp \
           $$PHONON_DIR/backendcapabilities.cpp \
           $$PHONON_DIR/globalconfig.cpp \
           $$PHONON_DIR/factory.cpp \
           $$PHONON_DIR/platform.cpp \
           $$PHONON_DIR/mediacontroller.cpp \
           $$PHONON_DIR/videowidget.cpp \
           $$PHONON_DIR/videoplayer.cpp \
           $$PHONON_DIR/seekslider.cpp \
           $$PHONON_DIR/volumeslider.cpp \
           $$PHONON_DIR/effectwidget.cpp \
           $$PHONON_DIR/iodevicestream.cpp

contains(QT_CONFIG, dbus) {
       QT      += dbus
       HEADERS += $$PHONON_DIR/audiooutputadaptor_p.h
       SOURCES += $$PHONON_DIR/audiooutputadaptor.cpp
       unix:QMAKE_PKGCONFIG_REQUIRES += QtDBus
} else {
       DEFINES += QT_NO_DBUS
}

contains(QT_CONFIG, reduce_exports): CONFIG += hide_symbols
