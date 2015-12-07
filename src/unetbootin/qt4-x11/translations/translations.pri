defineReplace(prependAll) {
    prepend = $$1
    arglist = $$2
    append  = $$3
    for(a,arglist) {
      result += $${prepend}$${a}$${append}
    }
    return ($$result)
}

defineReplace(fixPath) {
WIN {
    return ($$replace($$1, /, \))
} ELSE {
    return ($$1)
}
}

LUPDATE = $$fixPath($$QT_BUILD_TREE/bin/lupdate) -locations relative -no-ui-lines
LRELEASE = $$fixPath($$QT_BUILD_TREE/bin/lrelease)

###### Qt Libraries

QT_TS        = de fr zh_CN untranslated ar es iw ja_jp pl pt ru sk sv uk zh_TW 

ts-qt.commands = (cd $$QT_SOURCE_TREE/src && $$LUPDATE \
                                    3rdparty/phonon \
                                    3rdparty/webkit \
                                    activeqt \
                                    corelib \
                                    gui \
                                    network \
                                    opengl \
                                    plugins \
                                    qt3support \
                                    script \
                                    sql \
                                    svg \
                                    xml \
                                    xmlpatterns \
                                -ts $$prependAll($$[QT_INSTALL_TRANSLATIONS]/qt_,$$QT_TS,.ts))
ts-qt.depends = sub-tools

qm-qt.commands = $$LRELEASE $$prependAll($$[QT_INSTALL_TRANSLATIONS]/qt_,$$QT_TS,.ts)
qm-qt.depends = sub-tools

###### Designer

ts-designer.commands = (cd $$QT_SOURCE_TREE/src && $$LUPDATE \
                                    ../tools/designer/translations/translations.pro)
ts-designer.depends = sub-tools

qm-designer.commands = $$LRELEASE $$QT_SOURCE_TREE/tools/designer/translations/translations.pro
qm-designer.depends = sub-tools

###### Linguist

ts-linguist.commands = (cd $$QT_SOURCE_TREE/src && $$LUPDATE \
                                    ../tools/linguist/linguist/linguist.pro)
ts-linguist.depends = sub-tools

qm-linguist.commands = $$LRELEASE $$QT_SOURCE_TREE/tools/linguist/linguist/linguist.pro
qm-linguist.depends = sub-tools

###### Assistant

ts-assistant.commands = (cd $$QT_SOURCE_TREE/src && $$LUPDATE \
                                    ../tools/assistant/translations/translations.pro \
                                    && $$LUPDATE \
                                    ../tools/assistant/translations/qt_help.pro \
                                    && $$LUPDATE \
                                    ../tools/assistant/translations/translations_adp.pro)
ts-assistant.depends = sub-tools

qm-assistant.commands = ($$LRELEASE $$QT_SOURCE_TREE/tools/assistant/translations/translations.pro \
                         && $$LRELEASE \
                            $$QT_SOURCE_TREE/tools/assistant/translations/qt_help.pro \
                         && $$LRELEASE \
                            $$QT_SOURCE_TREE/tools/assistant/translations/translations_adp.pro)
qm-assistant.depends = sub-tools

###### Qtconfig

ts-qtconfig.commands = (cd $$QT_SOURCE_TREE/src && $$LUPDATE \
                                    ../tools/qtconfig/translations/translations.pro)
ts-qtconfig.depends = sub-tools

qm-qtconfig.commands = $$LRELEASE $$QT_SOURCE_TREE/tools/qtconfig/translations/translations.pro
qm-qtconfig.depends = sub-tools

###### Qvfp

ts-qvfb.commands = (cd $$QT_SOURCE_TREE/src && $$LUPDATE \
                                    ../tools/qvfb/translations/translations.pro)
ts-qvfb.depends = sub-tools

qm-qvfb.commands = $$LRELEASE $$QT_SOURCE_TREE/tools/qvfb/translations/translations.pro
qm-qvfb.depends = sub-tools

###### Overall Rules

ts.depends = ts-qt ts-designer ts-linguist ts-assistant ts-qtconfig ts-qvfb
qm.depends = qm-qt qm-designer qm-linguist qm-assistant qm-qtconfig qm-qvfb

QMAKE_EXTRA_TARGETS += ts-qt ts-designer ts-linguist ts-assistant ts-qtconfig ts-qvfb \
                       qm-qt qm-designer qm-linguist qm-assistant qm-qtconfig qm-qvfb \
                       ts qm
