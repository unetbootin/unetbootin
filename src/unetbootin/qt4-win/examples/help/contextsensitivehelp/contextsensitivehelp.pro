TEMPLATE = app

CONFIG += help

SOURCES += main.cpp \
           wateringconfigdialog.cpp \
           helpbrowser.cpp

HEADERS += wateringconfigdialog.h \
           helpbrowser.h

FORMS   += wateringconfigdialog.ui

# install
target.path = $$[QT_INSTALL_EXAMPLES]/help/contextsensitivehelp
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.png *.doc doc
sources.path = $$[QT_INSTALL_EXAMPLES]/help/contextsensitivehelp
INSTALLS += target sources
