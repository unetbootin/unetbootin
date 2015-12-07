contains(QT_PRODUCT, OpenSource.*):DEFINES *= QT_OPENSOURCE

for(QTSHAREDLIB, $$list($$unique(LIBS))) {
    QT_SHARED_LIB_NAME =
         isEqual(QTSHAREDLIB, -lformeditor):QT_SHARED_LIB_NAME = formeditor
    else:isEqual(QTSHAREDLIB, -lobjectinspector):QT_SHARED_LIB_NAME = objectinspector
    else:isEqual(QTSHAREDLIB, -lpropertyeditor):QT_SHARED_LIB_NAME = propertyeditor
    else:isEqual(QTSHAREDLIB, -lwidgetbox):QT_SHARED_LIB_NAME = widgetbox
    else:isEqual(QTSHAREDLIB, -lsignalsloteditor):QT_SHARED_LIB_NAME = signalsloteditor
    else:isEqual(QTSHAREDLIB, -ltabordereditor):QT_SHARED_LIB_NAME = tabordereditor
    else:isEqual(QTSHAREDLIB, -lresourceeditor):QT_SHARED_LIB_NAME = resourceeditor
    else:isEqual(QTSHAREDLIB, -lbuddyeditor):QT_SHARED_LIB_NAME = buddyeditor
    else:isEqual(QTSHAREDLIB, -ltaskmenu):QT_SHARED_LIB_NAME = taskmenu
    else:isEqual(QTSHAREDLIB, -lQtDesigner):QT_SHARED_LIB_NAME = QtDesigner
    else:isEqual(QTSHAREDLIB, -lQtDesignerComponents):QT_SHARED_LIB_NAME = QtDesignerComponents
    else:isEqual(QTSHAREDLIB, -lQtOpenGL):QT_SHARED_LIB_NAME = QtOpenGL

    !isEmpty(QT_SHARED_LIB_NAME) {
        LIBS -= -l$${QT_SHARED_LIB_NAME}
        qtAddLibrary($$QT_SHARED_LIB_NAME)
    }
}

unix {
   CONFIG     += create_pc
   QMAKE_PKGCONFIG_LIBDIR = $$[QT_INSTALL_LIBS]
   QMAKE_PKGCONFIG_INCDIR = $$[QT_INSTALL_HEADERS]/$$TARGET
   QMAKE_PKGCONFIG_CFLAGS = -I$$[QT_INSTALL_HEADERS]
   QMAKE_PKGCONFIG_DESTDIR = pkgconfig
}
