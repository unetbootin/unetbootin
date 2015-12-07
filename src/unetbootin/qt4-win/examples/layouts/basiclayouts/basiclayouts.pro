HEADERS     = dialog.h
SOURCES     = dialog.cpp \
              main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/layouts/basiclayouts
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/layouts/basiclayouts
INSTALLS += target sources
