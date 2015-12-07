TEMPLATE = app
LANGUAGE = C++
DESTDIR = ../../../bin

QT += xml \
    network

CONFIG += qt \
    warn_on \
    uitools

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
build_all:!build_pass { 
    CONFIG -= build_all
    CONFIG += release
}

include(../shared/formats.pri)

DEFINES += QFORMINTERNAL_NAMESPACE
INCLUDEPATH += ../../designer/src/uitools
INCLUDEPATH += ../../designer/src/lib/uilib

SOURCES += \
    batchtranslationdialog.cpp \
    errorsview.cpp \
    finddialog.cpp \
    formpreviewview.cpp \
    main.cpp \
    mainwindow.cpp \
    messageeditor.cpp \
    messageeditorwidgets.cpp \
    messagehighlighter.cpp \
    messagemodel.cpp \
    phrasebookbox.cpp \
    phrase.cpp \
    phrasemodel.cpp \
    phraseview.cpp \
    printout.cpp \
    recentfiles.cpp \
    sourcecodeview.cpp \
    statistics.cpp \
    translatedialog.cpp \
    translationsettingsdialog.cpp \
    ../shared/simtexth.cpp 

HEADERS += \
    batchtranslationdialog.h \
    errorsview.h \
    finddialog.h \
    formpreviewview.h \
    mainwindow.h \
    messageeditor.h \
    messageeditorwidgets.h \
    messagehighlighter.h \
    messagemodel.h \
    phrasebookbox.h \
    phrase.h \
    phrasemodel.h \
    phraseview.h \
    printout.h \
    recentfiles.h \
    sourcecodeview.h \
    statistics.h \
    translatedialog.h \
    translationsettingsdialog.h \
    ../shared/simtexth.h

contains(QT_PRODUCT, OpenSource.*):DEFINES *= QT_OPENSOURCE
DEFINES += QT_KEYWORDS
TARGET = linguist
win32:RC_FILE = linguist.rc
mac { 
    static:CONFIG -= global_init_link_order
    ICON = linguist.icns
    TARGET = Linguist
    QMAKE_INFO_PLIST=Info_mac.plist
}
PROJECTNAME = Qt \
    Linguist
target.path = $$[QT_INSTALL_BINS]
INSTALLS += target
linguisttranslations.files = *.qm
linguisttranslations.path = $$[QT_INSTALL_TRANSLATIONS]
INSTALLS += linguisttranslations
phrasebooks.path = $$[QT_INSTALL_DATA]/phrasebooks

# ## will this work on windows?
phrasebooks.files = $$QT_SOURCE_TREE/tools/linguist/phrasebooks/*
INSTALLS += phrasebooks
FORMS += statistics.ui \
    phrasebookbox.ui \
    batchtranslation.ui \
    translatedialog.ui \
    mainwindow.ui \
    translationsettings.ui \
    finddialog.ui
RESOURCES += linguist.qrc

TRANSLATIONS=$$[QT_INSTALL_TRANSLATIONS]/linguist_ja.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_pl.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_untranslated.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_tr_TR.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_zh_CN.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_zh_TW.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_de.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_fr.ts
