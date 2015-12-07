TEMPLATE = app
CONFIG -= moc
CONFIG += console
CONFIG -= app_bundle
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}
QT = xml core
DEPENDPATH += .
TARGET =qt3to4
DESTDIR = ../../../bin
DEFINES += QT_NO_CAST_TO_ASCII

HEADERS +=  smallobject.h             \
            tokenengine.h             \
            tokenizer.h               \
            rpplexer.h                \
            rpp.h                     \
            rpptreewalker.h           \
            rpptreeevaluator.h        \
            rppexpressionbuilder.h    \
            preprocessorcontrol.h     \
            replacetoken.h            \
            tokenreplacements.h       \
            textreplacement.h         \
            portingrules.h            \
            qtsimplexml.h             \
            projectporter.h           \
            proparser.h               \
            fileporter.h              \
            logger.h                  \
            tokens.h                  \
            filewriter.h              \
            cpplexer.h                \
            smallobject.h             \
            ast.h                     \
            errors.h                  \
            parser.h                  \
            translationunit.h         \
            treewalker.h              \
            semantic.h                \
            codemodel.h               \
            codemodelwalker.h         \
            codemodelattributes.h

SOURCES += port.cpp                  \
           smallobject.cpp           \
           tokenengine.cpp           \
           tokenizer.cpp             \
           rpplexer.cpp              \
           rpp.cpp                   \
           rpptreewalker.cpp         \
           rpptreeevaluator.cpp      \
           rppexpressionbuilder.cpp  \
           preprocessorcontrol.cpp   \
           replacetoken.cpp          \
           tokenreplacements.cpp     \
           textreplacement.cpp       \
           portingrules.cpp          \
           qtsimplexml.cpp           \
           projectporter.cpp         \
           proparser.cpp             \
           fileporter.cpp            \
           logger.cpp                \
           filewriter.cpp            \
           cpplexer.cpp              \
           ast.cpp                   \
           errors.cpp                \
           parser.cpp                \
           translationunit.cpp       \
           treewalker.cpp            \
           semantic.cpp              \
           codemodel.cpp             \
           codemodelwalker.cpp       \
           codemodelattributes.cpp

RESOURCES += qt3headers0.qrc qt3headers1.qrc qt3headers2.qrc qt3headers3.qrc

target.path=$$[QT_INSTALL_BINS]
INSTALLS += target

RULESXML = q3porting.xml
RULESXML.files = q3porting.xml
RULESXML.path = $$[QT_INSTALL_DATA]
INSTALLS += RULESXML

win32-msvc.net|win32-msvc2* {
    QMAKE_CXXFLAGS -= -Zm200 -Zm300
    QMAKE_CXXFLAGS += -Zm1000
    QMAKE_CFLAGS   -= -Zm200 -Zm300
    QMAKE_CFLAGS   += -Zm1000
}
