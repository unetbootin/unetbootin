TEMPLATE      = subdirs
SUBDIRS       = recipes \
                xquery  \
                filetree

# This example depends on QtWebkit as well.
contains(QT_CONFIG, webkit):SUBDIRS += qobjectxmlmodel

# install
target.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS xmlpatterns.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/xmlpatterns
INSTALLS += target sources
