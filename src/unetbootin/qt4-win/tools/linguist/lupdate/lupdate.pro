TEMPLATE        = app
QT += xml
CONFIG          += qt warn_on console
CONFIG          -= app_bundle
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}


HEADERS         = lupdate.h \
                  ../shared/metatranslator.h \
                  ../shared/translator.h \
                  ../shared/proparser.h \
                  ../shared/profileevaluator.h \
                  ../shared/proparserutils.h \
                  ../shared/simtexth.h \
                  ../shared/xliff.h

                  
SOURCES         = lupdate.cpp \
                  fetchtr.cpp \
                  fetchtrjava.cpp \
                  main.cpp \
                  merge.cpp \
                  numberh.cpp \
                  sametexth.cpp \
                  ../shared/metatranslator.cpp \
                  ../shared/translator.cpp \
                  ../shared/proparser.cpp \
                  ../shared/profileevaluator.cpp \
                  ../shared/simtexth.cpp \
                  ../shared/xliff.cpp

PROPARSERPATH = ../shared
INCLUDEPATH += $$PROPARSERPATH
# Input
HEADERS += $$PROPARSERPATH/proitems.h \
        $$PROPARSERPATH/abstractproitemvisitor.h \
        $$PROPARSERPATH/proreader.h
SOURCES += $$PROPARSERPATH/proitems.cpp \
        $$PROPARSERPATH/proreader.cpp


TARGET          = lupdate
INCLUDEPATH     += ../shared
DESTDIR          = ../../../bin

win32:RC_FILE = winmanifest.rc

embed_manifest_exe {
    # The default configuration embed_manifest_exe overrides the manifest file
    # already embedded via RC_FILE
    # QMAKE_POST_LINK += $$quote(mt.exe -updateresource:$(DESTDIR_TARGET) -manifest lupdate.exe.manifest)
    QMAKE_POST_LINK += $$quote(mt.exe -updateresource:$$DESTDIR/lupdate.exe -manifest \"$${PWD}\\lupdate.exe.manifest\")
}


target.path=$$[QT_INSTALL_BINS]
INSTALLS        += target
