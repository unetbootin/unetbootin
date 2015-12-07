TEMPLATE = app
LANGUAGE = C++
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
include(previewtool/previewtool.pri)
SOURCES += batchtranslationdialog.cpp \
    finddialog.cpp \
    main.cpp \
    messagemodel.cpp \
    messagestreeview.cpp \
    messageeditor.cpp \
    messageeditorwidgets.cpp \
    messagehighlighter.cpp \
    phrasebookbox.cpp \
    phrase.cpp \
    phrasemodel.cpp \
    phraseview.cpp \
    sourcecodeview.cpp \
    errorsview.cpp \
    printout.cpp \
    recentfiles.cpp \
    sortedmessagesmodel.cpp \
    statistics.cpp \
    translatedialog.cpp \
    translationsettingsdialog.cpp \
    trwindow.cpp \
    languagesdialog.cpp \
    languagesmanager.cpp \
    ../shared/xliff.cpp \
    ../shared/simtexth.cpp \
    ../shared/metatranslator.cpp \
    ../shared/translator.cpp
HEADERS += batchtranslationdialog.h \
    finddialog.h \
    messagemodel.h \
    messagestreeview.h \
    messageeditor.h \
    messageeditorwidgets.h \
    messagehighlighter.h \
    phrasebookbox.h \
    phrase.h \
    phrasemodel.h \
    phraseview.h \
    sourcecodeview.h \
    errorsview.h \
    printout.h \
    recentfiles.h \
    sortedmessagesmodel.h \
    statistics.h \
    translatedialog.h \
    translationsettingsdialog.h \
    trwindow.h \
    languagesdialog.h \
    languagesmanager.h \
    ../shared/xliff.h \
    ../shared/simtexth.h \
    ../shared/metatranslator.h \
    ../shared/translator.h
contains(QT_PRODUCT, OpenSource.*):DEFINES *= QT_OPENSOURCE
DEFINES += QT_KEYWORDS
DESTDIR = ../../../bin
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
    finddialog.ui \
    languagesdialog.ui
INCLUDEPATH += ../shared \
    ../../assistant/lib
RESOURCES += linguist.qrc

TRANSLATIONS=$$[QT_INSTALL_TRANSLATIONS]/linguist_ja.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_pl.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_untranslated.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_tr_TR.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_zh_CN.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_zh_TW.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_de.ts \
             $$[QT_INSTALL_TRANSLATIONS]/linguist_fr.ts
