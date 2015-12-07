QT += qt3support
TEMPLATE = lib
CONFIG += plugin
DESTDIR =
TARGET = qt3supportwidgets

CONFIG += qt warn_on qt_no_compat_warning

include(../plugins.pri)
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

DEPENDPATH += q3buttongroup \
              q3frame \
              q3groupbox \
              q3iconview \
              q3listview \
              q3mainwindow \
              q3toolbar \
              q3widgetstack \
              q3wizard \
              q3listbox \
              q3table \
              q3textedit \
              q3dateedit \
              q3timeedit \
              q3datetimeedit \
              q3progressbar \
              q3textbrowser

INCLUDEPATH += . \
               q3buttongroup \
               q3frame \
               q3groupbox \
               q3iconview \
               q3listview \
               q3mainwindow \
               q3toolbar \
               q3widgetstack \
               q3wizard \
               q3listbox \
               q3table \
               q3textedit \
               q3dateedit \
               q3timeedit \
               q3datetimeedit \
               q3progressbar \
               q3textbrowser


SOURCES += qt3supportwidgets.cpp

# Input
HEADERS += q3buttongroup/q3buttongroup_plugin.h \
           q3frame/q3frame_plugin.h \
           q3groupbox/q3groupbox_plugin.h \
           q3iconview/q3iconview_extrainfo.h \
           q3iconview/q3iconview_plugin.h \
           q3listview/q3listview_extrainfo.h \
           q3listview/q3listview_plugin.h \
           q3mainwindow/q3mainwindow_container.h \
           q3mainwindow/q3mainwindow_plugin.h \
           q3toolbar/q3toolbar_extrainfo.h \
           q3toolbar/q3toolbar_plugin.h \
           q3widgetstack/q3widgetstack_container.h \
           q3widgetstack/q3widgetstack_plugin.h \
           q3widgetstack/qdesigner_q3widgetstack_p.h \
           q3wizard/q3wizard_container.h \
           q3wizard/q3wizard_plugin.h \
           q3listbox/q3listbox_extrainfo.h \
           q3listbox/q3listbox_plugin.h \
           q3table/q3table_extrainfo.h \
           q3table/q3table_plugin.h \
           q3textedit/q3textedit_extrainfo.h \
           q3textedit/q3textedit_plugin.h \
           q3dateedit/q3dateedit_plugin.h \
           q3timeedit/q3timeedit_plugin.h \
           q3datetimeedit/q3datetimeedit_plugin.h \
           q3progressbar/q3progressbar_plugin.h \
           q3textbrowser/q3textbrowser_plugin.h

SOURCES += q3buttongroup/q3buttongroup_plugin.cpp \
           q3frame/q3frame_plugin.cpp \
           q3groupbox/q3groupbox_plugin.cpp \
           q3iconview/q3iconview_extrainfo.cpp \
           q3iconview/q3iconview_plugin.cpp \
           q3listview/q3listview_extrainfo.cpp \
           q3listview/q3listview_plugin.cpp \
           q3mainwindow/q3mainwindow_container.cpp \
           q3mainwindow/q3mainwindow_plugin.cpp \
           q3toolbar/q3toolbar_extrainfo.cpp \
           q3toolbar/q3toolbar_plugin.cpp \
           q3widgetstack/q3widgetstack_container.cpp \
           q3widgetstack/q3widgetstack_plugin.cpp \
           q3widgetstack/qdesigner_q3widgetstack.cpp \
           q3wizard/q3wizard_container.cpp \
           q3wizard/q3wizard_plugin.cpp \
           q3listbox/q3listbox_extrainfo.cpp \
           q3listbox/q3listbox_plugin.cpp \
           q3table/q3table_extrainfo.cpp \
           q3table/q3table_plugin.cpp \
           q3textedit/q3textedit_extrainfo.cpp \
           q3textedit/q3textedit_plugin.cpp \
           q3dateedit/q3dateedit_plugin.cpp \
           q3timeedit/q3timeedit_plugin.cpp \
           q3datetimeedit/q3datetimeedit_plugin.cpp \
           q3progressbar/q3progressbar_plugin.cpp \
           q3textbrowser/q3textbrowser_plugin.cpp
