TEMPLATE = app
TARGET = unetbootin
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += release \
    static

macx {
    CONFIG -= x86_64
    CONFIG += x86
    QMAKE_CFLAGS_PPC_64     -= -arch ppc64 -Xarch_ppc64 -mmacosx-version-min=10.5
    QMAKE_OBJECTIVE_CFLAGS_PPC_64  -= -arch ppc64 -Xarch_ppc64 -mmacosx-version-min=10.5
    QMAKE_CFLAGS_X86_64     -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
    QMAKE_OBJECTIVE_CFLAGS_X86_64  -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
    QMAKE_CXXFLAGS_PPC_64   -= -arch ppc64 -Xarch_ppc64 -mmacosx-version-min=10.5
    QMAKE_CXXFLAGS_X86_64   -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
    QMAKE_LFLAGS_PPC_64     -= -arch ppc64 -Xarch_ppc64 -mmacosx-version-min=10.5
    QMAKE_LFLAGS_X86_64     -= -arch x86_64 -Xarch_x86_64 -mmacosx-version-min=10.5
}

HEADERS += unetbootin.h
FORMS += unetbootin.ui
SOURCES += main.cpp \
    unetbootin.cpp \
    distrolst.cpp \
    distrover.cpp \
    distrovercust.cpp
QT += core \
    gui \
    network
ICON = unetbootin.icns
TRANSLATIONS += unetbootin.ts \
    unetbootin_am.ts \
    unetbootin_ar.ts \
    unetbootin_ast.ts \
    unetbootin_bg.ts \
    unetbootin_bn.ts \
    unetbootin_pt_BR.ts \
    unetbootin_cs.ts \
    unetbootin_ur.ts \
    unetbootin_da.ts \
    unetbootin_es.ts \
    unetbootin_et.ts \
    unetbootin_el.ts \
    unetbootin_eo.ts \
    unetbootin_eu.ts \
    unetbootin_fi.ts \
    unetbootin_fo.ts \
    unetbootin_gl.ts \
    unetbootin_hr.ts \
    unetbootin_ru.ts \
    unetbootin_ro.ts \
    unetbootin_tr.ts \
    unetbootin_de.ts \
    unetbootin_he.ts \
    unetbootin_hu.ts \
    unetbootin_id.ts \
    unetbootin_it.ts \
    unetbootin_lt.ts \
    unetbootin_lv.ts \
    unetbootin_zh_CN.ts \
    unetbootin_zh_TW.ts \
    unetbootin_fr.ts \
    unetbootin_ja.ts \
    unetbootin_vi.ts \
    unetbootin_uk.ts \
    unetbootin_be.ts \
    unetbootin_pt.ts \
    unetbootin_ms.ts \
    unetbootin_ml.ts \
    unetbootin_nan.ts \
    unetbootin_nb.ts \
    unetbootin_nl.ts \
    unetbootin_nn.ts \
    unetbootin_ca.ts \
    unetbootin_sk.ts \
    unetbootin_sl.ts \
    unetbootin_sr.ts \
    unetbootin_sv.ts \
    unetbootin_sw.ts \
    unetbootin_pl.ts \
    unetbootin_fa.ts \
    unetbootin_si.ts
RESOURCES += unetbootin.qrc
