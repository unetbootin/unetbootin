QT += opengl phonon
TARGET = phonon_qt7
DESTDIR = $$QT_BUILD_TREE/plugins/phonon_backend
LIBS += -framework QuickTime -framework AudioUnit \
	-framework AudioToolbox -framework CoreAudio \
	-framework QuartzCore -framework QTKit

DEPENDPATH += .
INCLUDEPATH += . 

PHONON_QUICKTIME_DIR=$$QT_SOURCE_TREE/src/3rdparty/kdebase/runtime/phonon/qt7

# Input
HEADERS += 	$$PHONON_QUICKTIME_DIR/medianode.h \
		$$PHONON_QUICKTIME_DIR/backend.h \
		$$PHONON_QUICKTIME_DIR/videowidget.h \
		$$PHONON_QUICKTIME_DIR/mediaobject.h \
		$$PHONON_QUICKTIME_DIR/quicktimevideoplayer.h \
	   	$$PHONON_QUICKTIME_DIR/backendheader.h \
	 	$$PHONON_QUICKTIME_DIR/displaylinkcallback.h \
	   	$$PHONON_QUICKTIME_DIR/medianodevideopart.h \
	 	$$PHONON_QUICKTIME_DIR/medianodeevent.h  \
		$$PHONON_QUICKTIME_DIR/quicktimeaudioplayer.h \
	   	$$PHONON_QUICKTIME_DIR/audionode.h  \
		$$PHONON_QUICKTIME_DIR/audiograph.h \
	 	$$PHONON_QUICKTIME_DIR/audiooutput.h \
	 	$$PHONON_QUICKTIME_DIR/quicktimemetadata.h \
	   	$$PHONON_QUICKTIME_DIR/audiomixer.h \
	 	$$PHONON_QUICKTIME_DIR/audiodevice.h \
	 	$$PHONON_QUICKTIME_DIR/backendinfo.h \
	 	$$PHONON_QUICKTIME_DIR/audioconnection.h \
	   	$$PHONON_QUICKTIME_DIR/videoframe.h \
	 	$$PHONON_QUICKTIME_DIR/audiosplitter.h \
	 	$$PHONON_QUICKTIME_DIR/audioeffects.h \
	 	$$PHONON_QUICKTIME_DIR/quicktimestreamreader.h \
	 	$$PHONON_QUICKTIME_DIR/mediaobjectaudionode.h 
# HEADERS += objc_help.h videoeffect.h

SOURCES += $$PHONON_QUICKTIME_DIR/medianode.cpp \
 		$$PHONON_QUICKTIME_DIR/backend.cpp \
 		$$PHONON_QUICKTIME_DIR/videowidget.cpp \
 		$$PHONON_QUICKTIME_DIR/mediaobject.cpp \
 		$$PHONON_QUICKTIME_DIR/quicktimevideoplayer.cpp \
	   	$$PHONON_QUICKTIME_DIR/displaylinkcallback.cpp \
	 	$$PHONON_QUICKTIME_DIR/error.cpp \
	   	$$PHONON_QUICKTIME_DIR/medianodevideopart.cpp \
	 	$$PHONON_QUICKTIME_DIR/medianodeevent.cpp \
	 	$$PHONON_QUICKTIME_DIR/quicktimeaudioplayer.cpp \
	   	$$PHONON_QUICKTIME_DIR/audionode.cpp \
	 	$$PHONON_QUICKTIME_DIR/audiograph.cpp \
	 	$$PHONON_QUICKTIME_DIR/audiooutput.cpp \
	 	$$PHONON_QUICKTIME_DIR/quicktimemetadata.cpp \
	   	$$PHONON_QUICKTIME_DIR/audiomixer.cpp  \
		$$PHONON_QUICKTIME_DIR/audiodevice.cpp \
	 	$$PHONON_QUICKTIME_DIR/backendinfo.cpp \
	 	$$PHONON_QUICKTIME_DIR/audioconnection.cpp \
	   	$$PHONON_QUICKTIME_DIR/videoframe.cpp \
	 	$$PHONON_QUICKTIME_DIR/audiosplitter.cpp \
	 	$$PHONON_QUICKTIME_DIR/audioeffects.cpp \
	 	$$PHONON_QUICKTIME_DIR/quicktimestreamreader.cpp \
	 	$$PHONON_QUICKTIME_DIR/mediaobjectaudionode.cpp

OBJECTIVE_SOURCES += $$PHONON_QUICKTIME_DIR/quicktimestreamreader_objc.mm

target.path = $$[QT_INSTALL_PLUGINS]/phonon_backend
INSTALLS += target

include(../../qpluginbase.pri)
