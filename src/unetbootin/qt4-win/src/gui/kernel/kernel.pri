# Qt kernel module

# Only used on platforms with CONFIG += precompile_header
PRECOMPILED_HEADER = kernel/qt_gui_pch.h

# enable a workaround for a miscompilation on AVR32
equals(QT_ARCH, avr32):CONFIG -= precompile_header

KERNEL_P= kernel
HEADERS += \
	kernel/qaction.h \
	kernel/qactiongroup.h \
	kernel/qapplication.h \
	kernel/qapplication_p.h \
	kernel/qboxlayout.h \
	kernel/qclipboard.h \
	kernel/qcursor.h \
	kernel/qdesktopwidget.h \
	kernel/qdrag.h \
	kernel/qdnd_p.h \
	kernel/qevent.h \
	kernel/qevent_p.h \
	kernel/qformlayout.h \
	kernel/qgridlayout.h \
	kernel/qkeysequence.h \
	kernel/qlayout.h \
	kernel/qlayout_p.h \
	kernel/qlayoutengine_p.h \
	kernel/qlayoutitem.h \
	kernel/qmime.h \
	kernel/qsessionmanager.h \
	kernel/qshortcut.h \
	kernel/qshortcutmap_p.h \
	kernel/qsizepolicy.h \
	kernel/qpalette.h \
	kernel/qsound.h \
	kernel/qsound_p.h \
	kernel/qstackedlayout.h \
	kernel/qtooltip.h \
	kernel/qwhatsthis.h \
	kernel/qwidget.h \
	kernel/qwidgetaction.h \
	kernel/qwidgetaction_p.h \
	kernel/qwindowdefs.h \
	kernel/qkeymapper_p.h

SOURCES += \
	kernel/qaction.cpp \
	kernel/qactiongroup.cpp \
	kernel/qapplication.cpp \
	kernel/qboxlayout.cpp \
	kernel/qclipboard.cpp \
	kernel/qcursor.cpp \
	kernel/qdrag.cpp \
	kernel/qdnd.cpp \
	kernel/qevent.cpp \
	kernel/qformlayout.cpp \
	kernel/qgridlayout.cpp \
	kernel/qkeysequence.cpp \
	kernel/qlayout.cpp \
	kernel/qlayoutengine.cpp \
	kernel/qlayoutitem.cpp \
	kernel/qmime.cpp \
	kernel/qpalette.cpp \
	kernel/qshortcut.cpp \
	kernel/qshortcutmap.cpp \
	kernel/qsound.cpp \
	kernel/qstackedlayout.cpp \
	kernel/qtooltip.cpp \
	kernel/qguivariant.cpp \
	kernel/qwhatsthis.cpp \
	kernel/qwidget.cpp \
	kernel/qwidgetaction.cpp \
	kernel/qkeymapper.cpp

win32 {
	SOURCES += \
		kernel/qapplication_win.cpp \
		kernel/qclipboard_win.cpp \
		kernel/qcursor_win.cpp \
		kernel/qdesktopwidget_win.cpp \
		kernel/qdnd_win.cpp \
		kernel/qmime_win.cpp \
		kernel/qsound_win.cpp \
		kernel/qwidget_win.cpp \
		kernel/qole_win.cpp \
		kernel/qkeymapper_win.cpp
}

unix:x11 {
	HEADERS += \
		kernel/qx11embed_x11.h \
		kernel/qx11info_x11.h

	SOURCES += \
		kernel/qapplication_x11.cpp \
		kernel/qclipboard_x11.cpp \
		kernel/qcursor_x11.cpp \
		kernel/qdnd_x11.cpp \
		kernel/qdesktopwidget_x11.cpp \
		kernel/qmotifdnd_x11.cpp \
		kernel/qsound_x11.cpp \
		kernel/qwidget_x11.cpp \
		kernel/qwidgetcreate_x11.cpp \
		kernel/qx11embed_x11.cpp \
		kernel/qx11info_x11.cpp \
		kernel/qkeymapper_x11.cpp

        contains(QT_CONFIG, glib) {
            SOURCES += \
		kernel/qguieventdispatcher_glib.cpp
            HEADERS += \
                kernel/qguieventdispatcher_glib_p.h
            QMAKE_CXXFLAGS += $$QT_CFLAGS_GLIB
	}
            SOURCES += \
		kernel/qeventdispatcher_x11.cpp
            HEADERS += \
                kernel/qeventdispatcher_x11_p.h
}

embedded {
	HEADERS += \
		kernel/qeventdispatcher_qws_p.h

	SOURCES += \
		kernel/qapplication_qws.cpp \
		kernel/qclipboard_qws.cpp \
		kernel/qcursor_qws.cpp \
		kernel/qdesktopwidget_qws.cpp \
		kernel/qdnd_qws.cpp \
		kernel/qeventdispatcher_qws.cpp \
		kernel/qsound_qws.cpp \
		kernel/qwidget_qws.cpp \
		kernel/qkeymapper_qws.cpp

        contains(QT_CONFIG, glib) {
            SOURCES += \
		kernel/qeventdispatcher_glib_qws.cpp
            HEADERS += \
                kernel/qeventdispatcher_glib_qws_p.h
            QMAKE_CXXFLAGS += $$QT_CFLAGS_GLIB
	}


}

!embedded:!x11:mac {
	SOURCES += \
		kernel/qapplication_mac.cpp \
		kernel/qclipboard_mac.cpp \
                kernel/qeventdispatcher_mac.cpp \
		kernel/qmime_mac.cpp \
		kernel/qdnd_mac.cpp \
		kernel/qdesktopwidget_mac.cpp \
		kernel/qwidget_mac.cpp \
		kernel/qkeymapper_mac.cpp

        OBJECTIVE_SOURCES += \
                kernel/qcursor_mac.mm \
                kernel/qsound_mac.mm

        HEADERS += \
                kernel/qeventdispatcher_mac_p.h
        LIBS += -framework AppKit
}

wince*: {
        HEADERS += \
                ../corelib/kernel/qfunctions_wince.h \
                kernel/qguifunctions_wince.h

        SOURCES += \
                ../corelib/kernel/qfunctions_wince.cpp \
                kernel/qguifunctions_wince.cpp
}
