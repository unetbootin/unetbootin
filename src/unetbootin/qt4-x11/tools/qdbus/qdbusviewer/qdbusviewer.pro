TEMPLATE        = app
TARGET          = qdbusviewer

HEADERS         = qdbusviewer.h \
                  qdbusmodel.h \
                  propertydialog.h

SOURCES         = qdbusviewer.cpp \
                  qdbusmodel.cpp \
                  propertydialog.cpp \
                  main.cpp

RESOURCES += qdbusviewer.qrc

DESTDIR = ../../../bin

CONFIG += qdbus
QT += xml

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

mac {
    ICON = images/qdbusviewer.icns
    QMAKE_INFO_PLIST = Info_mac.plist
}

win32 {
    RC_FILE = qdbusviewer.rc
}
