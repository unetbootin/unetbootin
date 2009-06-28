TEMPLATE = app
TARGET = unetbootin
DEPENDPATH += .
INCLUDEPATH += .
CONFIG += release static
HEADERS += unetbootin.h
FORMS += unetbootin.ui
SOURCES += main.cpp unetbootin.cpp distrolst.cpp
QT += core gui network
TRANSLATIONS += unetbootin.ts \
 unetbootin_ar.ts \
 unetbootin_pt_BR.ts \
 unetbootin_cs.ts \
 unetbootin_es.ts \
 unetbootin_el.ts \
 unetbootin_gl.ts \
 unetbootin_ru.ts \
 unetbootin_ro.ts \
 unetbootin_tr.ts \
 unetbootin_de.ts \
 unetbootin_he.ts \
 unetbootin_hu.ts \
 unetbootin_it.ts \
 unetbootin_zh_CN.ts \
 unetbootin_zh_TW.ts \
 unetbootin_fr.ts \
 unetbootin_ja.ts \
 unetbootin_vi.ts \
 unetbootin_uk.ts \
 unetbootin_be.ts \
 unetbootin_pt.ts \
 unetbootin_nb.ts \
 unetbootin_nl.ts \
 unetbootin_ca.ts \
 unetbootin_sv.ts \
 unetbootin_pl.ts
RESOURCES += unetbootin.qrc
