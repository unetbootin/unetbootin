hpux-g++*:QMAKE_CXXFLAGS += -mbig-switch

HEADERS += $$PWD/qparsercontext_p.h           \
           $$PWD/qquerytransformparser_p.h    \
           $$PWD/qtokenizer_p.h               \
           $$PWD/qxquerytokenizer_p.h         \

SOURCES += $$PWD/qquerytransformparser.cpp  \
           $$PWD/qparsercontext.cpp         \
           $$PWD/qtokenizer.cpp             \
           $$PWD/qxquerytokenizer.cpp
