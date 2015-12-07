TEMPLATE      = \
              subdirs
SUBDIRS       = \
              elasticnodes \
              collidingmice \
              diagramscene \
              dragdroprobot \
              padnavigator \
              basicgraphicslayouts

contains(QT_CONFIG, qt3support):SUBDIRS += portedcanvas portedasteroids
contains(DEFINES, QT_NO_CURSOR): SUBDIRS -= dragdroprobot

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS graphicsview.pro README
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview
INSTALLS += target sources
