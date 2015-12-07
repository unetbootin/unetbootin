QT += opengl phonon
TARGET = phonon_qt7
DESTDIR = $$QT_BUILD_TREE/plugins/phonon_backend

# The Quicktime framework is only awailable for 32-bit builds, so we
# need to check for this before linking against it.
# QMAKE_MAC_XARCH is not awailable on Tiger, but at the same time,
# we never build for 64-bit architechtures on Tiger either:
contains(QMAKE_MAC_XARCH, no) {
    LIBS += -framework QuickTime
} else {
    LIBS += -Xarch_i386 -framework QuickTime -Xarch_ppc -framework QuickTime
}

LIBS += -framework AudioUnit \
	-framework AudioToolbox -framework CoreAudio \
	-framework QuartzCore -framework QTKit

DEPENDPATH += .
INCLUDEPATH += . 

PHONON_QUICKTIME_DIR=$$QT_SOURCE_TREE/src/3rdparty/phonon/qt7

# Input
HEADERS += $$PHONON_QUICKTIME_DIR/medianode.h \
		$$PHONON_QUICKTIME_DIR/backend.h \
		$$PHONON_QUICKTIME_DIR/videowidget.h \
		$$PHONON_QUICKTIME_DIR/mediaobject.h \
		$$PHONON_QUICKTIME_DIR/quicktimevideoplayer.h \
	   	$$PHONON_QUICKTIME_DIR/backendheader.h \
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

OBJECTIVE_SOURCES += $$PHONON_QUICKTIME_DIR/quicktimevideoplayer.mm \
			$$PHONON_QUICKTIME_DIR/backendheader.mm \
	   		$$PHONON_QUICKTIME_DIR/medianodevideopart.mm \
	 		$$PHONON_QUICKTIME_DIR/medianodeevent.mm \
	 		$$PHONON_QUICKTIME_DIR/audiooutput.mm \
	 		$$PHONON_QUICKTIME_DIR/backendinfo.mm \
	 		$$PHONON_QUICKTIME_DIR/audiosplitter.mm \
	 		$$PHONON_QUICKTIME_DIR/audioeffects.mm \
	 		$$PHONON_QUICKTIME_DIR/quicktimestreamreader.mm \
			$$PHONON_QUICKTIME_DIR/medianode.mm \
			$$PHONON_QUICKTIME_DIR/backend.mm \
			$$PHONON_QUICKTIME_DIR/mediaobject.mm \
			$$PHONON_QUICKTIME_DIR/mediaobjectaudionode.mm \
	   		$$PHONON_QUICKTIME_DIR/audiomixer.mm  \
			$$PHONON_QUICKTIME_DIR/quicktimeaudioplayer.mm \
			$$PHONON_QUICKTIME_DIR/videoframe.mm \
	 		$$PHONON_QUICKTIME_DIR/quicktimemetadata.mm \
			$$PHONON_QUICKTIME_DIR/audiodevice.mm \
	 		$$PHONON_QUICKTIME_DIR/audioconnection.mm \
	 		$$PHONON_QUICKTIME_DIR/audiograph.mm \
	   		$$PHONON_QUICKTIME_DIR/audionode.mm \
			$$PHONON_QUICKTIME_DIR/videowidget.mm

target.path = $$[QT_INSTALL_PLUGINS]/phonon_backend
INSTALLS += target

include(../../qpluginbase.pri)
