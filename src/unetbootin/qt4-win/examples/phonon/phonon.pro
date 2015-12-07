TEMPLATE      = subdirs
CONFIG       += ordered
SUBDIRS       = musicplayer \
                capabilities

# install
target.path = $$[QT_INSTALL_EXAMPLES]/phonon
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS phonon.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/phonon
INSTALLS += target sources
