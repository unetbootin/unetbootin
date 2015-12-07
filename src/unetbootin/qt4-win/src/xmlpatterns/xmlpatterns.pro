TARGET     = QtXmlPatterns
QPRO_PWD   = $$PWD
QT         = core network
DEFINES   += QT_BUILD_XMLPATTERNS_LIB QT_NO_USING_NAMESPACE
win32-msvc*|win32-icc:QMAKE_LFLAGS += /BASE:0x61000000

include(../qbase.pri)

PRECOMPILED_HEADER = ../corelib/global/qt_pch.h

include($$PWD/common.pri)
include($$PWD/acceltree/acceltree.pri)
include($$PWD/api/api.pri)
include($$PWD/data/data.pri)
include($$PWD/environment/environment.pri)
include($$PWD/expr/expr.pri)
include($$PWD/functions/functions.pri)
include($$PWD/iterators/iterators.pri)
include($$PWD/janitors/janitors.pri)
include($$PWD/parser/parser.pri)
include($$PWD/projection/projection.pri)
include($$PWD/type/type.pri)
include($$PWD/utils/utils.pri)
include($$PWD/qobjectmodel/qobjectmodel.pri)

linux-icc*:QMAKE_CXXFLAGS_RELEASE ~= s/-O2/-O0/
