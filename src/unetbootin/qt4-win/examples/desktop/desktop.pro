TEMPLATE      = subdirs
CONFIG       += ordered
SUBDIRS       = screenshot

contains(QT_CONFIG, svg): SUBDIRS += systray

# install
target.path = $$[QT_INSTALL_EXAMPLES]/desktop
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS desktop.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/desktop
INSTALLS += target sources
