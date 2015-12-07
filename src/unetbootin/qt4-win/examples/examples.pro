TEMPLATE      = subdirs
SUBDIRS       = \
                desktop \
                dialogs \
                draganddrop \
                graphicsview \
                ipc \
                itemviews \
                layouts \
                linguist \
                mainwindows \
                network \
                painting \
                qtconcurrent \
                richtext \
                sql \
                threads \
                tools \
                tutorials \
                widgets \
                uitools \
                xml \
                script

contains(QT_CONFIG, phonon): SUBDIRS += phonon
contains(QT_CONFIG, webkit): SUBDIRS += webkit
embedded:SUBDIRS += qws
!wince*: {
	!contains(QT_EDITION, Console):contains(QT_BUILD_PARTS, tools):SUBDIRS += designer
	contains(QT_BUILD_PARTS, tools):SUBDIRS += assistant qtestlib help
} else {
	contains(QT_BUILD_PARTS, tools):SUBDIRS += qtestlib
}
!embedded:contains(QT_CONFIG, opengl): SUBDIRS += opengl
contains(QT_CONFIG, dbus): SUBDIRS += dbus
win32:!contains(QT_EDITION, OpenSource|Console):SUBDIRS += activeqt
contains(QT_CONFIG, xmlpatterns): SUBDIRS += xmlpatterns
contains(DEFINES, QT_NO_CURSOR): SUBDIRS -= mainwindows

# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]
INSTALLS += sources
