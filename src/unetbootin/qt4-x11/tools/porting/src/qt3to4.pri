DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

QT += XML
HEADERS +=  $$PWD/smallobject.h             \
            $$PWD/tokenengine.h             \
            $$PWD/tokenizer.h               \
            $$PWD/rpplexer.h                \
            $$PWD/rpp.h                     \
            $$PWD/rpptreewalker.h           \
            $$PWD/rpptreeevaluator.h        \
            $$PWD/rppexpressionbuilder.h    \
            $$PWD/preprocessorcontrol.h     \
            $$PWD/replacetoken.h            \
            $$PWD/tokenreplacements.h       \
            $$PWD/textreplacement.h         \
            $$PWD/portingrules.h            \
            $$PWD/qtsimplexml.h             \
            $$PWD/projectporter.h           \
            $$PWD/proparser.h               \
            $$PWD/fileporter.h              \
            $$PWD/logger.h                  \
            $$PWD/tokens.h                  \
            $$PWD/filewriter.h              \
            $$PWD/cpplexer.h                \
            $$PWD/smallobject.h             \
            $$PWD/ast.h                     \
            $$PWD/errors.h                  \
            $$PWD/parser.h                  \
            $$PWD/translationunit.h         \
            $$PWD/treewalker.h              \
            $$PWD/semantic.h                \
            $$PWD/codemodel.h               \
            $$PWD/codemodelwalker.h         \
            $$PWD/codemodelattributes.h

SOURCES += \
           $$PWD/smallobject.cpp           \
           $$PWD/tokenengine.cpp           \
           $$PWD/tokenizer.cpp             \
           $$PWD/rpplexer.cpp              \
           $$PWD/rpp.cpp                   \
           $$PWD/rpptreewalker.cpp         \
           $$PWD/rpptreeevaluator.cpp      \
           $$PWD/rppexpressionbuilder.cpp  \
           $$PWD/preprocessorcontrol.cpp   \
           $$PWD/replacetoken.cpp          \
           $$PWD/tokenreplacements.cpp     \
           $$PWD/textreplacement.cpp       \
           $$PWD/portingrules.cpp          \
           $$PWD/qtsimplexml.cpp           \
           $$PWD/projectporter.cpp         \
           $$PWD/proparser.cpp             \
           $$PWD/fileporter.cpp            \
           $$PWD/logger.cpp                \
           $$PWD/filewriter.cpp            \
           $$PWD/cpplexer.cpp              \
           $$PWD/ast.cpp                   \
           $$PWD/errors.cpp                \
           $$PWD/parser.cpp                \
           $$PWD/translationunit.cpp       \
           $$PWD/treewalker.cpp            \
           $$PWD/semantic.cpp              \
           $$PWD/codemodel.cpp             \
           $$PWD/codemodelwalker.cpp       \
           $$PWD/codemodelattributes.cpp


