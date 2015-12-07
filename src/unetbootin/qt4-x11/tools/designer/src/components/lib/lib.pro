TEMPLATE = lib
TARGET = QtDesignerComponents
contains(QT_CONFIG, reduce_exports):CONFIG += hide_symbols
CONFIG += qt depend_prl no_objective_c designer
win32|mac: CONFIG += debug_and_release
QTDIR_build { 
    DESTDIR = $$QT_BUILD_TREE/lib
    !wince*:DLLDESTDIR = $$QT_BUILD_TREE/bin
}

# QtDesignerComponents uses
DEFINES += QT_STATICPLUGIN

isEmpty(QT_MAJOR_VERSION) {
   VERSION=4.3.0
} else {
   VERSION=$${QT_MAJOR_VERSION}.$${QT_MINOR_VERSION}.$${QT_PATCH_VERSION}
}

include(../../../../../src/qt_targets.pri)
QMAKE_TARGET_PRODUCT = Designer
QMAKE_TARGET_DESCRIPTION = Graphical user interface designer.

#load up the headers info
CONFIG += qt_install_headers
HEADERS_PRI = $$QT_BUILD_TREE/include/QtDesigner/headers.pri
include($$HEADERS_PRI)|clear(HEADERS_PRI)

#mac frameworks
mac:!static:contains(QT_CONFIG, qt_framework) {
   QMAKE_FRAMEWORK_BUNDLE_NAME = $$TARGET
   CONFIG += lib_bundle qt_no_framework_direct_includes qt_framework
   CONFIG(debug, debug|release):!build_pass:CONFIG += build_all
}

SOURCES += qdesigner_components.cpp

!contains(CONFIG, static) {
    DEFINES += QDESIGNER_COMPONENTS_LIBRARY
    CONFIG += dll
    LIBS += -lQtDesigner
} else {
    DEFINES += QT_DESIGNER_STATIC
}

INCLUDEPATH += . .. \
    $$QT_SOURCE_TREE/tools/designer/src/lib/components \
    $$QT_SOURCE_TREE/tools/designer/src/lib/sdk \
    $$QT_SOURCE_TREE/tools/designer/src/lib/extension \
    $$QT_SOURCE_TREE/tools/designer/src/lib/uilib \
    $$QT_SOURCE_TREE/tools/designer/src/lib/shared

include(../propertyeditor/propertyeditor.pri)
include(../objectinspector/objectinspector.pri)
include(../signalsloteditor/signalsloteditor.pri)
include(../formeditor/formeditor.pri)
include(../widgetbox/widgetbox.pri)
include(../buddyeditor/buddyeditor.pri)
include(../taskmenu/taskmenu.pri)
include(../tabordereditor/tabordereditor.pri)

PRECOMPILED_HEADER= lib_pch.h

include(../../sharedcomponents.pri)
include(../component.pri)

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore QtDesigner QtGui QtScript QtXml

target.path=$$[QT_INSTALL_LIBS]
INSTALLS        += target
win32 {
    dlltarget.path=$$[QT_INSTALL_BINS]
    INSTALLS += dlltarget
}
