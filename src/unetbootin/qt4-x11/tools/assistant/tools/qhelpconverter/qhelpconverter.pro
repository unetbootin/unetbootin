QT += xml
TEMPLATE = app
TARGET = qhelpconverter
DESTDIR = ../../../../bin
CONFIG += qt warn_on help

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

SOURCES += conversionwizard.cpp \
           inputpage.cpp \
           generalpage.cpp \
           filterpage.cpp \
           identifierpage.cpp \
           pathpage.cpp \
           filespage.cpp \
           outputpage.cpp \
           finishpage.cpp \
           adpreader.cpp \
           qhpwriter.cpp \
           qhcpwriter.cpp \
           helpwindow.cpp \
           main.cpp

HEADERS += conversionwizard.h \
           inputpage.h \
           generalpage.h \
           filterpage.h \
           identifierpage.h \
           pathpage.h \
           filespage.h \
           outputpage.h \
           finishpage.h \
           adpreader.h \
           qhcpwriter.h \
           qhpwriter.h \
           helpwindow.h

FORMS   += inputpage.ui \
           generalpage.ui \
           filterpage.ui \
           identifierpage.ui \
           pathpage.ui \
           filespage.ui \
           outputpage.ui

RESOURCES += qhelpconverter.qrc
