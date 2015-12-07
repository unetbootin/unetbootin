# This qmake file is included by all Patternist projects and contains common Qt defines,
# compiler warnings, and include paths.

INCLUDEPATH += $$PWD/acceltree      \
               $$PWD/data           \
               $$PWD/api            \
               $$PWD/environment    \
               $$PWD/expr           \
               $$PWD/functions      \
               $$PWD/iterators      \
               $$PWD/janitors       \
               $$PWD/parser         \
               $$PWD/type           \
               $$PWD/utils

DEPENDPATH += $$INCLUDEPATH

