#the next line prevents non-shadowbuilds from including the same directory twice
#otherwise, the build command would be too long for some win32 shells.
!exists($$QT_BUILD_TREE/tools/designer/src/components/propertyeditor/propertyeditor.h):INCLUDEPATH += $$QT_BUILD_TREE/tools/designer/src/components/propertyeditor

INCLUDEPATH += $$PWD

include($$QT_SOURCE_TREE/tools/shared/qtpropertybrowser/qtpropertybrowser.pri)
include($$QT_SOURCE_TREE/tools/shared/qtgradienteditor/qtcolorbutton.pri)

FORMS += $$PWD/paletteeditor.ui \
    $$PWD/stringlisteditor.ui \
    $$PWD/previewwidget.ui \
    $$PWD/newdynamicpropertydialog.ui

HEADERS += $$PWD/propertyeditor.h \
    $$PWD/designerpropertymanager.h \
    $$PWD/paletteeditor.h \
    $$PWD/paletteeditorbutton.h \
    $$PWD/stringlisteditor.h \
    $$PWD/stringlisteditorbutton.h \
    $$PWD/previewwidget.h \
    $$PWD/previewframe.h \
    $$PWD/newdynamicpropertydialog.h \
    $$PWD/brushpropertymanager.h \
    $$PWD/fontpropertymanager.h

SOURCES += $$PWD/propertyeditor.cpp \
    $$PWD/designerpropertymanager.cpp \
    $$PWD/paletteeditor.cpp \
    $$PWD/paletteeditorbutton.cpp \
    $$PWD/stringlisteditor.cpp \
    $$PWD/stringlisteditorbutton.cpp \
    $$PWD/previewwidget.cpp \
    $$PWD/previewframe.cpp \
    $$PWD/newdynamicpropertydialog.cpp \
    $$PWD/brushpropertymanager.cpp \
    $$PWD/fontpropertymanager.cpp

HEADERS += \
    $$PWD/propertyeditor_global.h \
    $$PWD/defs.h \
    $$PWD/qlonglongvalidator.h

SOURCES += $$PWD/defs.cpp \
    $$PWD/qlonglongvalidator.cpp

RESOURCES += $$PWD/propertyeditor.qrc
