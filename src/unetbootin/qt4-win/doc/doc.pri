#####################################################################
# Qt documentation build
#####################################################################

win32 {
    QT_WINCONFIG = release/
    CONFIG(debug, debug|release) {
        QT_WINCONFIG = debug/
    }
}

DOCS_GENERATION_DEFINES = -Dopensourceedition
GENERATOR = $$QT_BUILD_TREE/bin/qhelpgenerator

win32:isEmpty(QMAKE_SH) {
        QDOC = cd $$QT_SOURCE_TREE/tools/qdoc3/test && set QT_BUILD_TREE=$$QT_BUILD_TREE&& set QT_SOURCE_TREE=$$QT_SOURCE_TREE&& $$QT_BUILD_TREE/tools/qdoc3/$${QT_WINCONFIG}qdoc3 $$DOCS_GENERATION_DEFINES
        QDOC = $$replace(QDOC, "/", "\\\\")
} else {
    QDOC = cd $$QT_SOURCE_TREE/tools/qdoc3/test && QT_BUILD_TREE=$$QT_BUILD_TREE QT_SOURCE_TREE=$$QT_SOURCE_TREE $$QT_BUILD_TREE/tools/qdoc3/$${QT_WINCONFIG}qdoc3 $$DOCS_GENERATION_DEFINES
}
macx {
    ADP_DOCS_QDOCCONF_FILE = qt-build-docs-with-xcode.qdocconf
} else {
    ADP_DOCS_QDOCCONF_FILE = qt-build-docs.qdocconf
}
QT_DOCUMENTATION = ($$QDOC qt-api-only.qdocconf assistant.qdocconf designer.qdocconf \
                    linguist.qdocconf qmake.qdocconf) && \
               (cd $$QT_BUILD_TREE && \
                    $$GENERATOR doc-build/html-qt/qt.qhp -o doc/qch/qt.qch && \
                    $$GENERATOR doc-build/html-assistant/assistant.qhp -o doc/qch/assistant.qch && \
                    $$GENERATOR doc-build/html-designer/designer.qhp -o doc/qch/designer.qch && \
                    $$GENERATOR doc-build/html-linguist/linguist.qhp -o doc/qch/linguist.qch && \
                    $$GENERATOR doc-build/html-qmake/qmake.qhp -o doc/qch/qmake.qch \
               )

win32-g++:isEmpty(QMAKE_SH) {
	QT_DOCUMENTATION = $$replace(QT_DOCUMENTATION, "/", "\\\\")
}


!wince*:!cross_compile:SUBDIRS += tools/qdoc3

# Build rules:
adp_docs.commands = ($$QDOC $$ADP_DOCS_QDOCCONF_FILE)
adp_docs.depends += sub-tools-qdoc3
qch_docs.commands = $$QT_DOCUMENTATION
qch_docs.depends += sub-tools

docs.depends = adp_docs qch_docs

# Install rules
htmldocs.files = $$QT_BUILD_TREE/doc/html
htmldocs.path = $$[QT_INSTALL_DOCS]
htmldocs.CONFIG += no_check_exist

qchdocs.files= $$QT_BUILD_TREE/doc/qch
qchdocs.path = $$[QT_INSTALL_DOCS]
qchdocs.CONFIG += no_check_exist

docimages.files = $$QT_BUILD_TREE/doc/src/images
docimages.path = $$[QT_INSTALL_DOCS]/src

QMAKE_EXTRA_TARGETS += qdoc adp_docs qch_docs docs
INSTALLS += htmldocs qchdocs docimages
