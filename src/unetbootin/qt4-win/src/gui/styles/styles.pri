# Qt styles module

HEADERS += \
	styles/qstyle.h \
	styles/qstylefactory.h \
	styles/qstyleoption.h \
	styles/qstyleplugin.h \
	styles/qcommonstylepixmaps_p.h \
	styles/qcommonstyle.h \
	styles/qstylesheetstyle_p.h
SOURCES += \
	styles/qstyle.cpp \
	styles/qstylefactory.cpp \
	styles/qstyleoption.cpp \
	styles/qstyleplugin.cpp \
	styles/qcommonstyle.cpp \
	styles/qstylesheetstyle.cpp

!wince* {
        RESOURCES += styles/qstyle.qrc
} else {
        RESOURCES += styles/qstyle_wince.qrc
}

contains( styles, all ) {
	styles = mac windows windowsxp windowsvista
}

x11|embedded|!macx-*:styles -= mac

contains( styles, mac ) {
	HEADERS += \
		styles/qmacstyle_mac.h \
		styles/qmacstylepixmaps_mac_p.h
	SOURCES += \
		styles/qmacstyle_mac.cpp

	!contains( styles, windows ) {
		message( mac requires windows )
		styles += windows
		DEFINES+= QT_STYLE_WINDOWS
	}
} else {
	DEFINES += QT_NO_STYLE_MAC
}

contains( styles, cde ) {
	HEADERS += styles/qcdestyle.h
	SOURCES += styles/qcdestyle.cpp

	!contains( styles, motif ) {
		message( cde requires motif )
		styles += motif
		DEFINES+= QT_STYLE_MOTIF
	}
} else {
	DEFINES += QT_NO_STYLE_CDE
}

contains( styles, windowsvista ) {
	HEADERS += styles/qwindowsvistastyle.h
	HEADERS += styles/qwindowsvistastyle_p.h
	SOURCES += styles/qwindowsvistastyle.cpp
	!contains( styles, windowsxp ) {
		message( windowsvista requires windowsxp )
		styles += windowsxp
		DEFINES+= QT_STYLE_WINDOWSXP
	}
} else {
	DEFINES += QT_NO_STYLE_WINDOWSVISTA
}

contains( styles, windowsxp ) {
	HEADERS += styles/qwindowsxpstyle.h
	SOURCES += styles/qwindowsxpstyle.cpp
	!contains( styles, windows ) {
		message( windowsxp requires windows )
		styles += windows
		DEFINES+= QT_STYLE_WINDOWS
	}
} else {
	DEFINES += QT_NO_STYLE_WINDOWSXP
}

contains( styles, plastique ) {
	HEADERS += styles/qplastiquestyle.h
	SOURCES += styles/qplastiquestyle.cpp
	!contains( styles, windows ) {
		message( plastique requires windows )
		styles += windows
		DEFINES+= QT_STYLE_WINDOWS
	}
} else {
	DEFINES += QT_NO_STYLE_PLASTIQUE
}

contains( styles, cleanlooks ) {
	HEADERS += styles/qcleanlooksstyle.h
	SOURCES += styles/qcleanlooksstyle.cpp
	!contains( styles, windows ) {
		message( cleanlooks requires windows )
		styles += windows
		DEFINES+= QT_STYLE_WINDOWS
	}
} else {
	DEFINES += QT_NO_STYLE_CLEANLOOKS
}
				
contains( styles, windows ) {
	HEADERS += styles/qwindowsstyle.h
	SOURCES += styles/qwindowsstyle.cpp
} else {
	DEFINES += QT_NO_STYLE_WINDOWS
}

contains( styles, motif ) {
	HEADERS += styles/qmotifstyle.h
	SOURCES += styles/qmotifstyle.cpp
} else {
	DEFINES += QT_NO_STYLE_MOTIF
}

contains( styles, windowsce ) {
	HEADERS += styles/qwindowscestyle.h
	SOURCES += styles/qwindowscestyle.cpp
} else {
	DEFINES += QT_NO_STYLE_WINDOWSCE
}

contains( styles, windowsmobile ) {
	HEADERS += styles/qwindowsmobilestyle.h
	SOURCES += styles/qwindowsmobilestyle.cpp
} else {
	DEFINES += QT_NO_STYLE_WINDOWSMOBILE
}

