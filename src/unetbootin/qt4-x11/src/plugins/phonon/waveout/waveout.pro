DESTDIR = $$QT_BUILD_TREE/plugins/phonon_backend
QT += phonon
win32-msvc2005:DEFINES += _CRT_SECURE_NO_WARNINGS
TARGET = phonon_waveout

DEFINES += PHONON_MAKE_QT_ONLY_BACKEND
PHONON_WAVEOUT_DIR = $$QT_SOURCE_TREE/src/3rdparty/phonon/waveout

# Input
HEADERS += $$PHONON_WAVEOUT_DIR/audiooutput.h \
           $$PHONON_WAVEOUT_DIR/backend.h \
           $$PHONON_WAVEOUT_DIR/mediaobject.h


SOURCES += $$PHONON_WAVEOUT_DIR/audiooutput.cpp \
           $$PHONON_WAVEOUT_DIR/backend.cpp \
           $$PHONON_WAVEOUT_DIR/mediaobject.cpp


target.path = $$[QT_INSTALL_PLUGINS]/phonon_backend
INSTALLS += target

include(../../qpluginbase.pri)
