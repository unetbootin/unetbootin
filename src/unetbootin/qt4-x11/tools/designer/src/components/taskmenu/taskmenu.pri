INCLUDEPATH += $$PWD \
    ../propertyeditor \
    $$QT_BUILD_TREE/tools/designer/src/components/taskmenu \
    $$QT_SOURCE_TREE/tools/shared/qtpropertybrowser

FORMS += $$PWD/itemlisteditor.ui \
    $$PWD/treewidgeteditor.ui \
    $$PWD/tablewidgeteditor.ui

HEADERS += $$PWD/button_taskmenu.h \
  $$PWD/groupbox_taskmenu.h \
  $$PWD/label_taskmenu.h \
  $$PWD/lineedit_taskmenu.h \
  $$PWD/listwidget_taskmenu.h \
  $$PWD/treewidget_taskmenu.h \
  $$PWD/tablewidget_taskmenu.h \
  $$PWD/combobox_taskmenu.h \
  $$PWD/textedit_taskmenu.h \
  $$PWD/toolbar_taskmenu.h \
  $$PWD/containerwidget_taskmenu.h \
  $$PWD/inplace_editor.h \
  $$PWD/taskmenu_component.h \
  $$PWD/itemlisteditor.h \
  $$PWD/listwidgeteditor.h \
  $$PWD/treewidgeteditor.h \
  $$PWD/tablewidgeteditor.h \
  $$PWD/inplace_widget_helper.h \
  $$PWD/menutaskmenu.h \
  $$PWD/layouttaskmenu.h

SOURCES += $$PWD/button_taskmenu.cpp \
  $$PWD/groupbox_taskmenu.cpp \
  $$PWD/label_taskmenu.cpp \
  $$PWD/lineedit_taskmenu.cpp \
  $$PWD/listwidget_taskmenu.cpp \
  $$PWD/treewidget_taskmenu.cpp \
  $$PWD/tablewidget_taskmenu.cpp \
  $$PWD/combobox_taskmenu.cpp \
  $$PWD/textedit_taskmenu.cpp \
  $$PWD/toolbar_taskmenu.cpp \
  $$PWD/containerwidget_taskmenu.cpp \
  $$PWD/inplace_editor.cpp \
  $$PWD/taskmenu_component.cpp \
  $$PWD/itemlisteditor.cpp \
  $$PWD/listwidgeteditor.cpp \
  $$PWD/treewidgeteditor.cpp \
  $$PWD/tablewidgeteditor.cpp \
  $$PWD/inplace_widget_helper.cpp \
  $$PWD/menutaskmenu.cpp \
  $$PWD/layouttaskmenu.cpp
