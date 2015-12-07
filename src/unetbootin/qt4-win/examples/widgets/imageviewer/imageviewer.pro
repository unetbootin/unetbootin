HEADERS       = imageviewer.h
SOURCES       = imageviewer.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/imageviewer
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS imageviewer.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/imageviewer
INSTALLS += target sources

wince*: {
   DEPLOYMENT_PLUGIN += qjpeg qmng qgif
}
