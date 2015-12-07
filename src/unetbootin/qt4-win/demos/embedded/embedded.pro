TEMPLATE  = subdirs
SUBDIRS   = embeddedsvgviewer \
            styledemo \
            fluidlauncher


# install
sources.files = README *.pro
sources.path = $$[QT_INSTALL_DEMOS]/embedded
INSTALLS += sources

