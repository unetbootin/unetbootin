hpux-g++*:QMAKE_CXXFLAGS += -mbig-switch

HEADERS += $$PWD/qparsercontext_p.h             \
           $$PWD/qmaintainingreader_p.h         \
           $$PWD/qquerytransformparser_p.h      \
           $$PWD/qtokenizer_p.h                 \
           $$PWD/qtokenrevealer_p.h             \
           $$PWD/qtokensource_p.h               \
           $$PWD/qxquerytokenizer_p.h           \
           $$PWD/qxslttokenizer_p.h             \
           $$PWD/qxslttokenlookup_p.h

SOURCES += $$PWD/qquerytransformparser.cpp  \
           $$PWD/qparsercontext.cpp         \
           $$PWD/qtokenrevealer.cpp         \
           $$PWD/qtokensource.cpp           \
           $$PWD/qxquerytokenizer.cpp       \
           $$PWD/qxslttokenizer.cpp         \
           $$PWD/qxslttokenlookup.cpp
