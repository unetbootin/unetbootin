TEMPLATE = lib
CONFIG += static

!embedded:contains(QT_CONFIG, opengl) {
	DEFINES += QT_OPENGL_SUPPORT
	QT += opengl
}

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}
TARGET = demo_shared

SOURCES += \
	arthurstyle.cpp\
	arthurwidgets.cpp \
	hoverpoints.cpp

HEADERS += \
	arthurstyle.h \
	arthurwidgets.h \
	hoverpoints.h

RESOURCES += shared.qrc

# install
target.path = $$[QT_INSTALL_DEMOS]/shared
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro *.pri images
sources.path = $$[QT_INSTALL_DEMOS]/shared
INSTALLS += sources
!cross_compile:INSTALLS += target

