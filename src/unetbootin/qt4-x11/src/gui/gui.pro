TARGET     = QtGui
QPRO_PWD   = $$PWD
QT = core
DEFINES   += QT_BUILD_GUI_LIB QT_NO_USING_NAMESPACE
win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x65000000

!win32:!embedded:!mac:CONFIG      += x11

unix:QMAKE_PKGCONFIG_REQUIRES = QtCore

include(../qbase.pri)

contains(QT_CONFIG, x11sm):CONFIG += x11sm

#platforms
x11:include(kernel/x11.pri)
mac:include(kernel/mac.pri)
win32:include(kernel/win.pri)
embedded:include(embedded/embedded.pri)

#modules
include(kernel/kernel.pri)
include(image/image.pri)
include(painting/painting.pri)
include(text/text.pri)
include(styles/styles.pri)
include(widgets/widgets.pri)
include(dialogs/dialogs.pri)
include(accessible/accessible.pri)
include(itemviews/itemviews.pri)
include(inputmethod/inputmethod.pri)
include(graphicsview/graphicsview.pri)
include(util/util.pri)

embedded: QT += network

QMAKE_LIBS += $$QMAKE_LIBS_GUI

contains(DEFINES,QT_EVAL):include($$QT_SOURCE_TREE/src/corelib/eval.pri)

QMAKE_DYNAMIC_LIST_FILE = $$PWD/QtGui.dynlist

DEFINES += Q_INTERNAL_QAPP_SRC

