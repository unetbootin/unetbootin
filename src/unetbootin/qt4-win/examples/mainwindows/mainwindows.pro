TEMPLATE      = subdirs
SUBDIRS       = application \
                dockwidgets \
                mdi \
                menus \
                recentfiles \
                sdi

# install
target.path = $$[QT_INSTALL_EXAMPLES]/mainwindows
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS mainwindows.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/mainwindows
INSTALLS += target sources
