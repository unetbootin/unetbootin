
# infrastructure
QT *= xml

INCLUDEPATH *= $$PWD

SOURCES += \
    $$PWD/numerus.cpp \
    $$PWD/translator.cpp \
    $$PWD/translatormessage.cpp

HEADERS += \
    $$PWD/translator.h \
    $$PWD/translatormessage.h

# "real" formats readers and writers
SOURCES += \
    $$PWD/qm.cpp \
    $$PWD/po.cpp \
    $$PWD/ts.cpp \
    $$PWD/ui.cpp \
    $$PWD/cpp.cpp \
    $$PWD/java.cpp \
    $$PWD/qscript.cpp \
    $$PWD/xliff.cpp 
