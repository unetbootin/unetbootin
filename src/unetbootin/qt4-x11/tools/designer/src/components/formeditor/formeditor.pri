
QT += xml

INCLUDEPATH += $$PWD

FORMS +=   $$PWD/deviceprofiledialog.ui \
           $$PWD/formwindowsettings.ui \
           $$PWD/templateoptionspage.ui

HEADERS += $$PWD/qdesigner_resource.h \
           $$PWD/formwindow.h \
           $$PWD/formwindow_widgetstack.h \
           $$PWD/formwindow_dnditem.h \
           $$PWD/formwindowcursor.h \
           $$PWD/widgetselection.h \
           $$PWD/formwindowmanager.h \
           $$PWD/formeditor.h \
           $$PWD/formeditor_global.h \
           $$PWD/qlayoutwidget_propertysheet.h \
           $$PWD/layout_propertysheet.h \
           $$PWD/spacer_propertysheet.h \
           $$PWD/line_propertysheet.h \
           $$PWD/default_container.h \
           $$PWD/default_actionprovider.h \
           $$PWD/qmainwindow_container.h \
           $$PWD/qworkspace_container.h \
           $$PWD/qmdiarea_container.h \
           $$PWD/qwizard_container.h \
           $$PWD/default_layoutdecoration.h \
           $$PWD/qtbrushmanager.h \
           $$PWD/brushmanagerproxy.h \
           $$PWD/iconcache.h \
           $$PWD/tool_widgeteditor.h \
           $$PWD/formeditor_optionspage.h \
           $$PWD/embeddedoptionspage.h \
           $$PWD/formwindowsettings.h \
           $$PWD/deviceprofiledialog.h \
           $$PWD/dpi_chooser.h \
           $$PWD/previewactiongroup.h \
           $$PWD/itemview_propertysheet.h \
           $$PWD/templateoptionspage.h

SOURCES += $$PWD/qdesigner_resource.cpp \
           $$PWD/formwindow.cpp \
           $$PWD/formwindow_widgetstack.cpp \
           $$PWD/formwindow_dnditem.cpp \
           $$PWD/formwindowcursor.cpp \
           $$PWD/widgetselection.cpp \
           $$PWD/formwindowmanager.cpp \
           $$PWD/formeditor.cpp \
           $$PWD/qlayoutwidget_propertysheet.cpp \
           $$PWD/layout_propertysheet.cpp \
           $$PWD/spacer_propertysheet.cpp \
           $$PWD/line_propertysheet.cpp \
           $$PWD/qmainwindow_container.cpp \
           $$PWD/qworkspace_container.cpp \
           $$PWD/qmdiarea_container.cpp \
           $$PWD/qwizard_container.cpp \
           $$PWD/default_container.cpp \
           $$PWD/default_layoutdecoration.cpp \
           $$PWD/default_actionprovider.cpp \
           $$PWD/tool_widgeteditor.cpp \
           $$PWD/qtbrushmanager.cpp \
           $$PWD/brushmanagerproxy.cpp \
           $$PWD/iconcache.cpp \
           $$PWD/formeditor_optionspage.cpp \
           $$PWD/embeddedoptionspage.cpp \
           $$PWD/formwindowsettings.cpp \
           $$PWD/deviceprofiledialog.cpp \
           $$PWD/dpi_chooser.cpp \
           $$PWD/previewactiongroup.cpp \
           $$PWD/itemview_propertysheet.cpp \
           $$PWD/templateoptionspage.cpp

RESOURCES += $$PWD/formeditor.qrc
