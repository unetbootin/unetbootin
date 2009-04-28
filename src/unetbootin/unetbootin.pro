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
 unetbootin_es.ts \
 unetbootin_ru.ts \
 unetbootin_de.ts \
 unetbootin_he.ts \
 unetbootin_hu.ts \
 unetbootin_it.ts \
 unetbootin_zh.ts \
 unetbootin_fr.ts \
 unetbootin_ja.ts \
 unetbootin_vn.ts \
 unetbootin_uk.ts \
 unetbootin_be.ts \
 unetbootin_pt.ts \
 unetbootin_nb.ts \
 unetbootin_ca.ts
RESOURCES += unetbootin.qrc
