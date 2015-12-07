# Include those manually as they do not contain any directory specification

FORMS += ../helpdialog.ui \
         ../mainwindow.ui \
         ../tabbedbrowser.ui \
         ../topicchooser.ui

SOURCES += ../main.cpp \
           ../helpwindow.cpp \
           ../topicchooser.cpp \
           ../docuparser.cpp \
           ../index.cpp \
           ../profile.cpp \
           ../config.cpp \
           ../helpdialog.cpp \
           ../mainwindow.cpp \
           ../tabbedbrowser.cpp \
           ../fontsettingsdialog.cpp

SOURCES += ../../../shared/fontpanel/fontpanel.cpp

HEADERS += ../helpwindow.h \
           ../topicchooser.h \
           ../docuparser.h \
           ../index.h \
           ../profile.h \
           ../helpdialog.h \
           ../mainwindow.h \
           ../tabbedbrowser.h \
           ../config.h \
           ../fontsettingsdialog.h


TRANSLATIONS=$$[QT_INSTALL_TRANSLATIONS]/assistant_de.ts $$[QT_INSTALL_TRANSLATIONS]/assistant_untranslated.ts
