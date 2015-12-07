TEMPLATE        = subdirs

no-png {
    message("Some graphics-related tools are unavailable without PNG support")
} else {
     SUBDIRS += assistant \
		pixeltool \
		porting \
                qtestlib
     contains(QT_EDITION, Console) {
         SUBDIRS += designer/src/uitools     # Linguist depends on this
     } else {
         SUBDIRS += designer
     }
     SUBDIRS     += linguist
     wince*: SUBDIRS = qtestlib designer
     unix:!mac:!embedded:contains(QT_CONFIG, qt3support):SUBDIRS += qtconfig
     win32:!wince*:!contains(QT_EDITION, OpenSource|Console):SUBDIRS += activeqt
}

mac {
    SUBDIRS += macdeployqt
}

contains(QT_CONFIG, dbus):SUBDIRS += qdbus
!wince*:contains(QT_CONFIG, xmlpatterns): SUBDIRS += xmlpatterns
embedded: SUBDIRS += makeqpf

CONFIG+=ordered
QTDIR_build:REQUIRES = "contains(QT_CONFIG, full-config)"
