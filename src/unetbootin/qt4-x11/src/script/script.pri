
*-g++:DEFINES += Q_SCRIPT_DIRECT_CODE

SOURCES += \
    $$PWD/qscriptasm.cpp \
    $$PWD/qscriptast.cpp \
    $$PWD/qscriptastvisitor.cpp \
    $$PWD/qscriptcompiler.cpp \
    $$PWD/qscriptecmaarray.cpp \
    $$PWD/qscriptecmaboolean.cpp \
    $$PWD/qscriptecmacore.cpp \
    $$PWD/qscriptecmadate.cpp \
    $$PWD/qscriptecmafunction.cpp \
    $$PWD/qscriptecmaglobal.cpp \
    $$PWD/qscriptecmamath.cpp \
    $$PWD/qscriptecmanumber.cpp \
    $$PWD/qscriptecmaobject.cpp \
    $$PWD/qscriptecmaregexp.cpp \
    $$PWD/qscriptecmastring.cpp \
    $$PWD/qscriptecmaerror.cpp \
    $$PWD/qscriptcontext_p.cpp \
    $$PWD/qscriptengine.cpp \
    $$PWD/qscriptengine_p.cpp \
    $$PWD/qscriptengineagent.cpp \
    $$PWD/qscriptextenumeration.cpp \
    $$PWD/qscriptextvariant.cpp \
    $$PWD/qscriptcontext.cpp \
    $$PWD/qscriptcontextinfo.cpp \
    $$PWD/qscriptfunction.cpp \
    $$PWD/qscriptgrammar.cpp \
    $$PWD/qscriptlexer.cpp \
    $$PWD/qscriptclassdata.cpp \
    $$PWD/qscriptparser.cpp \
    $$PWD/qscriptprettypretty.cpp \
    $$PWD/qscriptxmlgenerator.cpp \
    $$PWD/qscriptsyntaxchecker.cpp \
    $$PWD/qscriptstring.cpp \
    $$PWD/qscriptclass.cpp \
    $$PWD/qscriptclasspropertyiterator.cpp \
    $$PWD/qscriptvalueiteratorimpl.cpp \
    $$PWD/qscriptvalueiterator.cpp \
    $$PWD/qscriptvalueimpl.cpp \
    $$PWD/qscriptvalue.cpp

HEADERS += \
    $$PWD/qscriptarray_p.h \
    $$PWD/qscriptasm_p.h \
    $$PWD/qscriptastfwd_p.h \
    $$PWD/qscriptast_p.h \
    $$PWD/qscriptastvisitor_p.h \
    $$PWD/qscriptbuffer_p.h \
    $$PWD/qscriptcompiler_p.h \
    $$PWD/qscriptcontext.h \
    $$PWD/qscriptcontextfwd_p.h \
    $$PWD/qscriptcontext_p.h \
    $$PWD/qscriptcontextinfo.h \
    $$PWD/qscriptcontextinfo_p.h \
    $$PWD/qscriptecmaarray_p.h \
    $$PWD/qscriptecmaboolean_p.h \
    $$PWD/qscriptecmacore_p.h \
    $$PWD/qscriptecmadate_p.h \
    $$PWD/qscriptecmafunction_p.h \
    $$PWD/qscriptecmaglobal_p.h \
    $$PWD/qscriptecmamath_p.h \
    $$PWD/qscriptecmanumber_p.h \
    $$PWD/qscriptecmaobject_p.h \
    $$PWD/qscriptecmaregexp_p.h \
    $$PWD/qscriptecmastring_p.h \
    $$PWD/qscriptecmaerror_p.h \
    $$PWD/qscriptengine.h \
    $$PWD/qscriptenginefwd_p.h \
    $$PWD/qscriptengine_p.h \
    $$PWD/qscriptengineagent.h \
    $$PWD/qscriptengineagent_p.h \
    $$PWD/qscriptable.h \
    $$PWD/qscriptable_p.h \
    $$PWD/qscriptextenumeration_p.h \
    $$PWD/qscriptextvariant_p.h \
    $$PWD/qscriptfunction_p.h \
    $$PWD/qscriptgc_p.h \
    $$PWD/qscriptglobals_p.h \
    $$PWD/qscriptgrammar_p.h \
    $$PWD/qscriptobjectdata_p.h \
    $$PWD/qscriptobjectfwd_p.h \
    $$PWD/qscriptobject_p.h \
    $$PWD/qscriptlexer_p.h \
    $$PWD/qscriptmemberfwd_p.h \
    $$PWD/qscriptmember_p.h \
    $$PWD/qscriptmemorypool_p.h \
    $$PWD/qscriptnodepool_p.h \
    $$PWD/qscriptclassinfo_p.h \
    $$PWD/qscriptparser_p.h \
    $$PWD/qscriptprettypretty_p.h \
    $$PWD/qscriptsyntaxcheckresult_p.h \
    $$PWD/qscriptxmlgenerator_p.h \
    $$PWD/qscriptrepository_p.h \
    $$PWD/qscriptsyntaxchecker_p.h \
    $$PWD/qscriptstring.h \
    $$PWD/qscriptstring_p.h \
    $$PWD/qscriptclass.h \
    $$PWD/qscriptclass_p.h \
    $$PWD/qscriptclasspropertyiterator.h \
    $$PWD/qscriptclasspropertyiterator_p.h \
    $$PWD/qscriptvalue.h \
    $$PWD/qscriptvaluefwd_p.h \
    $$PWD/qscriptvalue_p.h \
    $$PWD/qscriptvalueimplfwd_p.h \
    $$PWD/qscriptvalueimpl_p.h \
    $$PWD/qscriptvalueiteratorimpl_p.h \
    $$PWD/qscriptvalueiterator.h \
    $$PWD/qscriptvalueiterator_p.h \
    $$PWD/qscriptextensioninterface.h \
    $$PWD/qscriptextensionplugin.h \
    $$PWD/qscriptnameid_p.h \
    $$PWD/qscriptclassdata_p.h

!contains(DEFINES, QT_NO_QOBJECT) {
    HEADERS += $$PWD/qscriptextqobject_p.h
    SOURCES += $$PWD/qscriptextqobject.cpp \
               $$PWD/qscriptable.cpp \
               $$PWD/qscriptextensionplugin.cpp
} else {
    INCLUDEPATH += $$PWD
}
